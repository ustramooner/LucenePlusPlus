/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "TestInc.h"
#include "LuceneTestFixture.h"
#include "TestUtils.h"
#include "SimpleFSDirectory.h"
#include "_SimpleFSDirectory.h"
#include "IndexOutput.h"
#include "IndexInput.h"
#include "CompoundFileWriter.h"
#include "CompoundFileReader.h"
#include "Random.h"
#include "MiscUtils.h"
#include "FileUtils.h"

using namespace Lucene;

class CompoundFileTestFixture : public LuceneTestFixture
{
public:
    CompoundFileTestFixture()
    {
        indexDir = FileUtils::joinPath(getTempDir(), L"testIndex");
        FileUtils::removeDirectory(indexDir);
        
        // use a simple FSDir here, to be sure to have SimpleFSInputs
        dir = newLucene<SimpleFSDirectory>(indexDir);
    }
    
    virtual ~CompoundFileTestFixture()
    {
        dir->close();
        FileUtils::removeDirectory(indexDir);
    }

protected:
    String indexDir;
    DirectoryPtr dir;

public:
    /// Creates a file of the specified size with random data.
    void createRandomFile(DirectoryPtr dir, const String& name, int32_t size)
    {
        IndexOutputPtr os = dir->createOutput(name);
        RandomPtr r = newLucene<Random>();
        for (int32_t i = 0; i < size; ++i)
            os->writeByte((uint8_t)r->nextInt(256));
        os->close();
    }
    
    void createSequenceFile(DirectoryPtr dir, const String& name, uint8_t start, int32_t size)
    {
        IndexOutputPtr os = dir->createOutput(name);
        for (int32_t i = 0; i < size; ++i)
        {
            os->writeByte(start);
            ++start;
        }
        os->close();
    }
    
    void checkSameStreams(IndexInputPtr expected, IndexInputPtr test)
    {
        BOOST_CHECK(expected);
        BOOST_CHECK(test);
        BOOST_CHECK_EQUAL(expected->length(), test->length());
        BOOST_CHECK_EQUAL(expected->getFilePointer(), test->getFilePointer());
        
        ByteArray expectedBuffer(ByteArray::newInstance(512));
        ByteArray testBuffer(ByteArray::newInstance(expectedBuffer.size()));

        int64_t remainder = expected->length() - expected->getFilePointer();
        while (remainder > 0)
        {
            int32_t readLen = std::min((int32_t)remainder, expectedBuffer.size());
            expected->readBytes(expectedBuffer.get(), 0, readLen);
            test->readBytes(testBuffer.get(), 0, readLen);
            checkEqualArrays(expectedBuffer, testBuffer, 0, readLen);
            remainder -= readLen;
        }
    }
    
    void checkSameStreams(IndexInputPtr expected, IndexInputPtr actual, int64_t seekTo)
    {
        if (seekTo >= 0 && seekTo < (int64_t)expected->length())
        {
            expected->seek(seekTo);
            actual->seek(seekTo);
            checkSameStreams(expected, actual);
        }
    }
    
    void checkSameSeekBehavior(IndexInputPtr expected, IndexInputPtr actual)
    {
        // seek to 0
        int64_t point = 0;
        checkSameStreams(expected, actual, point);

        // seek to middle
        point = expected->length() / 2l;
        checkSameStreams(expected, actual, point);

        // seek to end - 2
        point = expected->length() - 2;
        checkSameStreams(expected, actual, point);

        // seek to end - 1
        point = expected->length() - 1;
        checkSameStreams(expected, actual, point);

        // seek to the end
        point = expected->length();
        checkSameStreams(expected, actual, point);

        // seek past end
        point = expected->length() + 1;
        checkSameStreams(expected, actual, point);
    }
    
    void checkEqualArrays(ByteArray expected, ByteArray test, int32_t start, int32_t length)
    {
        BOOST_CHECK(expected);
        BOOST_CHECK(test);
        for (int32_t i = start; i < length; ++i)
            BOOST_CHECK_EQUAL(expected[i], test[i]);
    }
    
