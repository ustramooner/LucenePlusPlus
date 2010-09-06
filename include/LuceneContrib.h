/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

namespace Lucene
{
	// analyzers
	DECLARE_SHARED_PTR(ArabicAnalyzer)
	DECLARE_SHARED_PTR(ArabicAnalyzerSavedStreams)
	DECLARE_SHARED_PTR(ArabicLetterTokenizer)
	DECLARE_SHARED_PTR(ArabicNormalizationFilter)
	DECLARE_SHARED_PTR(ArabicNormalizer)
	DECLARE_SHARED_PTR(ArabicStemFilter)
	DECLARE_SHARED_PTR(ArabicStemmer)
	DECLARE_SHARED_PTR(BrazilianAnalyzer)
	DECLARE_SHARED_PTR(BrazilianAnalyzerSavedStreams)
	DECLARE_SHARED_PTR(BrazilianStemFilter)
	DECLARE_SHARED_PTR(BrazilianStemmer)
	DECLARE_SHARED_PTR(CJKAnalyzer)
	DECLARE_SHARED_PTR(CJKAnalyzerSavedStreams)
	DECLARE_SHARED_PTR(CJKTokenizer)
	DECLARE_SHARED_PTR(ReverseStringFilter)
}
