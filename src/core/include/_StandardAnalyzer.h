#pragma once
#include "StandardAnalyzer.h"

namespace Lucene
{
	class StandardAnalyzerSavedStreams : public LuceneObject
	{
	public:
		virtual ~StandardAnalyzerSavedStreams();

	public:
		StandardTokenizerPtr tokenStream;
		TokenStreamPtr filteredTokenStream;
	};
}