    /// Setup a larger compound file with a number of components, each of which is a sequential file (so that we can 
    /// easily tell that we are reading in the right byte). The methods sets up 20 files - f0 to f19, the size of each 
    /// file is 1000 bytes.
    void setUpLarger()
    {
        CompoundFileWriterPtr cw = newLucene<CompoundFileWriter>(dir, L"f.comp");
        for (int32_t i = 0; i < 20; ++i)
        {
            createSequenceFile(dir, L"f" + StringUtils::toString(i), 0, 2000);
            cw->addFile(L"f" + StringUtils::toString(i));
        }
        cw->close();
    }
    
    bool isCSIndexInputOpen(IndexInputPtr is)
    {
        if (MiscUtils::typeOf<CSIndexInput>(is))
        {
            CSIndexInputPtr cis = boost::dynamic_pointer_cast<CSIndexInput>(is);
            return isSimpleFSIndexInputOpen(cis->base);
        }
        else
            return false;
    }
    
    bool isSimpleFSIndexInputOpen(IndexInputPtr is)
    {
        if (MiscUtils::typeOf<SimpleFSIndexInput>(is))
        {
            SimpleFSIndexInputPtr fis = boost::dynamic_pointer_cast<SimpleFSIndexInput>(is);
            return fis->isValid();
        }
        else
            return false;
    }
};

BOOST_FIXTURE_TEST_SUITE(CompoundFileTest, CompoundFileTestFixture)

/// This test creates compound file based on a single file.  Files of different sizes are tested: 0, 1, 10, 100 bytes.
BOOST_AUTO_TEST_CASE(testSingleFile)
{
    IntArray data(IntArray::newInstance(4));
    data[0] = 0;
    data[1] = 1;
    data[2] = 10;
    data[3] = 100;
    for (int32_t i = 0; i < data.size(); ++i)
    {
        String name = L"t" + StringUtils::toString(data[i]);
        createSequenceFile(dir, name, 0, data[i]);
        CompoundFileWriterPtr csw = newLucene<CompoundFileWriter>(dir, name + L".cfs");
        csw->addFile(name);
        csw->close();

        CompoundFileReaderPtr csr = newLucene<CompoundFileReader>(dir, name + L".cfs");
        IndexInputPtr expected = dir->openInput(name);
        IndexInputPtr actual = csr->openInput(name);
        checkSameStreams(expected, actual);
        checkSameSeekBehavior(expected, actual);
        expected->close();
        actual->close();
        csr->close();
    }
}

/// This test creates compound file based on two files.
BOOST_AUTO_TEST_CASE(testTwoFiles)
{
    createSequenceFile(dir, L"d1", 0, 15);
    createSequenceFile(dir, L"d2", 0, 114);

    CompoundFileWriterPtr csw = newLucene<CompoundFileWriter>(dir, L"d.csf");
    csw->addFile(L"d1");
    csw->addFile(L"d2");
    csw->close();

    CompoundFileReaderPtr csr = newLucene<CompoundFileReader>(dir, L"d.csf");
    IndexInputPtr expected = dir->openInput(L"d1");
    IndexInputPtr actual = csr->openInput(L"d1");
    checkSameStreams(expected, actual);
    checkSameSeekBehavior(expected, actual);
    expected->close();
    actual->close();

    expected = dir->openInput(L"d2");
    actual = csr->openInput(L"d2");
    checkSameStreams(expected, actual);
    checkSameSeekBehavior(expected, actual);
    expected->close();
    actual->close();
    csr->close();
}

