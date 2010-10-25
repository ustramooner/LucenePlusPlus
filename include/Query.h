/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef QUERY_H
#define QUERY_H

#include "LuceneObject.h"

namespace Lucene
{
	/// The abstract base class for queries.
	///
	/// Instantiable subclasses are:
	///
    /// {@link TermQuery}
    /// {@link MultiTermQuery}
    /// {@link BooleanQuery}
    /// {@link WildcardQuery}
    /// {@link PhraseQuery}
    /// {@link PrefixQuery}
    /// {@link MultiPhraseQuery}
    /// {@link FuzzyQuery}
    /// {@link TermRangeQuery}
    /// {@link NumericRangeQuery}
    /// {@link org.apache.lucene.search.spans.SpanQuery}
    ///
    /// A parser for queries is contained in: {@link QueryParser}
	class LPPAPI Query : public LuceneObject
	{
	protected:
		Query();
	public:
		virtual ~Query();
		
		LUCENE_CLASS(Query);
	
	protected:
		double boost; // query boost factor
			
	public:
		/// Sets the boost for this query clause to b.  Documents matching this clause will (in addition to 
		/// the normal weightings) have their score multiplied by b.
		virtual void setBoost(double b);
		
		/// Gets the boost for this clause.  Documents matching this clause will (in addition to the normal
		/// weightings) have their score multiplied by b.   The boost is 1.0 by default.
		virtual double getBoost();
		
		/// Prints a query to a string, with field assumed to be the default field and omitted.
		///
		/// The representation used is one that is supposed to be readable by {@link QueryParser}. However,
		/// there are the following limitations:
		///
		/// If the query was created by the parser, the printed representation may not be exactly what was 
		/// parsed. For example, characters that need to be escaped will be represented without the required 
		/// backslash.
		///
		/// Some of the more complicated queries (eg. span queries) don't have a representation that can be 
		/// parsed by QueryParser.
		virtual String toString(const String& field);
		
		/// Prints a query to a string.
		virtual String toString();
		
		/// Constructs an appropriate Weight implementation for this query.
		/// Only implemented by primitive queries, which re-write to themselves.
		virtual WeightPtr createWeight(SearcherPtr searcher);
		
		/// Constructs and initializes a Weight for a top-level query.
		virtual WeightPtr weight(SearcherPtr searcher);
		
		/// Called to re-write queries into primitive queries.  For example, a PrefixQuery will be rewritten 
		/// into a BooleanQuery that consists of TermQuerys.
		virtual QueryPtr rewrite(IndexReaderPtr reader);
		
		/// Called when re-writing queries under MultiSearcher.
		///
		/// Create a single query suitable for use by all subsearchers (in 1-1 correspondence with queries). 
		/// This is an optimization of the OR of all queries. We handle the common optimization cases of equal
		/// queries and overlapping clauses of boolean OR queries (as generated by MultiTermQuery.rewrite()).
		/// Be careful overriding this method as queries[0] determines which method will be called and is not 
		/// necessarily of the same type as the other queries.
		virtual QueryPtr combine(Collection<QueryPtr> queries);
		
		/// Adds all terms occurring in this query to the terms set.  Only works if this query is in its 
		/// {@link #rewrite rewritten} form.
		virtual void extractTerms(SetTerm terms);
		
		/// Merges the clauses of a set of BooleanQuery's into a single BooleanQuery.
		///
		/// A utility for use by {@link #combine(Query[])} implementations.
		static QueryPtr mergeBooleanQueries(Collection<BooleanQueryPtr> queries);
		
		/// Returns the Similarity implementation to be used for this query.  Subclasses may override this method 
		/// to specify their own Similarity implementation, perhaps one that delegates through that of the Searcher.
		/// By default the Searcher's Similarity implementation is returned.
		virtual SimilarityPtr getSimilarity(SearcherPtr searcher);
		
		/// Returns a clone of this query.
		virtual LuceneObjectPtr clone(LuceneObjectPtr other = LuceneObjectPtr());
		
		virtual int32_t hashCode();
		virtual bool equals(LuceneObjectPtr other);
		
		/// Return given boost value as a string.
		String boostString();
	};
}

#endif
