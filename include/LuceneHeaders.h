/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LUCENEHEADERS_H
#define LUCENEHEADERS_H

#include "Lucene.h"

// Include most common files: analysis
#include "StandardAnalyzer.h"
#include "StandardFilter.h"
#include "StandardTokenizer.h"
#include "KeywordAnalyzer.h"
#include "KeywordTokenizer.h"
#include "LowerCaseFilter.h"
#include "LowerCaseTokenizer.h"
#include "PerFieldAnalyzerWrapper.h"
#include "PorterStemFilter.h"
#include "SimpleAnalyzer.h"
#include "StopAnalyzer.h"
#include "StopFilter.h"
#include "Token.h"
#include "TokenFilter.h"
#include "WhitespaceAnalyzer.h"
#include "WhitespaceTokenizer.h"

// Include most common files: document
#include "DateField.h"
#include "DateTools.h"
#include "Document.h"
#include "Field.h"
#include "NumberTools.h"
#include "NumericField.h"

// Include most common files: index
#include "IndexCommit.h"
#include "IndexDeletionPolicy.h"
#include "IndexReader.h"
#include "IndexWriter.h"
#include "KeepOnlyLastCommitDeletionPolicy.h"
#include "LogByteSizeMergePolicy.h"
#include "LogDocMergePolicy.h"
#include "LogMergePolicy.h"
#include "MergeScheduler.h"
#include "MultiReader.h"
#include "Term.h"
#include "TermDocs.h"
#include "TermEnum.h"

// Include most common files: queryparser
#include "MultiFieldQueryParser.h"
#include "QueryParser.h"

// Include most common files: search
#include "Explanation.h"
#include "IndexSearcher.h"
#include "MultiSearcher.h"
#include "ScoreDoc.h"
#include "Scorer.h"
#include "Searcher.h"
#include "Sort.h"
#include "TermQuery.h"
#include "TermRangeFilter.h"
#include "TermRangeQuery.h"
#include "TopDocs.h"
#include "TopDocsCollector.h"
#include "TopFieldCollector.h"
#include "TopScoreDocCollector.h"
#include "Weight.h"

// Include most common files: store
#include "FSDirectory.h"
#include "MMapDirectory.h"
#include "RAMDirectory.h"
#include "SimpleFSDirectory.h"

// Include most common files: util
#include "FileReader.h"
#include "InfoStream.h"

#endif
