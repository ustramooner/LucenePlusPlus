#pragma once
#include "Similarity.h"

namespace Lucene
{

	class SimilarityIDFExplanation : public IDFExplanation
	{
	public:
		SimilarityIDFExplanation(int32_t df, int32_t max, double idf);
		SimilarityIDFExplanation(const String& exp, double idf);
		
		virtual ~SimilarityIDFExplanation();
		
		LUCENE_CLASS(SimilarityIDFExplanation);
	
	protected:
		String exp;
		int32_t df;
		int32_t max;
		double idf;
	
	public:
		virtual String explain();
		virtual double getIdf();
	};
}

