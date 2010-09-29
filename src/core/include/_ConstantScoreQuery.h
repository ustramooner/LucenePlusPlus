#pragma once
#include "ConstantScoreQuery.h"

namespace Lucene
{

	class ConstantWeight : public Weight
	{
	public:
		ConstantWeight(ConstantScoreQueryPtr constantScorer, SearcherPtr searcher);
		virtual ~ConstantWeight();
	
		LUCENE_CLASS(ConstantWeight);
	
	protected:
		ConstantScoreQueryPtr constantScorer;
		SimilarityPtr similarity;
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
	
	class ConstantScorer : public Scorer
	{
	public:
		ConstantScorer(ConstantScoreQueryPtr constantScorer, SimilarityPtr similarity, IndexReaderPtr reader, WeightPtr w);
		virtual ~ConstantScorer();
	
		LUCENE_CLASS(ConstantScorer);
	
	public:
		DocIdSetIteratorPtr docIdSetIterator;
		double theScore;
		int32_t doc;
	
	public:
		virtual int32_t nextDoc();
		virtual int32_t docID();
		virtual double score();
		virtual int32_t advance(int32_t target);
	};
}
