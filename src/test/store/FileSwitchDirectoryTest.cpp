/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "TestInc.h"
#include "LuceneTestFixture.h"
#include "FileSwitchDirectory.h"
#include "MockRAMDirectory.h"
#include "IndexWriter.h"
#include "IndexReader.h"
#include "WhitespaceAnalyzer.h"
#include "Document.h"
#include "Field.h"

using namespace Lucene;

BOOST_FIXTURE_TEST_SUITE(FileSwitchDirectoryTest, LuceneTestFixture)

static DocumentPtr createDocument(int32_t n, const String& indexName, int32_t numFields)
{
    StringStream buffer;
    DocumentPtr doc = newLucene<Document>();
    doc->add(newLucene<Field>(L"id", StringUtils::toString(n), Field::STORE_YES, Field::INDEX_NOT_ANALYZED, Field::TERM_VECTOR_WITH_POSITIONS_OFFSETS));
    doc->add(newLucene<Field>(L"indexname", indexName, Field::STORE_YES, Field::INDEX_NOT_ANALYZED, Field::TERM_VECTOR_WITH_POSITIONS_OFFSETS));
    buffer << L"a" << n;
    doc->add(newLucene<Field>(L"field1", buffer.str(), Field::STORE_YES, Field::INDEX_ANALYZED, Field::TERM_VECTOR_WITH_POSITIONS_OFFSETS));
    buffer << L" b" << n;
    for (int32_t i = 1; i < numFields; ++i)
        doc->add(newLucene<Field>(L"field" + StringUtils::toString(i + 1), buffer.str(), Field::STORE_YES, Field::INDEX_ANALYZED, Field::TERM_VECTOR_WITH_POSITIONS_OFFSETS));
    return doc;
}

static void createIndexNoClose(bool multiSegment, const String& indexName, IndexWriterPtr writer)
{
    for (int32_t i = 0; i < 100; ++i)
        writer->addDocument(createDocument(i, indexName, 4));
    if (!multiSegment)
        writer->optimize();
}

// Test if writing doc stores to disk and everything else to ram works.
BOOST_AUTO_TEST_CASE(testBasic)
{
    HashSet<String> fileExtensions(HashSet<String>::newInstance());
    fileExtensions.add(L"fdt");
    fileExtensions.add(L"fdx");

    DirectoryPtr primaryDir(newLucene<MockRAMDirectory>());
    RAMDirectoryPtr secondaryDir(newLucene<MockRAMDirectory>());
    
    FileSwitchDirectoryPtr fsd(newLucene<FileSwitchDirectory>(fileExtensions, primaryDir, secondaryDir, true));
    IndexWriterPtr writer(newLucene<IndexWriter>(fsd, newLucene<WhitespaceAnalyzer>(), IndexWriter::MaxFieldLengthLIMITED));
    writer->setUseCompoundFile(false);
    createIndexNoClose(true, L"ram", writer);
    IndexReaderPtr reader = writer->getReader();
    BOOST_CHECK_EQUAL(reader->maxDoc(), 100);
    writer->commit();
    // we should see only fdx,fdt files here
    HashSet<String> files = primaryDir->listAll();
    BOOST_CHECK(!files.empty());
    for (HashSet<String>::iterator file = files.begin(); file != files.end(); ++file)
    {
        String ext = FileSwitchDirectory::getExtension(*file);
        BOOST_CHECK(fileExtensions.contains(ext));
    }
    files = secondaryDir->listAll();
    BOOST_CHECK(!files.empty());
    // we should not see fdx,fdt files here
    for (HashSet<String>::iterator file = files.begin(); file != files.end(); ++file)
    {
        String ext = FileSwitchDirectory::getExtension(*file);
        BOOST_CHECK(!fileExtensions.contains(ext));
    }
    reader->close();
    writer->close();

    files = fsd->listAll();
    for (HashSet<String>::iterator file = files.begin(); file != files.end(); ++file)
        BOOST_CHECK(!file->empty());
    fsd->close();
}

BOOST_AUTO_TEST_SUITE_END()
