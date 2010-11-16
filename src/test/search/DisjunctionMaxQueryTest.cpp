/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "TestInc.h"
#include "LuceneTestFixture.h"
#include "DefaultSimilarity.h"
#include "RAMDirectory.h"
#include "IndexWriter.h"
#include "WhitespaceAnalyzer.h"
#include "Document.h"
#include "Field.h"
#include "IndexReader.h"
#include "IndexSearcher.h"
#include "DisjunctionMaxQuery.h"
#include "QueryUtils.h"
#include "Weight.h"
#include "Scorer.h"
#include "DocIdSetIterator.h"
#include "TermQuery.h"
#include "Term.h"
#include "ScoreDoc.h"
#include "TopDocs.h"
#include "BooleanQuery.h"

using namespace Lucene;

/// Similarity to eliminate tf, idf and lengthNorm effects to isolate test case.
class DisjunctionMaxSimilarity : public DefaultSimilarity
{
public:
    virtual ~DisjunctionMaxSimilarity()
    {
    }

public:
    virtual double tf(double freq)
    {
        if (freq > 0.0)
            return 1.0;
        else
            return 0.0;
    }
    
    virtual double lengthNorm(const String& fieldName, int32_t numTokens)
    {
        return 1.0;
    }
    
    virtual double idf(int32_t docFreq, int32_t numDocs)
    {
        return 1.0;
    }
};

class DisjunctionMaxQueryFixture : public LuceneTestFixture
{
public:
    DisjunctionMaxQueryFixture()
    {
        sim = newLucene<DisjunctionMaxSimilarity>();

        index = newLucene<RAMDirectory>();
        IndexWriterPtr writer = newLucene<IndexWriter>(index, newLucene<WhitespaceAnalyzer>(), true, IndexWriter::MaxFieldLengthLIMITED);
        writer->setSimilarity(sim);

        // hed is the most important field, dek is secondary
        
        // d1 is an "ok" match for: albino elephant
        {
            DocumentPtr d1 = newLucene<Document>();
            d1->add(newLucene<Field>(L"id", L"d1", Field::STORE_YES, Field::INDEX_NOT_ANALYZED));
            d1->add(newLucene<Field>(L"hed", L"elephant", Field::STORE_YES, Field::INDEX_ANALYZED));
            d1->add(newLucene<Field>(L"dek", L"elephant", Field::STORE_YES, Field::INDEX_ANALYZED));
            writer->addDocument(d1);
        }
        
        // d2 is a "good" match for: albino elephant
        {
            DocumentPtr d2 = newLucene<Document>();
            d2->add(newLucene<Field>(L"id", L"d2", Field::STORE_YES, Field::INDEX_NOT_ANALYZED));
            d2->add(newLucene<Field>(L"hed", L"elephant", Field::STORE_YES, Field::INDEX_ANALYZED));
            d2->add(newLucene<Field>(L"dek", L"albino", Field::STORE_YES, Field::INDEX_ANALYZED));
            d2->add(newLucene<Field>(L"dek", L"elephant", Field::STORE_YES, Field::INDEX_ANALYZED));
            writer->addDocument(d2);
        }
        
        // d3 is a "better" match for: albino elephant
        {
            DocumentPtr d3 = newLucene<Document>();
            d3->add(newLucene<Field>(L"id", L"d3", Field::STORE_YES, Field::INDEX_NOT_ANALYZED));
            d3->add(newLucene<Field>(L"hed", L"albino", Field::STORE_YES, Field::INDEX_ANALYZED));
            d3->add(newLucene<Field>(L"hed", L"elephant", Field::STORE_YES, Field::INDEX_ANALYZED));
            writer->addDocument(d3);
        }
        
        // d4 is the "best" match for: albino elephant
        {
            DocumentPtr d4 = newLucene<Document>();
            d4->add(newLucene<Field>(L"id", L"d4", Field::STORE_YES, Field::INDEX_NOT_ANALYZED));
            d4->add(newLucene<Field>(L"hed", L"albino", Field::STORE_YES, Field::INDEX_ANALYZED));
            d4->add(newLucene<Field>(L"hed", L"elephant", Field::STORE_YES, Field::INDEX_ANALYZED));
            d4->add(newLucene<Field>(L"dek", L"albino", Field::STORE_YES, Field::INDEX_ANALYZED));
            writer->addDocument(d4);
        }
        
        writer->close();

        r = IndexReader::open(index, true);
        s = newLucene<IndexSearcher>(r);
        s->setSimilarity(sim);
    }
    
