#pragma once
#include "FilteredQuery.h"

namespace Lucene
{

	class FilteredQueryWeight : public Weight
	{
	public:
		FilteredQueryWeight(FilteredQueryPtr query, WeightPtr weight, SimilarityPtr similarity);
		virtual ~FilteredQueryWeight();
		
		LUCENE_CLASS(FilteredQueryWeight);
	
	protected:
		FilteredQueryPtr query;
		WeightPtr weight;
		SimilarityPtr similarity;
		double value;
	
	public:
		virtual double getValue();
		virtual double sumOfSquaredWeights();
		virtual void normalize(double norm);
		virtual ExplanationPtr explain(IndexReaderPtr reader, int32_t doc);
		virtual QueryPtr getQuery();
		virtual ScorerPtr scorer(IndexReaderPtr reader, bool scoreDocsInOrder, bool topScorer);
		
		friend class FilteredQueryWeightScorer;
	};
	
	class FilteredQueryWeightScorer : public Scorer
	{
	public:
		FilteredQueryWeightScorer(FilteredQueryWeightPtr weight, ScorerPtr scorer, DocIdSetIteratorPtr docIdSetIterator, SimilarityPtr similarity);
		virtual ~FilteredQueryWeightScorer();
		
		LUCENE_CLASS(FilteredQueryWeightScorer);
	
	protected:
		FilteredQueryWeightPtr weight;
		ScorerPtr scorer;
		DocIdSetIteratorPtr docIdSetIterator;
		int32_t doc;
	
	public:
		virtual int32_t nextDoc();
		virtual int32_t docID();
		virtual int32_t advance(int32_t target);
		virtual double score();
	
	protected:
		int32_t advanceToCommon(int32_t scorerDoc, int32_t disiDoc);
	};
}

