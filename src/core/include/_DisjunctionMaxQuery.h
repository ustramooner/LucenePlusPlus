#pragma once
#include "DisjunctionMaxQuery.h"

namespace Lucene
{

	/// The Weight for DisjunctionMaxQuery, used to normalize, score and explain these queries.
	class DisjunctionMaxWeight : public Weight
	{
	public:
		/// Construct the Weight for this Query searched by searcher.  Recursively construct subquery weights.
		DisjunctionMaxWeight(DisjunctionMaxQueryPtr query, SearcherPtr searcher);
		virtual ~DisjunctionMaxWeight();
		
		LUCENE_CLASS(DisjunctionMaxWeight);
	
	protected:
		DisjunctionMaxQueryPtr query;
		
		/// The Similarity implementation.
		SimilarityPtr similarity;
		
		/// The Weights for our subqueries, in 1-1 correspondence with disjuncts
		Collection<WeightPtr> weights;
	
	public:
		/// Return our associated DisjunctionMaxQuery
		virtual QueryPtr getQuery();
		
		/// Return our boost
		virtual double getValue();
		
		/// Compute the sub of squared weights of us applied to our subqueries.  Used for normalization.
		virtual double sumOfSquaredWeights();
		
		/// Apply the computed normalization factor to our subqueries
		virtual void normalize(double norm);
		
		/// Create the scorer used to score our associated DisjunctionMaxQuery
		virtual ScorerPtr scorer(IndexReaderPtr reader, bool scoreDocsInOrder, bool topScorer);
		
		/// Explain the score we computed for doc
		virtual ExplanationPtr explain(IndexReaderPtr reader, int32_t doc);
	};	
}

