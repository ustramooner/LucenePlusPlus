/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "TestInc.h"
#include "LuceneTestFixture.h"
#include "IndexWriter.h"
#include "IndexReader.h"
#include "WhitespaceAnalyzer.h"
#include "LogDocMergePolicy.h"
#include "Field.h"
#include "Document.h"
#include "Term.h"
#include "TermDocs.h"
#include "PhraseQuery.h"
#include "MockRAMDirectory.h"
#include "LogByteSizeMergePolicy.h"
#include "SerialMergeScheduler.h"
#include "SegmentInfo.h"

using namespace Lucene;

BOOST_FIXTURE_TEST_SUITE(AddIndexesNoOptimizeTest, LuceneTestFixture)

static IndexWriterPtr newWriter(DirectoryPtr dir, bool create)
{
    IndexWriterPtr writer = newLucene<IndexWriter>(dir, newLucene<WhitespaceAnalyzer>(), create, IndexWriter::MaxFieldLengthUNLIMITED);
    writer->setMergePolicy(newLucene<LogDocMergePolicy>(writer));
    return writer;
}

static void addDocs(IndexWriterPtr writer, int32_t numDocs)
{
    for (int32_t i = 0; i < numDocs; ++i)
    {
        DocumentPtr doc = newLucene<Document>();
        doc->add(newLucene<Field>(L"content", L"aaa", Field::STORE_NO, Field::INDEX_ANALYZED));
        writer->addDocument(doc);
    }
}

static void addDocs2(IndexWriterPtr writer, int32_t numDocs)
{
    for (int32_t i = 0; i < numDocs; ++i)
    {
        DocumentPtr doc = newLucene<Document>();
        doc->add(newLucene<Field>(L"content", L"bbb", Field::STORE_NO, Field::INDEX_ANALYZED));
        writer->addDocument(doc);
    }
}

static void verifyNumDocs(DirectoryPtr dir, int32_t numDocs)
{
    IndexReaderPtr reader = IndexReader::open(dir, true);
    BOOST_CHECK_EQUAL(reader->maxDoc(), numDocs);
    BOOST_CHECK_EQUAL(reader->numDocs(), numDocs);
    reader->close();
}

static void verifyTermDocs(DirectoryPtr dir, TermPtr term, int32_t numDocs)
{
    IndexReaderPtr reader = IndexReader::open(dir, true);
    TermDocsPtr termDocs = reader->termDocs(term);
    int32_t count = 0;
    while (termDocs->next())
        ++count;
    BOOST_CHECK_EQUAL(count, numDocs);
    reader->close();
}

static void setUpDirs(DirectoryPtr dir, DirectoryPtr aux)
{
    IndexWriterPtr writer;

    writer = newWriter(dir, true);
    writer->setMaxBufferedDocs(1000);
    // add 1000 documents in 1 segment
    addDocs(writer, 1000);
    BOOST_CHECK_EQUAL(1000, writer->maxDoc());
    BOOST_CHECK_EQUAL(1, writer->getSegmentCount());
    writer->close();

    writer = newWriter(aux, true);
    writer->setUseCompoundFile(false); // use one without a compound file
    writer->setMaxBufferedDocs(100);
    writer->setMergeFactor(10);
    // add 30 documents in 3 segments
    for (int32_t i = 0; i < 3; ++i)
    {
        addDocs(writer, 10);
        writer->close();
        writer = newWriter(aux, false);
        writer->setUseCompoundFile(false); // use one without a compound file
        writer->setMaxBufferedDocs(100);
        writer->setMergeFactor(10);
    }
    BOOST_CHECK_EQUAL(30, writer->maxDoc());
    BOOST_CHECK_EQUAL(3, writer->getSegmentCount());
    writer->close();
}

