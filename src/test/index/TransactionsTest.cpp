/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "TestInc.h"
#include "LuceneTestFixture.h"
#include "TestUtils.h"
#include "IndexWriter.h"
#include "WhitespaceAnalyzer.h"
#include "ConcurrentMergeScheduler.h"
#include "Document.h"
#include "Field.h"
#include "Term.h"
#include "IndexReader.h"
#include "MockRAMDirectory.h"
#include "Random.h"
#include "MiscUtils.h"

using namespace Lucene;

BOOST_FIXTURE_TEST_SUITE(TransactionsTest, LuceneTestFixture)

static bool doFail = false;

DECLARE_SHARED_PTR(TimedThread)
DECLARE_SHARED_PTR(IndexerThread)
DECLARE_SHARED_PTR(SearcherThread)

class TimedThread : public LuceneThread
{
public:
    TimedThread()
    {
    }
    
    virtual ~TimedThread()
    {
    }
    
    LUCENE_CLASS(TimedThread);
    
protected:
    static const int32_t RUN_TIME_SEC;
    
public:
    virtual void doWork() = 0;
    
    virtual void run()
    {
        int64_t stopTime = MiscUtils::currentTimeMillis() + 1000 * RUN_TIME_SEC;
        
        try
        {
            while ((int64_t)MiscUtils::currentTimeMillis() < stopTime)
                doWork();
        }
        catch (LuceneException& e)
        {
            BOOST_FAIL("Unexpected exception: " << e.getError());
        }
    }
};

const int32_t TimedThread::RUN_TIME_SEC = 6;

class IndexerThread : public TimedThread
{
public:
    IndexerThread(SynchronizePtr lock, DirectoryPtr dir1, DirectoryPtr dir2)
    {
        this->lock = lock;
        this->dir1 = dir1;
        this->dir2 = dir2;
        this->nextID = 0;
        this->random = newLucene<Random>();
    }
    
    virtual ~IndexerThread()
    {
    }
    
    LUCENE_CLASS(IndexerThread);
    
public:
    DirectoryPtr dir1;
    DirectoryPtr dir2;
    SynchronizePtr lock;
    int32_t nextID;
    RandomPtr random;
    
public:
    virtual void doWork()
    {
        IndexWriterPtr writer1 = newLucene<IndexWriter>(dir1, newLucene<WhitespaceAnalyzer>(), IndexWriter::MaxFieldLengthLIMITED);
        writer1->setMaxBufferedDocs(3);
        writer1->setMergeFactor(2);
        boost::dynamic_pointer_cast<ConcurrentMergeScheduler>(writer1->getMergeScheduler())->setSuppressExceptions();

        IndexWriterPtr writer2 = newLucene<IndexWriter>(dir2, newLucene<WhitespaceAnalyzer>(), IndexWriter::MaxFieldLengthLIMITED);
        // Intentionally use different params so flush/merge happen at different times
        writer2->setMaxBufferedDocs(2);
        writer2->setMergeFactor(3);
        boost::dynamic_pointer_cast<ConcurrentMergeScheduler>(writer2->getMergeScheduler())->setSuppressExceptions();

        update(writer1);
        update(writer2);
        
        doFail = true;
        bool continueWork = true;
        
        LuceneException finally;
        try
        {
            SyncLock syncLock(lock);
            try
            {
                writer1->prepareCommit();
            }
            catch (...)
            {
                writer1->rollback();
                writer2->rollback();
                continueWork = false;
            }
            try
            {
                writer2->prepareCommit();
            }
            catch (...)
            {
                writer1->rollback();
                writer2->rollback();
                continueWork = false;
            }
            writer1->commit();
            writer2->commit();
        }
        catch (LuceneException& e)
        {
            finally = e;
        }
        doFail = false;
        finally.throwException();
        
        if (!continueWork)
            return;
        
        writer1->close();
        writer2->close();
    }
    
