#pragma once
#include "StopAnalyzer.h"

namespace Lucene
{
	/// Filters LowerCaseTokenizer with StopFilter.
	class StopAnalyzerSavedStreams : public LuceneObject
	{
	public:
		virtual ~StopAnalyzerSavedStreams();
	
		LUCENE_CLASS(StopAnalyzerSavedStreams);

	public:
		TokenizerPtr source;
		TokenStreamPtr result;
	};
}