BOOST_AUTO_TEST_CASE(testSimpleCase)
{
    // main directory
    DirectoryPtr dir = newLucene<RAMDirectory>();
    // two auxiliary directories
    DirectoryPtr aux = newLucene<RAMDirectory>();
    DirectoryPtr aux2 = newLucene<RAMDirectory>();

    IndexWriterPtr writer = newWriter(dir, true);
    // add 100 documents
    addDocs(writer, 100);
    BOOST_CHECK_EQUAL(writer->maxDoc(), 100);
    writer->close();
    
    writer = newWriter(aux, true);
    writer->setUseCompoundFile(false); // use one without a compound file
    // add 40 documents in separate files
    addDocs(writer, 40);
    BOOST_CHECK_EQUAL(writer->maxDoc(), 40);
    writer->close();
    
    writer = newWriter(aux2, true);
    // add 40 documents in compound files
    addDocs2(writer, 50);
    BOOST_CHECK_EQUAL(writer->maxDoc(), 50);
    writer->close();

    // test doc count before segments are merged
    writer = newWriter(dir, false);
    BOOST_CHECK_EQUAL(writer->maxDoc(), 100);
    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux, aux2));
    BOOST_CHECK_EQUAL(writer->maxDoc(), 190);
    writer->close();
    
    // make sure the old index is correct
    verifyNumDocs(aux, 40);

    // make sure the new index is correct
    verifyNumDocs(dir, 190);

    // now add another set in.
    DirectoryPtr aux3 = newLucene<RAMDirectory>();
    writer = newWriter(aux3, true);
    // add 40 documents
    addDocs(writer, 40);
    BOOST_CHECK_EQUAL(writer->maxDoc(), 40);
    writer->close();
    
    // test doc count before segments are merged/index is optimized
    writer = newWriter(dir, false);
    BOOST_CHECK_EQUAL(writer->maxDoc(), 190);
    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux3));
    BOOST_CHECK_EQUAL(writer->maxDoc(), 230);
    writer->close();

    // make sure the new index is correct
    verifyNumDocs(dir, 230);

    verifyTermDocs(dir, newLucene<Term>(L"content", L"aaa"), 180);

    verifyTermDocs(dir, newLucene<Term>(L"content", L"bbb"), 50);

    // now optimize it.
    writer = newWriter(dir, false);
    writer->optimize();
    writer->close();

    // make sure the new index is correct
    verifyNumDocs(dir, 230);

    verifyTermDocs(dir, newLucene<Term>(L"content", L"aaa"), 180);

    verifyTermDocs(dir, newLucene<Term>(L"content", L"bbb"), 50);

    // now add a single document
    DirectoryPtr aux4 = newLucene<RAMDirectory>();
    writer = newWriter(aux4, true);
    addDocs2(writer, 1);
    writer->close();

    writer = newWriter(dir, false);
    BOOST_CHECK_EQUAL(writer->maxDoc(), 230);
    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux4));
    BOOST_CHECK_EQUAL(writer->maxDoc(), 231);
    writer->close();

    verifyNumDocs(dir, 231);

    verifyTermDocs(dir, newLucene<Term>(L"content", L"bbb"), 51);
}

BOOST_AUTO_TEST_CASE(testWithPendingDeletes)
{
    // main directory
    DirectoryPtr dir = newLucene<RAMDirectory>();
    // auxiliary directory
    DirectoryPtr aux = newLucene<RAMDirectory>();

    setUpDirs(dir, aux);
    IndexWriterPtr writer = newWriter(dir, false);
    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux));
    
    // Adds 10 docs, then replaces them with another 10 docs, so 10 pending deletes
    for (int32_t i = 0; i < 20; ++i)
    {
        DocumentPtr doc = newLucene<Document>();
        doc->add(newLucene<Field>(L"id", StringUtils::toString(i % 10), Field::STORE_NO, Field::INDEX_NOT_ANALYZED));
        doc->add(newLucene<Field>(L"content", L"bbb " + StringUtils::toString(i), Field::STORE_NO, Field::INDEX_ANALYZED));
        writer->updateDocument(newLucene<Term>(L"id", StringUtils::toString(i % 10)), doc);
    }

    // Deletes one of the 10 added docs, leaving 9
    PhraseQueryPtr q = newLucene<PhraseQuery>();
    q->add(newLucene<Term>(L"content", L"bbb"));
    q->add(newLucene<Term>(L"content", L"14"));
    writer->deleteDocuments(q);

    writer->optimize();
    writer->commit();

    verifyNumDocs(dir, 1039);
    verifyTermDocs(dir, newLucene<Term>(L"content", L"aaa"), 1030);
    verifyTermDocs(dir, newLucene<Term>(L"content", L"bbb"), 9);

    writer->close();
    dir->close();
    aux->close();
}

