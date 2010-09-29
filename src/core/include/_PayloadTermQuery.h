#pragma once
#include "PayloadTermQuery.h"

namespace Lucene
{

	class PayloadTermWeight : public SpanWeight
	{
	public:
		PayloadTermWeight(PayloadTermQueryPtr query, SearcherPtr searcher);
		virtual ~PayloadTermWeight();
		
		LUCENE_CLASS(PayloadTermWeight);
	
	public:
		virtual ScorerPtr scorer(IndexReaderPtr reader, bool scoreDocsInOrder, bool topScorer);
	};
	

	class PayloadTermSpanScorer : public SpanScorer
	{
	public:
		PayloadTermSpanScorer(TermSpansPtr spans, WeightPtr weight, SimilarityPtr similarity, ByteArray norms);
		virtual ~PayloadTermSpanScorer();
		
		LUCENE_CLASS(PayloadTermSpanScorer);
	
	protected:
		ByteArray payload;
		TermPositionsPtr positions;
		double payloadScore;
		int32_t payloadsSeen;
	
	public:
		virtual double score();
	
	protected:
		virtual bool setFreqCurrentDoc();
		
		void processPayload(SimilarityPtr similarity);
		
		/// Returns the SpanScorer score only.
		///
		/// Should not be overridden without good cause
		///
		/// @return the score for just the Span part without the payload
		/// @see #score()
		virtual double getSpanScore();
		
		/// The score for the payload
		///
		/// @return The score, as calculated by {@link PayloadFunction#docScore(int32_t, const String&, 
		/// int32_t, double)}
		virtual double getPayloadScore();
		
		virtual ExplanationPtr explain(int32_t doc);
	};
}

