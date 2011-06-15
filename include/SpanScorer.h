/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SPANSCORER_H
#define SPANSCORER_H

#include "Scorer.h"

namespace Lucene
{
    /// Public for extension only.
    class LPPAPI SpanScorer : public Scorer
    {
    public:
        SpanScorer(SpansPtr spans, WeightPtr weight, SimilarityPtr similarity, ByteArray norms);
        virtual ~SpanScorer();
        
        LUCENE_CLASS(SpanScorer);
    
    protected:
        SpansPtr spans;
        WeightPtr weight;
        ByteArray norms;
        double value;
        bool more;
        int32_t doc;
        double freq;
    
    public:
        virtual int32_t nextDoc();
        virtual int32_t advance(int32_t target);
        virtual int32_t docID();
        virtual double score();
        
    protected:
        virtual bool setFreqCurrentDoc();
        
        /// This method is no longer an official member of {@link Scorer}, but it is needed by SpanWeight 
        /// to build an explanation.
        virtual ExplanationPtr explain(int32_t doc);
        
        friend class SpanWeight;
        friend class PayloadNearSpanWeight;
    };
}

#endif
