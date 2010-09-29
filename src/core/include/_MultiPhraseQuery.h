#pragma once
#include "MultiPhraseQuery.h"

namespace Lucene
{

	class MultiPhraseWeight : public Weight
	{
	public:
		MultiPhraseWeight(MultiPhraseQueryPtr query, SearcherPtr searcher);
		virtual ~MultiPhraseWeight();
	
		LUCENE_CLASS(MultiPhraseWeight);
	
	protected:
		MultiPhraseQueryPtr query;
		SimilarityPtr similarity;
		double value;
		double idf;
		double queryNorm;
		double queryWeight;
	
	public:
		virtual QueryPtr getQuery();
		virtual double getValue();
		virtual double sumOfSquaredWeights();
		virtual void normalize(double norm);
		virtual ScorerPtr scorer(IndexReaderPtr reader, bool scoreDocsInOrder, bool topScorer);
		virtual ExplanationPtr explain(IndexReaderPtr reader, int32_t doc);
	};
}