/// This test creates a compound file based on a large number of files of various length. The file content is generated randomly. 
/// The sizes range from 0 to 1Mb.  Some of the sizes are selected to test the buffering logic in the file reading code. 
/// For this the chunk variable is set to the length of the buffer used internally by the compound file logic.
BOOST_AUTO_TEST_CASE(testRandomFiles)
{
    // Setup the test segment
    String segment = L"test";
    int32_t chunk = 1024; // internal buffer size used by the stream
    createRandomFile(dir, segment + L".zero", 0);
    createRandomFile(dir, segment + L".one", 1);
    createRandomFile(dir, segment + L".ten", 10);
    createRandomFile(dir, segment + L".hundred", 100);
    createRandomFile(dir, segment + L".big1", chunk);
    createRandomFile(dir, segment + L".big2", chunk - 1);
    createRandomFile(dir, segment + L".big3", chunk + 1);
    createRandomFile(dir, segment + L".big4", 3 * chunk);
    createRandomFile(dir, segment + L".big5", 3 * chunk - 1);
    createRandomFile(dir, segment + L".big6", 3 * chunk + 1);
    createRandomFile(dir, segment + L".big7", 1000 * chunk);

    // Setup extraneous files
    createRandomFile(dir, L"onetwothree", 100);
    createRandomFile(dir, segment + L".notIn", 50);
    createRandomFile(dir, segment + L".notIn2", 51);

    // Now test
    CompoundFileWriterPtr csw = newLucene<CompoundFileWriter>(dir, L"test.cfs");
    
    Collection<String> data(Collection<String>::newInstance());
    data.add(L".zero");    data.add(L".one"); data.add(L".ten");
    data.add(L".hundred"); data.add(L".big1"); data.add(L".big2");
    data.add(L".big3"); data.add(L".big4"); data.add(L".big5");
    data.add(L".big6"); data.add(L".big7");
    
    for (Collection<String>::iterator name = data.begin(); name != data.end(); ++name)
        csw->addFile(segment + *name);
    csw->close();
    
    CompoundFileReaderPtr csr = newLucene<CompoundFileReader>(dir, L"test.cfs");
    for (Collection<String>::iterator name = data.begin(); name != data.end(); ++name)
    {
        IndexInputPtr check = dir->openInput(segment + *name);
        IndexInputPtr test = csr->openInput(segment + *name);
        checkSameStreams(check, test);
        checkSameSeekBehavior(check, test);
        test->close();
        check->close();
    }
    csr->close();
}

BOOST_AUTO_TEST_CASE(testReadAfterClose)
{
    // Setup the test file - we need more than 1024 bytes
    IndexOutputPtr os = dir->createOutput(L"test");
    for (int32_t i = 0; i < 2000; ++i)
        os->writeByte((uint8_t)i);
    os->close();

    IndexInputPtr in = dir->openInput(L"test");

    // This read primes the buffer in IndexInput
    uint8_t b = in->readByte();

    // Close the file
    in->close();

    // ERROR: this call should fail, but succeeds because the buffer is still filled
    b = in->readByte();

    // ERROR: this call should fail, but succeeds for some reason as well
    in->seek(1099);
    
    BOOST_CHECK_EXCEPTION(in->readByte(), LuceneException, check_exception(LuceneException::IO));
}

BOOST_AUTO_TEST_CASE(testClonedStreamsClosing)
{
    setUpLarger();
    
    CompoundFileReaderPtr cr = newLucene<CompoundFileReader>(dir, L"f.comp");

    // basic clone
    IndexInputPtr expected = dir->openInput(L"f11");

    // this test only works for FSIndexInput
    BOOST_CHECK(MiscUtils::typeOf<SimpleFSIndexInput>(expected));
    BOOST_CHECK(isSimpleFSIndexInputOpen(expected));

    IndexInputPtr one = cr->openInput(L"f11");
    BOOST_CHECK(isCSIndexInputOpen(one));

    IndexInputPtr two = boost::dynamic_pointer_cast<IndexInput>(one->clone());
    BOOST_CHECK(isCSIndexInputOpen(two));

    checkSameStreams(expected, one);
    expected->seek(0);
    checkSameStreams(expected, two);

    // Now close the first stream
    one->close();
    BOOST_CHECK(isCSIndexInputOpen(one)); // Only close when cr is closed

    // The following should really fail since we couldn't expect to access a file once close has been called 
    // on it (regardless of buffering and/or clone magic)
    expected->seek(0);
    two->seek(0);
    checkSameStreams(expected, two); // basic clone two/2

    // Now close the compound reader
    cr->close();
    BOOST_CHECK(!isCSIndexInputOpen(one));
    BOOST_CHECK(!isCSIndexInputOpen(two));

    // The following may also fail since the compound stream is closed
    expected->seek(0);
    two->seek(0);

    // Now close the second clone
    two->close();
    expected->seek(0);
    two->seek(0);

    expected->close();
}