BOOST_AUTO_TEST_CASE(testWithPendingDeletes2)
{
    // main directory
    DirectoryPtr dir = newLucene<RAMDirectory>();
    // auxiliary directory
    DirectoryPtr aux = newLucene<RAMDirectory>();
    
    setUpDirs(dir, aux);
    IndexWriterPtr writer = newWriter(dir, false);
    
    // Adds 10 docs, then replaces them with another 10 docs, so 10 pending deletes
    for (int32_t i = 0; i < 20; ++i)
    {
        DocumentPtr doc = newLucene<Document>();
        doc->add(newLucene<Field>(L"id", StringUtils::toString(i % 10), Field::STORE_NO, Field::INDEX_NOT_ANALYZED));
        doc->add(newLucene<Field>(L"content", L"bbb " + StringUtils::toString(i), Field::STORE_NO, Field::INDEX_ANALYZED));
        writer->updateDocument(newLucene<Term>(L"id", StringUtils::toString(i % 10)), doc);
    }
    
    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux));

    // Deletes one of the 10 added docs, leaving 9
    PhraseQueryPtr q = newLucene<PhraseQuery>();
    q->add(newLucene<Term>(L"content", L"bbb"));
    q->add(newLucene<Term>(L"content", L"14"));
    writer->deleteDocuments(q);

    writer->optimize();
    writer->commit();

    verifyNumDocs(dir, 1039);
    verifyTermDocs(dir, newLucene<Term>(L"content", L"aaa"), 1030);
    verifyTermDocs(dir, newLucene<Term>(L"content", L"bbb"), 9);

    writer->close();
    dir->close();
    aux->close();
}

BOOST_AUTO_TEST_CASE(testWithPendingDeletes3)
{
    // main directory
    DirectoryPtr dir = newLucene<RAMDirectory>();
    // auxiliary directory
    DirectoryPtr aux = newLucene<RAMDirectory>();
    
    setUpDirs(dir, aux);
    IndexWriterPtr writer = newWriter(dir, false);
    
    // Adds 10 docs, then replaces them with another 10 docs, so 10 pending deletes
    for (int32_t i = 0; i < 20; ++i)
    {
        DocumentPtr doc = newLucene<Document>();
        doc->add(newLucene<Field>(L"id", StringUtils::toString(i % 10), Field::STORE_NO, Field::INDEX_NOT_ANALYZED));
        doc->add(newLucene<Field>(L"content", L"bbb " + StringUtils::toString(i), Field::STORE_NO, Field::INDEX_ANALYZED));
        writer->updateDocument(newLucene<Term>(L"id", StringUtils::toString(i % 10)), doc);
    }

    // Deletes one of the 10 added docs, leaving 9
    PhraseQueryPtr q = newLucene<PhraseQuery>();
    q->add(newLucene<Term>(L"content", L"bbb"));
    q->add(newLucene<Term>(L"content", L"14"));
    writer->deleteDocuments(q);
    
    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux));

    writer->optimize();
    writer->commit();

    verifyNumDocs(dir, 1039);
    verifyTermDocs(dir, newLucene<Term>(L"content", L"aaa"), 1030);
    verifyTermDocs(dir, newLucene<Term>(L"content", L"bbb"), 9);

    writer->close();
    dir->close();
    aux->close();
}

BOOST_AUTO_TEST_CASE(testAddSelf)
{
    // main directory
    DirectoryPtr dir = newLucene<RAMDirectory>();
    // auxiliary directory
    DirectoryPtr aux = newLucene<RAMDirectory>();
    
    IndexWriterPtr writer = newWriter(dir, true);
    // add 100 documents
    addDocs(writer, 100);
    BOOST_CHECK_EQUAL(100, writer->maxDoc());
    writer->close();

    writer = newWriter(aux, true);
    writer->setUseCompoundFile(false); // use one without a compound file
    writer->setMaxBufferedDocs(1000);
    // add 140 documents in separate files
    addDocs(writer, 40);
    writer->close();
    writer = newWriter(aux, true);
    writer->setUseCompoundFile(false); // use one without a compound file
    writer->setMaxBufferedDocs(1000);
    addDocs(writer, 100);
    writer->close();
    
    writer = newWriter(dir, false);
    BOOST_CHECK_EXCEPTION(writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux, dir)), LuceneException, check_exception(LuceneException::IllegalArgument)); // cannot add self
    BOOST_CHECK_EQUAL(100, writer->maxDoc());

    writer->close();
    
    // make sure the index is correct
    verifyNumDocs(dir, 100);

    dir->close();
    aux->close();
}

