/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GreekAnalyzer.h"
#include "StandardTokenizer.h"
#include "GreekLowerCaseFilter.h"
#include "StringUtils.h"
#include "StopFilter.h"

namespace Lucene
{
    /// Default Greek stopwords in UTF-8 format.
    const uint8_t GreekAnalyzer::_GREEK_STOP_WORDS[] = 
    {
        0xce, 0xbf, 0x0a, 0xce, 0xb7, 0x0a, 0xcf, 0x84, 0xce, 0xbf, 0x0a, 0xce, 0xbf, 0xce, 0xb9, 0x0a,
        0xcf, 0x84, 0xce, 0xb1, 0x0a, 0xcf, 0x84, 0xce, 0xbf, 0xcf, 0x85, 0x0a, 0xcf, 0x84, 0xce, 0xb7, 
        0xcf, 0x83, 0x0a, 0xcf, 0x84, 0xcf, 0x89, 0xce, 0xbd, 0x0a, 0xcf, 0x84, 0xce, 0xbf, 0xce, 0xbd, 
        0x0a, 0xcf, 0x84, 0xce, 0xb7, 0xce, 0xbd, 0x0a, 0xce, 0xba, 0xce, 0xb1, 0xce, 0xb9, 0x0a, 0xce, 
        0xba, 0xce, 0xb9, 0x0a, 0xce, 0xba, 0x0a, 0xce, 0xb5, 0xce, 0xb9, 0xce, 0xbc, 0xce, 0xb1, 0xce, 
        0xb9, 0x0a, 0xce, 0xb5, 0xce, 0xb9, 0xcf, 0x83, 0xce, 0xb1, 0xce, 0xb9, 0x0a, 0xce, 0xb5, 0xce, 
        0xb9, 0xce, 0xbd, 0xce, 0xb1, 0xce, 0xb9, 0x0a, 0xce, 0xb5, 0xce, 0xb9, 0xce, 0xbc, 0xce, 0xb1, 
        0xcf, 0x83, 0xcf, 0x84, 0xce, 0xb5, 0x0a, 0xce, 0xb5, 0xce, 0xb9, 0xcf, 0x83, 0xcf, 0x84, 0xce, 
        0xb5, 0x0a, 0xcf, 0x83, 0xcf, 0x84, 0xce, 0xbf, 0x0a, 0xcf, 0x83, 0xcf, 0x84, 0xce, 0xbf, 0xce, 
        0xbd, 0x0a, 0xcf, 0x83, 0xcf, 0x84, 0xce, 0xb7, 0x0a, 0xcf, 0x83, 0xcf, 0x84, 0xce, 0xb7, 0xce, 
        0xbd, 0x0a, 0xce, 0xbc, 0xce, 0xb1, 0x0a, 0xce, 0xb1, 0xce, 0xbb, 0xce, 0xbb, 0xce, 0xb1, 0x0a, 
        0xce, 0xb1, 0xcf, 0x80, 0xce, 0xbf, 0x0a, 0xce, 0xb3, 0xce, 0xb9, 0xce, 0xb1, 0x0a, 0xcf, 0x80, 
        0xcf, 0x81, 0xce, 0xbf, 0xcf, 0x83, 0x0a, 0xce, 0xbc, 0xce, 0xb5, 0x0a, 0xcf, 0x83, 0xce, 0xb5, 
        0x0a, 0xcf, 0x89, 0xcf, 0x83, 0x0a, 0xcf, 0x80, 0xce, 0xb1, 0xcf, 0x81, 0xce, 0xb1, 0x0a, 0xce, 
        0xb1, 0xce, 0xbd, 0xcf, 0x84, 0xce, 0xb9, 0x0a, 0xce, 0xba, 0xce, 0xb1, 0xcf, 0x84, 0xce, 0xb1, 
        0x0a, 0xce, 0xbc, 0xce, 0xb5, 0xcf, 0x84, 0xce, 0xb1, 0x0a, 0xce, 0xb8, 0xce, 0xb1, 0x0a, 0xce, 
        0xbd, 0xce, 0xb1, 0x0a, 0xce, 0xb4, 0xce, 0xb5, 0x0a, 0xce, 0xb4, 0xce, 0xb5, 0xce, 0xbd, 0x0a, 
        0xce, 0xbc, 0xce, 0xb7, 0x0a, 0xce, 0xbc, 0xce, 0xb7, 0xce, 0xbd, 0x0a, 0xce, 0xb5, 0xcf, 0x80, 
        0xce, 0xb9, 0x0a, 0xce, 0xb5, 0xce, 0xbd, 0xcf, 0x89, 0x0a, 0xce, 0xb5, 0xce, 0xb1, 0xce, 0xbd, 
        0x0a, 0xce, 0xb1, 0xce, 0xbd, 0x0a, 0xcf, 0x84, 0xce, 0xbf, 0xcf, 0x84, 0xce, 0xb5, 0x0a, 0xcf, 
        0x80, 0xce, 0xbf, 0xcf, 0x85, 0x0a, 0xcf, 0x80, 0xcf, 0x89, 0xcf, 0x83, 0x0a, 0xcf, 0x80, 0xce, 
        0xbf, 0xce, 0xb9, 0xce, 0xbf, 0xcf, 0x83, 0x0a, 0xcf, 0x80, 0xce, 0xbf, 0xce, 0xb9, 0xce, 0xb1, 
        0x0a, 0xcf, 0x80, 0xce, 0xbf, 0xce, 0xb9, 0xce, 0xbf, 0x0a, 0xcf, 0x80, 0xce, 0xbf, 0xce, 0xb9, 
        0xce, 0xbf, 0xce, 0xb9, 0x0a, 0xcf, 0x80, 0xce, 0xbf, 0xce, 0xb9, 0xce, 0xb5, 0xcf, 0x83, 0x0a, 
        0xcf, 0x80, 0xce, 0xbf, 0xce, 0xb9, 0xcf, 0x89, 0xce, 0xbd, 0x0a, 0xcf, 0x80, 0xce, 0xbf, 0xce, 
        0xb9, 0xce, 0xbf, 0xcf, 0x85, 0xcf, 0x83, 0x0a, 0xce, 0xb1, 0xcf, 0x85, 0xcf, 0x84, 0xce, 0xbf, 
        0xcf, 0x83, 0x0a, 0xce, 0xb1, 0xcf, 0x85, 0xcf, 0x84, 0xce, 0xb7, 0x0a, 0xce, 0xb1, 0xcf, 0x85, 
        0xcf, 0x84, 0xce, 0xbf, 0x0a, 0xce, 0xb1, 0xcf, 0x85, 0xcf, 0x84, 0xce, 0xbf, 0xce, 0xb9, 0x0a, 
        0xce, 0xb1, 0xcf, 0x85, 0xcf, 0x84, 0xcf, 0x89, 0xce, 0xbd, 0x0a, 0xce, 0xb1, 0xcf, 0x85, 0xcf, 
        0x84, 0xce, 0xbf, 0xcf, 0x85, 0xcf, 0x83, 0x0a, 0xce, 0xb1, 0xcf, 0x85, 0xcf, 0x84, 0xce, 0xb5, 
        0xcf, 0x83, 0x0a, 0xce, 0xb1, 0xcf, 0x85, 0xcf, 0x84, 0xce, 0xb1, 0x0a, 0xce, 0xb5, 0xce, 0xba, 
        0xce, 0xb5, 0xce, 0xb9, 0xce, 0xbd, 0xce, 0xbf, 0xcf, 0x83, 0x0a, 0xce, 0xb5, 0xce, 0xba, 0xce, 
        0xb5, 0xce, 0xb9, 0xce, 0xbd, 0xce, 0xb7, 0x0a, 0xce, 0xb5, 0xce, 0xba, 0xce, 0xb5, 0xce, 0xb9, 
        0xce, 0xbd, 0xce, 0xbf, 0x0a, 0xce, 0xb5, 0xce, 0xba, 0xce, 0xb5, 0xce, 0xb9, 0xce, 0xbd, 0xce, 
        0xbf, 0xce, 0xb9, 0x0a, 0xce, 0xb5, 0xce, 0xba, 0xce, 0xb5, 0xce, 0xb9, 0xce, 0xbd, 0xce, 0xb5, 
        0xcf, 0x83, 0x0a, 0xce, 0xb5, 0xce, 0xba, 0xce, 0xb5, 0xce, 0xb9, 0xce, 0xbd, 0xce, 0xb1, 0x0a, 
        0xce, 0xb5, 0xce, 0xba, 0xce, 0xb5, 0xce, 0xb9, 0xce, 0xbd, 0xcf, 0x89, 0xce, 0xbd, 0x0a, 0xce, 
        0xb5, 0xce, 0xba, 0xce, 0xb5, 0xce, 0xb9, 0xce, 0xbd, 0xce, 0xbf, 0xcf, 0x85, 0xcf, 0x83, 0x0a, 
        0xce, 0xbf, 0xcf, 0x80, 0xcf, 0x89, 0xcf, 0x83, 0x0a, 0xce, 0xbf, 0xce, 0xbc, 0xcf, 0x89, 0xcf, 
        0x83, 0x0a, 0xce, 0xb9, 0xcf, 0x83, 0xcf, 0x89, 0xcf, 0x83, 0x0a, 0xce, 0xbf, 0xcf, 0x83, 0xce, 
        0xbf, 0x0a, 0xce, 0xbf, 0xcf, 0x84, 0xce, 0xb9, 0x0a
    };
    