/// This test opens two files from a compound stream and verifies that their file positions are independent of each other.
BOOST_AUTO_TEST_CASE(testRandomAccess)
{
    setUpLarger();

    CompoundFileReaderPtr cr = newLucene<CompoundFileReader>(dir, L"f.comp");

    // Open two files
    IndexInputPtr e1 = dir->openInput(L"f11");
    IndexInputPtr e2 = dir->openInput(L"f3");

    IndexInputPtr a1 = cr->openInput(L"f11");
    IndexInputPtr a2 = dir->openInput(L"f3");

    // Seek the first pair
    e1->seek(100);
    a1->seek(100);
    BOOST_CHECK_EQUAL(100, e1->getFilePointer());
    BOOST_CHECK_EQUAL(100, a1->getFilePointer());
    uint8_t be1 = e1->readByte();
    uint8_t ba1 = a1->readByte();
    BOOST_CHECK_EQUAL(be1, ba1);

    // Now seek the second pair
    e2->seek(1027);
    a2->seek(1027);
    BOOST_CHECK_EQUAL(1027, e2->getFilePointer());
    BOOST_CHECK_EQUAL(1027, a2->getFilePointer());
    uint8_t be2 = e2->readByte();
    uint8_t ba2 = a2->readByte();
    BOOST_CHECK_EQUAL(be2, ba2);

    // Now make sure the first one didn't move
    BOOST_CHECK_EQUAL(101, e1->getFilePointer());
    BOOST_CHECK_EQUAL(101, a1->getFilePointer());
    be1 = e1->readByte();
    ba1 = a1->readByte();
    BOOST_CHECK_EQUAL(be1, ba1);

    // Now more the first one again, past the buffer length
    e1->seek(1910);
    a1->seek(1910);
    BOOST_CHECK_EQUAL(1910, e1->getFilePointer());
    BOOST_CHECK_EQUAL(1910, a1->getFilePointer());
    be1 = e1->readByte();
    ba1 = a1->readByte();
    BOOST_CHECK_EQUAL(be1, ba1);

    // Now make sure the second set didn't move
    BOOST_CHECK_EQUAL(1028, e2->getFilePointer());
    BOOST_CHECK_EQUAL(1028, a2->getFilePointer());
    be2 = e2->readByte();
    ba2 = a2->readByte();
    BOOST_CHECK_EQUAL(be2, ba2);

    // Move the second set back, again cross the buffer size
    e2->seek(17);
    a2->seek(17);
    BOOST_CHECK_EQUAL(17, e2->getFilePointer());
    BOOST_CHECK_EQUAL(17, a2->getFilePointer());
    be2 = e2->readByte();
    ba2 = a2->readByte();
    BOOST_CHECK_EQUAL(be2, ba2);

    // Finally, make sure the first set didn't move
    // Now make sure the first one didn't move
    BOOST_CHECK_EQUAL(1911, e1->getFilePointer());
    BOOST_CHECK_EQUAL(1911, a1->getFilePointer());
    be1 = e1->readByte();
    ba1 = a1->readByte();
    BOOST_CHECK_EQUAL(be1, ba1);

    e1->close();
    e2->close();
    a1->close();
    a2->close();
    cr->close();
}