    virtual ~DisjunctionMaxQueryFixture()
    {
    }

public:
    SimilarityPtr sim;
    DirectoryPtr index;
    IndexReaderPtr r;
    IndexSearcherPtr s;
    
    static const double SCORE_COMP_THRESH;

protected:
    QueryPtr tq(const String& f, const String& t)
    {
        return newLucene<TermQuery>(newLucene<Term>(f, t));
    }
    
    QueryPtr tq(const String& f, const String& t, double b)
    {
        QueryPtr q = tq(f, t);
        q->setBoost(b);
        return q;
    }
};

const double DisjunctionMaxQueryFixture::SCORE_COMP_THRESH = 0.00001;

BOOST_FIXTURE_TEST_SUITE(DisjunctionMaxQueryTest, DisjunctionMaxQueryFixture)

BOOST_AUTO_TEST_CASE(testSkipToFirsttimeMiss)
{
    DisjunctionMaxQueryPtr dq = newLucene<DisjunctionMaxQuery>(0.0);
    dq->add(tq(L"id", L"d1"));
    dq->add(tq(L"dek", L"DOES_NOT_EXIST"));

    QueryUtils::check(dq, s);

    WeightPtr dw = dq->weight(s);
    ScorerPtr ds = dw->scorer(r, true, false);
    BOOST_CHECK_EQUAL(ds->advance(3), DocIdSetIterator::NO_MORE_DOCS);
}

BOOST_AUTO_TEST_CASE(testSkipToFirsttimeHit)
{
    DisjunctionMaxQueryPtr dq = newLucene<DisjunctionMaxQuery>(0.0);
    dq->add(tq(L"dek", L"albino"));
    dq->add(tq(L"dek", L"DOES_NOT_EXIST"));

    QueryUtils::check(dq, s);

    WeightPtr dw = dq->weight(s);
    ScorerPtr ds = dw->scorer(r, true, false);
    BOOST_CHECK_NE(ds->advance(3), DocIdSetIterator::NO_MORE_DOCS);
    BOOST_CHECK_EQUAL(L"d4", r->document(ds->docID())->get(L"id"));
}

BOOST_AUTO_TEST_CASE(testSimpleEqualScores1)
{
    DisjunctionMaxQueryPtr q = newLucene<DisjunctionMaxQuery>(0.0);
    q->add(tq(L"hed", L"albino"));
    q->add(tq(L"hed", L"elephant"));

    QueryUtils::check(q, s);

    Collection<ScoreDocPtr> h = s->search(q, FilterPtr(), 1000)->scoreDocs;
    BOOST_CHECK_EQUAL(4, h.size());
    
    double score = h[0]->score;
    for (int32_t i = 1; i < h.size(); ++i)
        BOOST_CHECK_CLOSE_FRACTION(score, h[i]->score, SCORE_COMP_THRESH);
}

BOOST_AUTO_TEST_CASE(testSimpleEqualScores2)
{
    DisjunctionMaxQueryPtr q = newLucene<DisjunctionMaxQuery>(0.0);
    q->add(tq(L"dek", L"albino"));
    q->add(tq(L"dek", L"elephant"));

    QueryUtils::check(q, s);

    Collection<ScoreDocPtr> h = s->search(q, FilterPtr(), 1000)->scoreDocs;
    BOOST_CHECK_EQUAL(3, h.size());
    
    double score = h[0]->score;
    for (int32_t i = 1; i < h.size(); ++i)
        BOOST_CHECK_CLOSE_FRACTION(score, h[i]->score, SCORE_COMP_THRESH);
}

