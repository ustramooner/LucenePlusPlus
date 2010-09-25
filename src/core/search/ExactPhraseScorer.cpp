/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExactPhraseScorer.h"
#include "PhrasePositions.h"
#include "PhraseQueue.h"

namespace Lucene
{
    ExactPhraseScorer::ExactPhraseScorer(WeightPtr weight, Collection<TermPositionsPtr> tps, Collection<int32_t> offsets, SimilarityPtr similarity, ByteArray norms) : PhraseScorer(weight, tps, offsets, similarity, norms)
    {
    }
    
    ExactPhraseScorer::~ExactPhraseScorer()
    {
    }
    
    double ExactPhraseScorer::phraseFreq()
    {
        // sort list with pq
        pq->clear();
        for (PhrasePositionsPtr pp(first); more && pp; pp = pp->_next)
        {
            pp->firstPosition();
            pq->add(pp); // build pq from list
        }
        pqToList(); // rebuild list from pq
        
        // For counting how many times the exact phrase is found in current document, just count how many 
        // times all PhrasePosition's have exactly the same position.
        int32_t freq = 0;
        do
        {
            while (first->position < last->position) // scan forward in first
            {
                do
                {
                    if (!first->nextPosition())
                        return freq;
                }
                while (first->position < last->position);
                firstToLast();
            }
            ++freq; // all equal: a match
        }
        while (last->nextPosition());
        
        return freq;
    }
}
