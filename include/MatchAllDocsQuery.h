/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Query.h"
#include "Weight.h"
#include "Scorer.h"

namespace Lucene
{
	/// A query that matches all documents.
	class LPPAPI MatchAllDocsQuery : public Query
	{
	public:
		/// @param normsField Field used for normalization factor (document boost). Null if nothing.
		MatchAllDocsQuery(const String& normsField = EmptyString);
		
		virtual ~MatchAllDocsQuery();
	
		LUCENE_CLASS(MatchAllDocsQuery);
	
	protected:
		String normsField;
	
	public:
		using Query::toString;
	
		virtual WeightPtr createWeight(SearcherPtr searcher);
		virtual void extractTerms(SetTerm terms);
		virtual String toString(const String& field);
		virtual bool equals(LuceneObjectPtr other);
		virtual int32_t hashCode();
		virtual LuceneObjectPtr clone(LuceneObjectPtr other = LuceneObjectPtr());
		
		friend class MatchAllDocsWeight;
	};
	
}