    GreekAnalyzer::GreekAnalyzer(LuceneVersion::Version matchVersion)
    {
        this->stopSet = getDefaultStopSet();
        this->matchVersion = matchVersion;
    }
    
    GreekAnalyzer::GreekAnalyzer(LuceneVersion::Version matchVersion, HashSet<String> stopwords)
    {
        this->stopSet = stopwords;
        this->matchVersion = matchVersion;
    }
    
    GreekAnalyzer::~GreekAnalyzer()
    {
    }
    
    HashSet<String> GreekAnalyzer::getDefaultStopSet()
    {
        static HashSet<String> stopSet;
        if (!stopSet)
        {
            String stopWords(UTF8_TO_STRING(_GREEK_STOP_WORDS));
            Collection<String> words(StringUtils::split(stopWords, L"\n"));
            stopSet = HashSet<String>::newInstance(words.begin(), words.end());
        }
        return stopSet;
    }
    
    TokenStreamPtr GreekAnalyzer::tokenStream(const String& fieldName, ReaderPtr reader)
    {
        TokenStreamPtr result = newLucene<StandardTokenizer>(matchVersion, reader);
        result = newLucene<GreekLowerCaseFilter>(result);
        result = newLucene<StopFilter>(StopFilter::getEnablePositionIncrementsVersionDefault(matchVersion), result, stopSet);
        return result;
    }
    
    TokenStreamPtr GreekAnalyzer::reusableTokenStream(const String& fieldName, ReaderPtr reader)
    {
        GreekAnalyzerSavedStreamsPtr streams(boost::dynamic_pointer_cast<GreekAnalyzerSavedStreams>(getPreviousTokenStream()));
        if (!streams)
        {
            streams = newLucene<GreekAnalyzerSavedStreams>();
            streams->source = newLucene<StandardTokenizer>(matchVersion, reader);
            streams->result = newLucene<GreekLowerCaseFilter>(streams->source);
            streams->result = newLucene<StopFilter>(StopFilter::getEnablePositionIncrementsVersionDefault(matchVersion), streams->result, stopSet);
            setPreviousTokenStream(streams);
        }
        else
            streams->source->reset(reader);
        return streams->result;
    }
    
    GreekAnalyzerSavedStreams::~GreekAnalyzerSavedStreams()
    {
    }
}