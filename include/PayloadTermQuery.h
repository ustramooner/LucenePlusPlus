/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SpanTermQuery.h"
#include "SpanWeight.h"
#include "SpanScorer.h"

namespace Lucene
{
	/// This class is very similar to {@link SpanTermQuery} except that it factors in the value of the payload 
	/// located at each of the positions where the {@link Term} occurs.
	///
	/// In order to take advantage of this, you must override {@link Similarity#scorePayload(int32_t, const String&, 
	/// int32_t, int32_t, ByteArray, int32_t, int32_t)} which returns 1 by default.
	///
	/// Payload scores are aggregated using a pluggable {@link PayloadFunction}.
	class LPPAPI PayloadTermQuery : public SpanTermQuery
	{
	public:
		PayloadTermQuery(TermPtr term, PayloadFunctionPtr function, bool includeSpanScore = true);
		virtual ~PayloadTermQuery();
		
		LUCENE_CLASS(PayloadTermQuery);
	
	protected:
		PayloadFunctionPtr function;
		bool includeSpanScore;
	
	public:
		virtual WeightPtr createWeight(SearcherPtr searcher);
		
		virtual LuceneObjectPtr clone(LuceneObjectPtr other = LuceneObjectPtr());
		virtual bool equals(LuceneObjectPtr other);
		virtual int32_t hashCode();
		
		friend class PayloadTermWeight;
		friend class PayloadTermSpanScorer;
	};
		
}
