/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SpanQueryFilter.h"
#include "SpanQuery.h"
#include "SpanFilterResult.h"
#include "Spans.h"
#include "OpenBitSet.h"
#include "IndexReader.h"

namespace Lucene
{
    SpanQueryFilter::SpanQueryFilter(SpanQueryPtr query)
    {
        this->query = query;
    }
    
    SpanQueryFilter::~SpanQueryFilter()
    {
    }
    
    DocIdSetPtr SpanQueryFilter::getDocIdSet(IndexReaderPtr reader)
    {
        SpanFilterResultPtr result(bitSpans(reader));
        return result->getDocIdSet();
    }
    
    SpanFilterResultPtr SpanQueryFilter::bitSpans(IndexReaderPtr reader)
    {
        OpenBitSetPtr bits(newLucene<OpenBitSet>(reader->maxDoc()));
        SpansPtr spans(query->getSpans(reader));
        Collection<PositionInfoPtr> tmp(Collection<PositionInfoPtr>::newInstance());
        int32_t currentDoc = -1;
        PositionInfoPtr currentInfo;
        while (spans->next())
        {
            int32_t doc = spans->doc();
            bits->set(doc);
            if (currentDoc != doc)
            {
                currentInfo = newLucene<PositionInfo>(doc);
                tmp.add(currentInfo);
                currentDoc = doc;
            }
            currentInfo->addPosition(spans->start(), spans->end());
        }
        return newLucene<SpanFilterResult>(bits, tmp);
    }
    
    SpanQueryPtr SpanQueryFilter::getQuery()
    {
        return query;
    }
    
    String SpanQueryFilter::toString()
    {
        return L"SpanQueryFilter(" + query->toString() + L")";
    }
    
    bool SpanQueryFilter::equals(LuceneObjectPtr other)
    {
        if (LuceneObject::equals(other))
            return true;
        
        SpanQueryFilterPtr otherSpanQueryFilter(boost::dynamic_pointer_cast<SpanQueryFilter>(other));
        if (!otherSpanQueryFilter)
            return false;
        return query->equals(otherSpanQueryFilter->query);
    }
    
    int32_t SpanQueryFilter::hashCode()
    {
        return query->hashCode() ^ 0x923f64b9;
    }
}
