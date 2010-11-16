/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "TestInc.h"
#include "LuceneTestFixture.h"
#include "IndexOutput.h"
#include "IndexInput.h"

using namespace Lucene;

BOOST_FIXTURE_TEST_SUITE(IndexOutputTest, LuceneTestFixture)

class TestableIndexOutput : public IndexOutput
{
public:
    using IndexOutput::writeBytes;
    
    TestableIndexOutput(uint8_t* b, int32_t length) : outputBytes(b), outputLength(length), nextByte(0)
    {
    }
    
    virtual ~TestableIndexOutput()
    {
    }
    
    virtual void writeByte(uint8_t b)
    {
        outputBytes[nextByte++] = b;
    }
    
    virtual void writeBytes(const uint8_t* b, int32_t offset, int32_t length)
    {
        std::copy(b + offset, b + offset + length, outputBytes + nextByte + offset);
        nextByte += length;
    }
    
    virtual void flush()
    {
    }
    
    virtual void close()
    {
    }

    virtual int64_t getFilePointer()
    {
        return 0;
    }
    
    virtual void seek(int64_t pos)
    {
    }
    
    virtual int64_t length()
    {
        return 0;
    }
    
    int32_t getNextPosition()
    {
        return nextByte;
    }

protected:
    uint8_t* outputBytes;
    int32_t outputLength;
    int32_t nextByte;
};

BOOST_AUTO_TEST_CASE(testWriteInt)
{
    ByteArray outputBytes(ByteArray::newInstance(10));
    TestableIndexOutput indexOutput(outputBytes.get(), 10);
    indexOutput.writeInt(1234);
    BOOST_CHECK_EQUAL(indexOutput.getNextPosition(), 4);
    uint8_t expected[4] = { 0, 0, 4, 210 };
    BOOST_CHECK_EQUAL(memcmp(outputBytes.get(), expected, 4), 0);
}

BOOST_AUTO_TEST_CASE(testWriteVInt)
{
    ByteArray outputBytes(ByteArray::newInstance(10));
    TestableIndexOutput indexOutput(outputBytes.get(), 10);
    indexOutput.writeVInt(1234);
    BOOST_CHECK_EQUAL(indexOutput.getNextPosition(), 2);
    uint8_t expected[2] = { 210, 9 };
    BOOST_CHECK_EQUAL(memcmp(outputBytes.get(), expected, 2), 0);
}

BOOST_AUTO_TEST_CASE(testWriteLong)
{
    ByteArray outputBytes(ByteArray::newInstance(10));
    TestableIndexOutput indexOutput(outputBytes.get(), 10);
    indexOutput.writeLong(1234123412341234LL);
    BOOST_CHECK_EQUAL(indexOutput.getNextPosition(), 8);
    uint8_t expected[8] = { 0, 4, 98, 109, 191, 154, 1, 242 };
    BOOST_CHECK_EQUAL(memcmp(outputBytes.get(), expected, 8), 0);
}

BOOST_AUTO_TEST_CASE(testWriteVLong)
{
    ByteArray outputBytes(ByteArray::newInstance(10));
    TestableIndexOutput indexOutput(outputBytes.get(), 10);
    indexOutput.writeVLong(1234123412341234LL);
    BOOST_CHECK_EQUAL(indexOutput.getNextPosition(), 8);
    uint8_t expected[8] = { 242, 131, 232, 252, 219, 205, 152, 2 };
    BOOST_CHECK_EQUAL(memcmp(outputBytes.get(), expected, 8), 0);
}

BOOST_AUTO_TEST_CASE(testWriteString)
{
    ByteArray outputBytes(ByteArray::newInstance(30));
    TestableIndexOutput indexOutput(outputBytes.get(), 30);
    indexOutput.writeString(L"test string");
    BOOST_CHECK_EQUAL(indexOutput.getNextPosition(), 12);
    uint8_t expected[12] = { 11, 116, 101, 115, 116, 32, 115, 116, 114, 105, 110, 103 };
    BOOST_CHECK_EQUAL(memcmp(outputBytes.get(), expected, 12), 0);
}

BOOST_AUTO_TEST_CASE(testWriteChars)
{
    ByteArray outputBytes(ByteArray::newInstance(30));
    TestableIndexOutput indexOutput(outputBytes.get(), 30);
    indexOutput.writeChars(L"test string", 5, 6);
    BOOST_CHECK_EQUAL(indexOutput.getNextPosition(), 6);
    uint8_t expected[6] = { 115, 116, 114, 105, 110, 103 };
    BOOST_CHECK_EQUAL(memcmp(outputBytes.get(), expected, 6), 0);
}

namespace TestCopyBytes
{
    class SourceIndexInput : public IndexInput
    {
    public:
        SourceIndexInput(const uint8_t* b, int32_t length) : inputBytes(b), inputLength(length), nextByte(0)
        {
        }
        
        virtual uint8_t readByte()
        {
            return 0;
        }
        
        virtual void readBytes(uint8_t* b, int32_t offset, int32_t length)
        {
            std::copy(inputBytes + nextByte + offset, inputBytes + nextByte + offset + length, b + offset);
            nextByte += length;
        }
        
        virtual void close()
        {
        }

        virtual int64_t getFilePointer()
        {
            return 0;
        }
        
        virtual void seek(int64_t pos)
        {
        }
        
        virtual int64_t length()
        {
            return 0;
        }
        
        virtual IndexInputPtr clone()
        {
            return IndexInputPtr();
        }

    protected:
        const uint8_t* inputBytes;
        int32_t inputLength;
        int32_t nextByte;
    };
}

BOOST_AUTO_TEST_CASE(testCopyBytes)
{
    ByteArray sourceBytes(ByteArray::newInstance(32768));
    std::generate(sourceBytes.get(), sourceBytes.get() + 32768, rand);
    IndexInputPtr indexSource(newLucene<TestCopyBytes::SourceIndexInput>(sourceBytes.get(), 32768));
    
    ByteArray outputBytes(ByteArray::newInstance(32768));
    TestableIndexOutput indexOutput(outputBytes.get(), 32768);
    
    indexOutput.copyBytes(indexSource, 20000);
    
    BOOST_CHECK_EQUAL(memcmp(outputBytes.get(), sourceBytes.get(), 20000), 0);
}

BOOST_AUTO_TEST_SUITE_END()