BOOST_AUTO_TEST_CASE(testSimpleEqualScores3)
{
    DisjunctionMaxQueryPtr q = newLucene<DisjunctionMaxQuery>(0.0);
    q->add(tq(L"hed", L"albino"));
    q->add(tq(L"hed", L"elephant"));
    q->add(tq(L"dek", L"albino"));
    q->add(tq(L"dek", L"elephant"));

    QueryUtils::check(q, s);

    Collection<ScoreDocPtr> h = s->search(q, FilterPtr(), 1000)->scoreDocs;
    BOOST_CHECK_EQUAL(4, h.size());
    
    double score = h[0]->score;
    for (int32_t i = 1; i < h.size(); ++i)
        BOOST_CHECK_CLOSE_FRACTION(score, h[i]->score, SCORE_COMP_THRESH);
}

BOOST_AUTO_TEST_CASE(testSimpleTiebreaker)
{
    DisjunctionMaxQueryPtr q = newLucene<DisjunctionMaxQuery>(0.01);
    q->add(tq(L"dek", L"albino"));
    q->add(tq(L"dek", L"elephant"));

    QueryUtils::check(q, s);

    Collection<ScoreDocPtr> h = s->search(q, FilterPtr(), 1000)->scoreDocs;
    BOOST_CHECK_EQUAL(3, h.size());
    BOOST_CHECK_EQUAL(L"d2", s->doc(h[0]->doc)->get(L"id"));
    double score0 = h[0]->score;
    double score1 = h[1]->score;
    double score2 = h[2]->score;
    BOOST_CHECK(score0 > score1);
    BOOST_CHECK_CLOSE_FRACTION(score1, score2, SCORE_COMP_THRESH);
}

BOOST_AUTO_TEST_CASE(testBooleanRequiredEqualScores)
{
    BooleanQueryPtr q = newLucene<BooleanQuery>();
    {
        DisjunctionMaxQueryPtr q1 = newLucene<DisjunctionMaxQuery>(0.0);
        q1->add(tq(L"hed", L"albino"));
        q1->add(tq(L"dek", L"albino"));
        q->add(q1, BooleanClause::MUST);
        QueryUtils::check(q1, s);
    }
    {
        DisjunctionMaxQueryPtr q2 = newLucene<DisjunctionMaxQuery>(0.0);
        q2->add(tq(L"hed", L"elephant"));
        q2->add(tq(L"dek", L"elephant"));
        q->add(q2, BooleanClause::MUST);
        QueryUtils::check(q2, s);
    }
    
    QueryUtils::check(q, s);
    
    Collection<ScoreDocPtr> h = s->search(q, FilterPtr(), 1000)->scoreDocs;
    BOOST_CHECK_EQUAL(3, h.size());
    double score = h[0]->score;
    for (int32_t i = 1; i < h.size(); ++i)
        BOOST_CHECK_CLOSE_FRACTION(score, h[i]->score, SCORE_COMP_THRESH);
}

BOOST_AUTO_TEST_CASE(testBooleanOptionalNoTiebreaker)
{
    BooleanQueryPtr q = newLucene<BooleanQuery>();
    {
        DisjunctionMaxQueryPtr q1 = newLucene<DisjunctionMaxQuery>(0.0);
        q1->add(tq(L"hed", L"albino"));
        q1->add(tq(L"dek", L"albino"));
        q->add(q1, BooleanClause::SHOULD);
    }
    {
        DisjunctionMaxQueryPtr q2 = newLucene<DisjunctionMaxQuery>(0.0);
        q2->add(tq(L"hed", L"elephant"));
        q2->add(tq(L"dek", L"elephant"));
        q->add(q2, BooleanClause::SHOULD);
    }
    
    QueryUtils::check(q, s);
    
    Collection<ScoreDocPtr> h = s->search(q, FilterPtr(), 1000)->scoreDocs;
    BOOST_CHECK_EQUAL(4, h.size());
    double score = h[0]->score;
    for (int32_t i = 1; i < h.size() - 1; ++i)
        BOOST_CHECK_CLOSE_FRACTION(score, h[i]->score, SCORE_COMP_THRESH);
    BOOST_CHECK_EQUAL(L"d1", s->doc(h[h.size() - 1]->doc)->get(L"id"));
    double score1 = h[h.size() - 1]->score;
    BOOST_CHECK(score > score1);
}

