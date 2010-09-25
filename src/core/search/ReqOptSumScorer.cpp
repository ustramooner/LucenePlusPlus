/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReqOptSumScorer.h"

namespace Lucene
{
    ReqOptSumScorer::ReqOptSumScorer(ScorerPtr reqScorer, ScorerPtr optScorer) : Scorer(SimilarityPtr()) // No similarity used.
    {
        this->reqScorer = reqScorer;
        this->optScorer = optScorer;
    }
    
    ReqOptSumScorer::~ReqOptSumScorer()
    {
    }
    
    int32_t ReqOptSumScorer::nextDoc()
    {
        return reqScorer->nextDoc();
    }
    
    int32_t ReqOptSumScorer::advance(int32_t target)
    {
        return reqScorer->advance(target);
    }
    
    int32_t ReqOptSumScorer::docID()
    {
        return reqScorer->docID();
    }
    
    double ReqOptSumScorer::score()
    {
        int32_t curDoc = reqScorer->docID();
        double reqScore = reqScorer->score();
        if (!optScorer)
            return reqScore;
        
        int32_t optScorerDoc = optScorer->docID();
        if (optScorerDoc < curDoc && (optScorerDoc = optScorer->advance(curDoc)) == NO_MORE_DOCS)
        {
            optScorer.reset();
            return reqScore;
        }
        
        return optScorerDoc == curDoc ? reqScore + optScorer->score() : reqScore;
    }
}
