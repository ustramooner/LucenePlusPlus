/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ConstantScoreQuery.h"
#include "_ConstantScoreQuery.h"
#include "Filter.h"
#include "ComplexExplanation.h"
#include "DocIdSet.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene
{
    ConstantScoreQuery::ConstantScoreQuery(FilterPtr filter)
    {
        this->filter = filter;
    }
    
    ConstantScoreQuery::~ConstantScoreQuery()
    {
    }
    
    FilterPtr ConstantScoreQuery::getFilter()
    {
        return filter;
    }
    
    QueryPtr ConstantScoreQuery::rewrite(IndexReaderPtr reader)
    {
        return shared_from_this();
    }
    
    void ConstantScoreQuery::extractTerms(SetTerm terms)
    {
        // OK to not add any terms when used for MultiSearcher, but may not be OK for highlighting
    }
    
    WeightPtr ConstantScoreQuery::createWeight(SearcherPtr searcher)
    {
        return newLucene<ConstantWeight>(shared_from_this(), searcher);
    }
    
    String ConstantScoreQuery::toString(const String& field)
    {
        return L"ConstantScore(" + filter->toString() + (getBoost() == 1.0 ? L")" : L"^" + StringUtils::toString(getBoost()));
    }
    
    bool ConstantScoreQuery::equals(LuceneObjectPtr other)
    {
        if (LuceneObject::equals(other))
            return true;
        
        ConstantScoreQueryPtr otherConstantScoreQuery(boost::dynamic_pointer_cast<ConstantScoreQuery>(other));
        if (!otherConstantScoreQuery)
            return false;
        
        return (this->getBoost() == otherConstantScoreQuery->getBoost() && this->filter->equals(otherConstantScoreQuery->filter));
    }
    
    int32_t ConstantScoreQuery::hashCode()
    {
        // Simple add is OK since no existing filter hashcode has a float component.
        return filter->hashCode() + MiscUtils::doubleToIntBits(getBoost());
    }
    
    LuceneObjectPtr ConstantScoreQuery::clone(LuceneObjectPtr other)
    {
        LuceneObjectPtr clone = other ? other : newLucene<ConstantScoreQuery>(filter);
        ConstantScoreQueryPtr cloneQuery(boost::dynamic_pointer_cast<ConstantScoreQuery>(Query::clone(clone)));
        cloneQuery->filter = filter;
        return cloneQuery;
    }
    
    ConstantWeight::ConstantWeight(ConstantScoreQueryPtr constantScorer, SearcherPtr searcher)
    {
        this->constantScorer = constantScorer;
        this->similarity = constantScorer->getSimilarity(searcher);
        this->queryNorm = 0;
        this->queryWeight = 0;
    }
    
    ConstantWeight::~ConstantWeight()
    {
    }
    
    QueryPtr ConstantWeight::getQuery()
    {
        return constantScorer;
    }
    
    double ConstantWeight::getValue()
    {
        return queryWeight;
    }
    
    double ConstantWeight::sumOfSquaredWeights()
    {
        queryWeight = constantScorer->getBoost();
        return queryWeight * queryWeight;
    }
    
    void ConstantWeight::normalize(double norm)
    {
        this->queryNorm = norm;
        queryWeight *= this->queryNorm;
    }
    
    ScorerPtr ConstantWeight::scorer(IndexReaderPtr reader, bool scoreDocsInOrder, bool topScorer)
    {
        return newLucene<ConstantScorer>(constantScorer, similarity, reader, shared_from_this());
    }
    
    ExplanationPtr ConstantWeight::explain(IndexReaderPtr reader, int32_t doc)
    {
        ConstantScorerPtr cs(newLucene<ConstantScorer>(constantScorer, similarity, reader, shared_from_this()));
        bool exists = (cs->docIdSetIterator->advance(doc) == doc);
        
        ComplexExplanationPtr result(newLucene<ComplexExplanation>());
        
        if (exists)
        {
            result->setDescription(L"ConstantScoreQuery(" + constantScorer->filter->toString() + L"), product of:");
            result->setValue(queryWeight);
            result->setMatch(true);
            result->addDetail(newLucene<Explanation>(constantScorer->getBoost(), L"boost"));
            result->addDetail(newLucene<Explanation>(queryNorm, L"queryNorm"));
        }
        else
        {
            result->setDescription(L"ConstantScoreQuery(" + constantScorer->filter->toString() + L") doesn't match id " + StringUtils::toString(doc));
            result->setValue(0);
            result->setMatch(false);
        }
        return result;
    }
    
    ConstantScorer::ConstantScorer(ConstantScoreQueryPtr constantScorer, SimilarityPtr similarity, IndexReaderPtr reader, WeightPtr w) : Scorer(similarity)
    {
        doc = -1;
        theScore = w->getValue();
        DocIdSetPtr docIdSet(constantScorer->filter->getDocIdSet(reader));
        if (!docIdSet)
            docIdSetIterator = DocIdSet::EMPTY_DOCIDSET()->iterator();
        else
        {
            DocIdSetIteratorPtr iter(docIdSet->iterator());
            if (!iter)
                docIdSetIterator = DocIdSet::EMPTY_DOCIDSET()->iterator();
            else
                docIdSetIterator = iter;
        }
    }
    
    ConstantScorer::~ConstantScorer()
    {
    }
    
    int32_t ConstantScorer::nextDoc()
    {
        return docIdSetIterator->nextDoc();
    }
    
    int32_t ConstantScorer::docID()
    {
        return docIdSetIterator->docID();
    }
    
    double ConstantScorer::score()
    {
        return theScore;
    }
    
    int32_t ConstantScorer::advance(int32_t target)
    {
        return docIdSetIterator->advance(target);
    }
}