BOOST_AUTO_TEST_CASE(testBooleanOptionalWithTiebreaker)
{
    BooleanQueryPtr q = newLucene<BooleanQuery>();
    {
        DisjunctionMaxQueryPtr q1 = newLucene<DisjunctionMaxQuery>(0.01);
        q1->add(tq(L"hed", L"albino"));
        q1->add(tq(L"dek", L"albino"));
        q->add(q1, BooleanClause::SHOULD);
    }
    {
        DisjunctionMaxQueryPtr q2 = newLucene<DisjunctionMaxQuery>(0.01);
        q2->add(tq(L"hed", L"elephant"));
        q2->add(tq(L"dek", L"elephant"));
        q->add(q2, BooleanClause::SHOULD);
    }
    
    QueryUtils::check(q, s);
    
    Collection<ScoreDocPtr> h = s->search(q, FilterPtr(), 1000)->scoreDocs;
    BOOST_CHECK_EQUAL(4, h.size());
    
    double score0 = h[0]->score;
    double score1 = h[1]->score;
    double score2 = h[2]->score;
    double score3 = h[3]->score;

    String doc0 = s->doc(h[0]->doc)->get(L"id");
    String doc1 = s->doc(h[1]->doc)->get(L"id");
    String doc2 = s->doc(h[2]->doc)->get(L"id");
    String doc3 = s->doc(h[3]->doc)->get(L"id");

    BOOST_CHECK(doc0 == L"d2" || doc0 == L"d4");
    BOOST_CHECK(doc1 == L"d2" || doc1 == L"d4");
    BOOST_CHECK_CLOSE_FRACTION(score0, score1, SCORE_COMP_THRESH);
    BOOST_CHECK_EQUAL(L"d3", doc2);
    BOOST_CHECK(score1 > score2);

    BOOST_CHECK_EQUAL(L"d1", doc3);
    BOOST_CHECK(score2 > score3);
}

BOOST_AUTO_TEST_CASE(testBooleanOptionalWithTiebreakerAndBoost)
{
    BooleanQueryPtr q = newLucene<BooleanQuery>();
    {
        DisjunctionMaxQueryPtr q1 = newLucene<DisjunctionMaxQuery>(0.01);
        q1->add(tq(L"hed", L"albino", 1.5));
        q1->add(tq(L"dek", L"albino"));
        q->add(q1, BooleanClause::SHOULD);
    }
    {
        DisjunctionMaxQueryPtr q2 = newLucene<DisjunctionMaxQuery>(0.01);
        q2->add(tq(L"hed", L"elephant", 1.5));
        q2->add(tq(L"dek", L"elephant"));
        q->add(q2, BooleanClause::SHOULD);
    }
    
    QueryUtils::check(q, s);
    
    Collection<ScoreDocPtr> h = s->search(q, FilterPtr(), 1000)->scoreDocs;
    BOOST_CHECK_EQUAL(4, h.size());
    
    double score0 = h[0]->score;
    double score1 = h[1]->score;
    double score2 = h[2]->score;
    double score3 = h[3]->score;

    String doc0 = s->doc(h[0]->doc)->get(L"id");
    String doc1 = s->doc(h[1]->doc)->get(L"id");
    String doc2 = s->doc(h[2]->doc)->get(L"id");
    String doc3 = s->doc(h[3]->doc)->get(L"id");

    BOOST_CHECK(score0 > score1);
    BOOST_CHECK(score1 > score2);
    BOOST_CHECK(score2 > score3);
}

BOOST_AUTO_TEST_SUITE_END()
