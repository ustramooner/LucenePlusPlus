#pragma once
#include "TermQuery.h"

namespace Lucene
{

	class TermWeight : public Weight
	{
	public:
		TermWeight(TermQueryPtr query, SearcherPtr searcher);
		virtual ~TermWeight();
	
		LUCENE_CLASS(TermWeight);
	
	protected:
		TermQueryPtr query;
		SimilarityPtr similarity;
		double value;
		double idf;
		double queryNorm;
		double queryWeight;
		IDFExplanationPtr idfExp;
	
	public:
		virtual String toString();
		virtual QueryPtr getQuery();
		virtual double getValue();
		virtual double sumOfSquaredWeights();
		virtual void normalize(double norm);
		virtual ScorerPtr scorer(IndexReaderPtr reader, bool scoreDocsInOrder, bool topScorer);
		virtual ExplanationPtr explain(IndexReaderPtr reader, int32_t doc);
	};
}