/// This test opens two files from a compound stream and verifies that their file positions are independent of each other.
BOOST_AUTO_TEST_CASE(testRandomAccessClones)
{
    setUpLarger();

    CompoundFileReaderPtr cr = newLucene<CompoundFileReader>(dir, L"f.comp");

    // Open two files
    IndexInputPtr e1 = cr->openInput(L"f11");
    IndexInputPtr e2 = cr->openInput(L"f3");

    IndexInputPtr a1 = boost::dynamic_pointer_cast<IndexInput>(e1->clone());
    IndexInputPtr a2 = boost::dynamic_pointer_cast<IndexInput>(e2->clone());
    
    // Seek the first pair
    e1->seek(100);
    a1->seek(100);
    BOOST_CHECK_EQUAL(100, e1->getFilePointer());
    BOOST_CHECK_EQUAL(100, a1->getFilePointer());
    uint8_t be1 = e1->readByte();
    uint8_t ba1 = a1->readByte();
    BOOST_CHECK_EQUAL(be1, ba1);

    // Now seek the second pair
    e2->seek(1027);
    a2->seek(1027);
    BOOST_CHECK_EQUAL(1027, e2->getFilePointer());
    BOOST_CHECK_EQUAL(1027, a2->getFilePointer());
    uint8_t be2 = e2->readByte();
    uint8_t ba2 = a2->readByte();
    BOOST_CHECK_EQUAL(be2, ba2);

    // Now make sure the first one didn't move
    BOOST_CHECK_EQUAL(101, e1->getFilePointer());
    BOOST_CHECK_EQUAL(101, a1->getFilePointer());
    be1 = e1->readByte();
    ba1 = a1->readByte();
    BOOST_CHECK_EQUAL(be1, ba1);

    // Now more the first one again, past the buffer length
    e1->seek(1910);
    a1->seek(1910);
    BOOST_CHECK_EQUAL(1910, e1->getFilePointer());
    BOOST_CHECK_EQUAL(1910, a1->getFilePointer());
    be1 = e1->readByte();
    ba1 = a1->readByte();
    BOOST_CHECK_EQUAL(be1, ba1);

    // Now make sure the second set didn't move
    BOOST_CHECK_EQUAL(1028, e2->getFilePointer());
    BOOST_CHECK_EQUAL(1028, a2->getFilePointer());
    be2 = e2->readByte();
    ba2 = a2->readByte();
    BOOST_CHECK_EQUAL(be2, ba2);

    // Move the second set back, again cross the buffer size
    e2->seek(17);
    a2->seek(17);
    BOOST_CHECK_EQUAL(17, e2->getFilePointer());
    BOOST_CHECK_EQUAL(17, a2->getFilePointer());
    be2 = e2->readByte();
    ba2 = a2->readByte();
    BOOST_CHECK_EQUAL(be2, ba2);

    // Finally, make sure the first set didn't move
    // Now make sure the first one didn't move
    BOOST_CHECK_EQUAL(1911, e1->getFilePointer());
    BOOST_CHECK_EQUAL(1911, a1->getFilePointer());
    be1 = e1->readByte();
    ba1 = a1->readByte();
    BOOST_CHECK_EQUAL(be1, ba1);

    e1->close();
    e2->close();
    a1->close();
    a2->close();
    cr->close();
}

BOOST_AUTO_TEST_CASE(testFileNotFound)
{
    setUpLarger();

    CompoundFileReaderPtr cr = newLucene<CompoundFileReader>(dir, L"f.comp");
    IndexInputPtr e1;
    
    // Open two files
    BOOST_CHECK_EXCEPTION(e1 = cr->openInput(L"bogus"), LuceneException, check_exception(LuceneException::IO));
    
    cr->close();
}

BOOST_AUTO_TEST_CASE(testReadPastEOF)
{
    setUpLarger();

    CompoundFileReaderPtr cr = newLucene<CompoundFileReader>(dir, L"f.comp");
    IndexInputPtr is = cr->openInput(L"f2");
    is->seek(is->length() - 10);
    ByteArray b(ByteArray::newInstance(100));
    is->readBytes(b.get(), 0, 10);
    uint8_t test = 0;
    
    BOOST_CHECK_EXCEPTION(test = is->readByte(), LuceneException, check_exception(LuceneException::IO));
    
    is->seek(is->length() - 10);
    
    BOOST_CHECK_EXCEPTION(is->readBytes(b.get(), 0, 50), LuceneException, check_exception(LuceneException::IO));
    
    is->close();
    cr->close();
}

/// This test that writes larger than the size of the buffer output will correctly increment the file pointer.
BOOST_AUTO_TEST_CASE(testLargeWrites)
{
    IndexOutputPtr os = dir->createOutput(L"testBufferStart.txt");
    RandomPtr r = newLucene<Random>();
    
    ByteArray largeBuf(ByteArray::newInstance(2048));
    for (int32_t i = 0; i < largeBuf.size(); ++i)
        largeBuf[i] = (uint8_t)r->nextInt(256);
    
    int64_t currentPos = os->getFilePointer();
    os->writeBytes(largeBuf.get(), largeBuf.size());
    
    BOOST_CHECK_EQUAL(currentPos + largeBuf.size(), os->getFilePointer());
    
    os->close();
}

BOOST_AUTO_TEST_SUITE_END()