    void update(IndexWriterPtr writer)
    {
        // Add 10 docs
        for (int32_t j = 0; j < 10; ++j)
        {
            DocumentPtr d = newLucene<Document>();
            d->add(newLucene<Field>(L"id", StringUtils::toString(nextID++), Field::STORE_YES, Field::INDEX_NOT_ANALYZED));
            d->add(newLucene<Field>(L"contents", intToEnglish(random->nextInt()), Field::STORE_NO, Field::INDEX_ANALYZED));
            writer->addDocument(d);
        }
        
        // Delete 5 docs
        int32_t deleteID = nextID - 1;
        for (int32_t j = 0; j < 5; ++j)
        {
            writer->deleteDocuments(newLucene<Term>(L"id", StringUtils::toString(deleteID)));
            deleteID -= 2;
        }
    }
};

class SearcherThread : public TimedThread
{
public:
    SearcherThread(SynchronizePtr lock, DirectoryPtr dir1, DirectoryPtr dir2)
    {
        this->lock = lock;
        this->dir1 = dir1;
        this->dir2 = dir2;
    }
    
    virtual ~SearcherThread()
    {
    }
    
    LUCENE_CLASS(SearcherThread);
    
protected:
    DirectoryPtr dir1;
    DirectoryPtr dir2;
    SynchronizePtr lock;
    
public:
    virtual void doWork()
    {
        IndexReaderPtr r1;
        IndexReaderPtr r2;
        {
            SyncLock syncLock(lock);
            r1 = IndexReader::open(dir1, true);
            r2 = IndexReader::open(dir2, true);
        }
        if (r1->numDocs() != r2->numDocs())
            BOOST_FAIL("doc counts differ");
        r1->close();
        r2->close();
    }
};

DECLARE_SHARED_PTR(RandomFailure)

class RandomFailure : public MockDirectoryFailure
{
public:
    RandomFailure()
    {
        random = newLucene<Random>();
    }
    
    virtual ~RandomFailure()
    {
    }

protected:
    RandomPtr random;
    
public:
    virtual void eval(MockRAMDirectoryPtr dir)
    {
        if (doFail && random->nextInt() % 10 <= 3)
            boost::throw_exception(IOException(L"now failing randomly but on purpose"));
    }
};

static void initIndex(DirectoryPtr dir)
{
    IndexWriterPtr writer = newLucene<IndexWriter>(dir, newLucene<WhitespaceAnalyzer>(), IndexWriter::MaxFieldLengthLIMITED);
    RandomPtr random = newLucene<Random>();
    for (int32_t j = 0; j < 7; ++j)
    {
        DocumentPtr d = newLucene<Document>();
        d->add(newLucene<Field>(L"contents", intToEnglish(random->nextInt()), Field::STORE_NO, Field::INDEX_ANALYZED));
        writer->addDocument(d);
    }
    writer->close();
}

BOOST_AUTO_TEST_CASE(testTransactions)
{
    MockRAMDirectoryPtr dir1 = newLucene<MockRAMDirectory>();
    MockRAMDirectoryPtr dir2 = newLucene<MockRAMDirectory>();
    dir1->setPreventDoubleWrite(false);
    dir2->setPreventDoubleWrite(false);
    dir1->failOn(newLucene<RandomFailure>());
    dir2->failOn(newLucene<RandomFailure>());

    initIndex(dir1);
    initIndex(dir2);

    Collection<TimedThreadPtr> threads = Collection<TimedThreadPtr>::newInstance(3);
    int32_t numThread = 0;
    
    SynchronizePtr lock = newInstance<Synchronize>();

    IndexerThreadPtr indexerThread = newLucene<IndexerThread>(lock, dir1, dir2);
    threads[numThread++] = indexerThread;
    indexerThread->start();

    SearcherThreadPtr searcherThread1 = newLucene<SearcherThread>(lock, dir1, dir2);
    threads[numThread++] = searcherThread1;
    searcherThread1->start();

    SearcherThreadPtr searcherThread2 = newLucene<SearcherThread>(lock, dir1, dir2);
    threads[numThread++] = searcherThread2;
    searcherThread2->start();
    
    for (int32_t i = 0; i < numThread; ++i)
        threads[i]->join();
}

BOOST_AUTO_TEST_SUITE_END()