BOOST_AUTO_TEST_CASE(testNoTailSegments)
{
    // main directory
    DirectoryPtr dir = newLucene<RAMDirectory>();
    // auxiliary directory
    DirectoryPtr aux = newLucene<RAMDirectory>();

    setUpDirs(dir, aux);

    IndexWriterPtr writer = newWriter(dir, false);
    writer->setMaxBufferedDocs(10);
    writer->setMergeFactor(4);
    addDocs(writer, 10);

    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux));
    
    BOOST_CHECK_EQUAL(1040, writer->maxDoc());
    BOOST_CHECK_EQUAL(2, writer->getSegmentCount());
    BOOST_CHECK_EQUAL(1000, writer->getDocCount(0));
    writer->close();

    // make sure the index is correct
    verifyNumDocs(dir, 1040);

    dir->close();
    aux->close();
}

BOOST_AUTO_TEST_CASE(testNoCopySegments)
{
    // main directory
    DirectoryPtr dir = newLucene<RAMDirectory>();
    // auxiliary directory
    DirectoryPtr aux = newLucene<RAMDirectory>();

    setUpDirs(dir, aux);

    IndexWriterPtr writer = newWriter(dir, false);
    writer->setMaxBufferedDocs(9);
    writer->setMergeFactor(4);
    addDocs(writer, 2);

    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux));
    
    BOOST_CHECK_EQUAL(1032, writer->maxDoc());
    BOOST_CHECK_EQUAL(2, writer->getSegmentCount());
    BOOST_CHECK_EQUAL(1000, writer->getDocCount(0));
    writer->close();

    // make sure the index is correct
    verifyNumDocs(dir, 1032);

    dir->close();
    aux->close();
}

BOOST_AUTO_TEST_CASE(testNoMergeAfterCopy)
{
    // main directory
    DirectoryPtr dir = newLucene<RAMDirectory>();
    // auxiliary directory
    DirectoryPtr aux = newLucene<RAMDirectory>();

    setUpDirs(dir, aux);

    IndexWriterPtr writer = newWriter(dir, false);
    writer->setMaxBufferedDocs(10);
    writer->setMergeFactor(4);

    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux, newLucene<RAMDirectory>(aux)));
    
    BOOST_CHECK_EQUAL(1060, writer->maxDoc());
    BOOST_CHECK_EQUAL(1000, writer->getDocCount(0));
    writer->close();

    // make sure the index is correct
    verifyNumDocs(dir, 1060);

    dir->close();
    aux->close();
}

BOOST_AUTO_TEST_CASE(testMergeAfterCopy)
{
    // main directory
    DirectoryPtr dir = newLucene<RAMDirectory>();
    // auxiliary directory
    DirectoryPtr aux = newLucene<RAMDirectory>();

    setUpDirs(dir, aux);

    IndexReaderPtr reader = IndexReader::open(aux, false);
    for (int32_t i = 0; i < 20; ++i)
        reader->deleteDocument(i);
    BOOST_CHECK_EQUAL(10, reader->numDocs());
    reader->close();

    IndexWriterPtr writer = newWriter(dir, false);
    writer->setMaxBufferedDocs(4);
    writer->setMergeFactor(4);

    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux, newLucene<RAMDirectory>(aux)));
    
    BOOST_CHECK_EQUAL(1020, writer->maxDoc());
    BOOST_CHECK_EQUAL(1000, writer->getDocCount(0));
    writer->close();

    // make sure the index is correct
    verifyNumDocs(dir, 1020);

    dir->close();
    aux->close();
}

