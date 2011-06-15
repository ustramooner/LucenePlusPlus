/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "TestInc.h"
#include "LuceneTestFixture.h"
#include "TestUtils.h"
#include "FileReader.h"
#include "InputStreamReader.h"
#include "BufferedReader.h"
#include "FileUtils.h"

using namespace Lucene;

BOOST_FIXTURE_TEST_SUITE(InputStreamReaderTest, LuceneTestFixture)

BOOST_AUTO_TEST_CASE(testInputStreamReaderChar)
{
    InputStreamReaderPtr stream = newLucene<InputStreamReader>(newLucene<FileReader>(FileUtils::joinPath(getTestDir(), L"testfile_text.txt")));
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), L't');
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), L'e');
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), L's');
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), L't');
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), L' ');
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), L'f');
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), L'i');
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), L'l');
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), L'e');
}

BOOST_AUTO_TEST_CASE(testInputStreamReaderCharUtf8)
{
    InputStreamReaderPtr stream = newLucene<InputStreamReader>(newLucene<FileReader>(FileUtils::joinPath(getTestDir(), L"testfile_uft8.txt")));
    
    const uint8_t chinese[] = {0xe4, 0xb8, 0xad, 0xe5, 0x8d, 0x8e, 0xe4, 0xba, 0xba, 0xe6, 0xb0, 
                               0x91, 0xe5, 0x85, 0xb1, 0xe5, 0x92, 0x8c, 0xe5, 0x9b, 0xbd};
    String expectedChinese(UTF8_TO_STRING(chinese));
    
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), expectedChinese[0]);
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), expectedChinese[1]);
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), expectedChinese[2]);
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), expectedChinese[3]);
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), expectedChinese[4]);
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), expectedChinese[5]);
    BOOST_CHECK_EQUAL((wchar_t)stream->read(), expectedChinese[6]);
}

BOOST_AUTO_TEST_CASE(testInputStreamReaderReadLine)
{
    BufferedReaderPtr reader = newLucene<BufferedReader>(newLucene<InputStreamReader>(newLucene<FileReader>(FileUtils::joinPath(getTestDir(), L"testfile_text.txt"))));
    
    Collection<String> readLines = Collection<String>::newInstance();
    String line;
    
    while (reader->readLine(line))
        readLines.add(line);
    
    BOOST_CHECK_EQUAL(reader->read(), FileReader::FILE_EOF);
    BOOST_CHECK_EQUAL(readLines.size(), 6);
    BOOST_CHECK_EQUAL(readLines[0], L"test file");
    BOOST_CHECK_EQUAL(readLines[1], L"that contains");
    BOOST_CHECK_EQUAL(readLines[2], L"multiple lines of text");
    BOOST_CHECK_EQUAL(readLines[3], L"");
    BOOST_CHECK_EQUAL(readLines[4], L"");
    BOOST_CHECK_EQUAL(readLines[5], L"1 2 3 4");
}

BOOST_AUTO_TEST_CASE(testInputStreamReaderReadLineUtf8)
{
    BufferedReaderPtr reader = newLucene<BufferedReader>(newLucene<InputStreamReader>(newLucene<FileReader>(FileUtils::joinPath(getTestDir(), L"testfile_uft8.txt"))));
    
    Collection<String> readLines = Collection<String>::newInstance();
    String line;
    
    while (reader->readLine(line))
        readLines.add(line);

    const uint8_t chinese[] = {0xe4, 0xb8, 0xad, 0xe5, 0x8d, 0x8e, 0xe4, 0xba, 0xba, 0xe6, 0xb0, 
                               0x91, 0xe5, 0x85, 0xb1, 0xe5, 0x92, 0x8c, 0xe5, 0x9b, 0xbd};
    
    const uint8_t persian[] = {0xd9, 0x86, 0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 0xb4, 0xd8, 0xaa, 0xd9, 0x87};
    
    const uint8_t russian[] = {0xd0, 0xb0, 0xd0, 0xb1, 0xd0, 0xb8, 0xd1, 0x81, 0xd1, 0x81, 0xd0,
                               0xb8, 0xd0, 0xbd, 0xd0, 0xb8, 0xd1, 0x8e};

    BOOST_CHECK_EQUAL(readLines.size(), 80);
    BOOST_CHECK_EQUAL(readLines[0], UTF8_TO_STRING(chinese));
    BOOST_CHECK_EQUAL(readLines[1], UTF8_TO_STRING(persian));
    BOOST_CHECK_EQUAL(readLines[2], UTF8_TO_STRING(russian));
}

BOOST_AUTO_TEST_SUITE_END()
