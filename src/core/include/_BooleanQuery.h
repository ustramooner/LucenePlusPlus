#pragma once

#include "BooleanQuery.h"
#include "SimilarityDelegator.h"

namespace Lucene
{

	/// Disabled coord Similarity
	class SimilarityDisableCoord : public SimilarityDelegator
	{
	public:
		SimilarityDisableCoord(SimilarityPtr delegee);
		virtual ~SimilarityDisableCoord();
		
		LUCENE_CLASS(SimilarityDisableCoord);
	
	public:
		virtual double coord(int32_t overlap, int32_t maxOverlap);
	};

	/// The Weight for BooleanQuery, used to normalize, score and explain these queries.
	class BooleanWeight : public Weight
	{
	public:
		BooleanWeight(BooleanQueryPtr query, SearcherPtr searcher);
		virtual ~BooleanWeight();
		
		LUCENE_CLASS(BooleanWeight);
	
	protected:
		BooleanQueryPtr query;
		
		/// The Similarity implementation.
		SimilarityPtr similarity;
		Collection<WeightPtr> weights;
	
	public:
		virtual QueryPtr getQuery();
		virtual double getValue();
		virtual double sumOfSquaredWeights();
		virtual void normalize(double norm);
		virtual ExplanationPtr explain(IndexReaderPtr reader, int32_t doc);
		virtual ScorerPtr scorer(IndexReaderPtr reader, bool scoreDocsInOrder, bool topScorer);
		virtual bool scoresDocsOutOfOrder();
	};
}