BOOST_AUTO_TEST_CASE(testMoreMerges)
{
    // main directory
    DirectoryPtr dir = newLucene<RAMDirectory>();
    // auxiliary directory
    DirectoryPtr aux = newLucene<RAMDirectory>();
    DirectoryPtr aux2 = newLucene<RAMDirectory>();

    setUpDirs(dir, aux);

    IndexWriterPtr writer = newWriter(aux2, true);
    writer->setMaxBufferedDocs(100);
    writer->setMergeFactor(10);

    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux));
    
    BOOST_CHECK_EQUAL(30, writer->maxDoc());
    BOOST_CHECK_EQUAL(3, writer->getSegmentCount());
    writer->close();
    
    IndexReaderPtr reader = IndexReader::open(aux, false);
    for (int32_t i = 0; i < 27; ++i)
        reader->deleteDocument(i);
    BOOST_CHECK_EQUAL(3, reader->numDocs());
    reader->close();
    
    reader = IndexReader::open(aux2, false);
    for (int32_t i = 0; i < 8; ++i)
        reader->deleteDocument(i);
    BOOST_CHECK_EQUAL(22, reader->numDocs());
    reader->close();
    
    writer = newWriter(dir, false);
    writer->setMaxBufferedDocs(6);
    writer->setMergeFactor(4);
    
    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(aux, aux2));

    BOOST_CHECK_EQUAL(1025, writer->maxDoc());
    BOOST_CHECK_EQUAL(1000, writer->getDocCount(0));
    writer->close();
    
    // make sure the index is correct
    verifyNumDocs(dir, 1025);

    dir->close();
    aux->close();
}

BOOST_AUTO_TEST_CASE(testHangOnClose)
{
    DirectoryPtr dir = newLucene<MockRAMDirectory>();
    IndexWriterPtr writer = newLucene<IndexWriter>(dir, newLucene<WhitespaceAnalyzer>(), true, IndexWriter::MaxFieldLengthLIMITED);
    writer->setMergePolicy(newLucene<LogByteSizeMergePolicy>(writer));
    writer->setMaxBufferedDocs(5);
    writer->setUseCompoundFile(false);
    writer->setMergeFactor(100);

    DocumentPtr doc = newLucene<Document>();
    doc->add(newLucene<Field>(L"content", L"aaa bbb ccc ddd eee fff ggg hhh iii", Field::STORE_YES, Field::INDEX_ANALYZED, Field::TERM_VECTOR_WITH_POSITIONS_OFFSETS));
    
    for (int32_t i = 0; i < 60; ++i)
        writer->addDocument(doc);
    writer->setMaxBufferedDocs(200);
    DocumentPtr doc2 = newLucene<Document>();
    
    doc2->add(newLucene<Field>(L"content", L"aaa bbb ccc ddd eee fff ggg hhh iii", Field::STORE_YES, Field::INDEX_NO));
    doc2->add(newLucene<Field>(L"content", L"aaa bbb ccc ddd eee fff ggg hhh iii", Field::STORE_YES, Field::INDEX_NO));
    doc2->add(newLucene<Field>(L"content", L"aaa bbb ccc ddd eee fff ggg hhh iii", Field::STORE_YES, Field::INDEX_NO));
    doc2->add(newLucene<Field>(L"content", L"aaa bbb ccc ddd eee fff ggg hhh iii", Field::STORE_YES, Field::INDEX_NO));
    
    for (int32_t i = 0; i < 60; ++i)
        writer->addDocument(doc2);
    writer->close();
    
    DirectoryPtr dir2 = newLucene<MockRAMDirectory>();
    writer = newLucene<IndexWriter>(dir2, newLucene<WhitespaceAnalyzer>(), true, IndexWriter::MaxFieldLengthLIMITED);
    LogByteSizeMergePolicyPtr lmp = newLucene<LogByteSizeMergePolicy>(writer);
    lmp->setMinMergeMB(0.0001);
    writer->setMergePolicy(lmp);
    writer->setMergeFactor(4);
    writer->setUseCompoundFile(false);
    writer->setMergeScheduler(newLucene<SerialMergeScheduler>());
    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(dir));
    writer->close();
    dir->close();
    dir2->close();
}

BOOST_AUTO_TEST_CASE(testTargetCFS)
{
    // make sure CFS of destination indexwriter is respected when copying tail segments
    DirectoryPtr dir = newLucene<RAMDirectory>();
    IndexWriterPtr writer = newWriter(dir, true);
    writer->setUseCompoundFile(false);
    addDocs(writer, 1);
    writer->close();

    DirectoryPtr other = newLucene<RAMDirectory>();
    writer = newWriter(other, true);
    writer->setUseCompoundFile(true);
    writer->addIndexesNoOptimize(newCollection<DirectoryPtr>(dir));
    BOOST_CHECK(writer->newestSegment()->getUseCompoundFile());
    writer->close();
}

BOOST_AUTO_TEST_SUITE_END()
