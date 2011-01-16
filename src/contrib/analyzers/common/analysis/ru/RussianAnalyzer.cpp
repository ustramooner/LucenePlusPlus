/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "ContribInc.h"
#include "RussianAnalyzer.h"
#include "RussianLetterTokenizer.h"
#include "LowerCaseFilter.h"
#include "StopFilter.h"
#include "RussianStemFilter.h"
#include "StringUtils.h"

namespace Lucene
{
    /// Default Russian stopwords in UTF-8 format.
    const uint8_t RussianAnalyzer::DEFAULT_STOPWORD_FILE[] = 
    {
        0xd0, 0xb0, 0x0a, 0xd0, 0xb1, 0xd0, 0xb5, 0xd0, 0xb7, 0x0a, 0xd0, 0xb1, 0xd0, 0xbe, 0xd0, 0xbb, 
        0xd0, 0xb5, 0xd0, 0xb5, 0x0a, 0xd0, 0xb1, 0xd1, 0x8b, 0x0a, 0xd0, 0xb1, 0xd1, 0x8b, 0xd0, 0xbb, 
        0x0a, 0xd0, 0xb1, 0xd1, 0x8b, 0xd0, 0xbb, 0xd0, 0xb0, 0x0a, 0xd0, 0xb1, 0xd1, 0x8b, 0xd0, 0xbb, 
        0xd0, 0xb8, 0x0a, 0xd0, 0xb1, 0xd1, 0x8b, 0xd0, 0xbb, 0xd0, 0xbe, 0x0a, 0xd0, 0xb1, 0xd1, 0x8b, 
        0xd1, 0x82, 0xd1, 0x8c, 0x0a, 0xd0, 0xb2, 0x0a, 0xd0, 0xb2, 0xd0, 0xb0, 0xd0, 0xbc, 0x0a, 0xd0, 
        0xb2, 0xd0, 0xb0, 0xd1, 0x81, 0x0a, 0xd0, 0xb2, 0xd0, 0xb5, 0xd1, 0x81, 0xd1, 0x8c, 0x0a, 0xd0, 
        0xb2, 0xd0, 0xbe, 0x0a, 0xd0, 0xb2, 0xd0, 0xbe, 0xd1, 0x82, 0x0a, 0xd0, 0xb2, 0xd1, 0x81, 0xd0, 
        0xb5, 0x0a, 0xd0, 0xb2, 0xd1, 0x81, 0xd0, 0xb5, 0xd0, 0xb3, 0xd0, 0xbe, 0x0a, 0xd0, 0xb2, 0xd1, 
        0x81, 0xd0, 0xb5, 0xd1, 0x85, 0x0a, 0xd0, 0xb2, 0xd1, 0x8b, 0x0a, 0xd0, 0xb3, 0xd0, 0xb4, 0xd0, 
        0xb5, 0x0a, 0xd0, 0xb4, 0xd0, 0xb0, 0x0a, 0xd0, 0xb4, 0xd0, 0xb0, 0xd0, 0xb6, 0xd0, 0xb5, 0x0a, 
        0xd0, 0xb4, 0xd0, 0xbb, 0xd1, 0x8f, 0x0a, 0xd0, 0xb4, 0xd0, 0xbe, 0x0a, 0xd0, 0xb5, 0xd0, 0xb3, 
        0xd0, 0xbe, 0x0a, 0xd0, 0xb5, 0xd0, 0xb5, 0x0a, 0xd0, 0xb5, 0xd0, 0xb9, 0x0a, 0xd0, 0xb5, 0xd1, 
        0x8e, 0x0a, 0xd0, 0xb5, 0xd1, 0x81, 0xd0, 0xbb, 0xd0, 0xb8, 0x0a, 0xd0, 0xb5, 0xd1, 0x81, 0xd1, 
        0x82, 0xd1, 0x8c, 0x0a, 0xd0, 0xb5, 0xd1, 0x89, 0xd0, 0xb5, 0x0a, 0xd0, 0xb6, 0xd0, 0xb5, 0x0a, 
        0xd0, 0xb7, 0xd0, 0xb0, 0x0a, 0xd0, 0xb7, 0xd0, 0xb4, 0xd0, 0xb5, 0xd1, 0x81, 0xd1, 0x8c, 0x0a, 
        0xd0, 0xb8, 0x0a, 0xd0, 0xb8, 0xd0, 0xb7, 0x0a, 0xd0, 0xb8, 0xd0, 0xbb, 0xd0, 0xb8, 0x0a, 0xd0, 
        0xb8, 0xd0, 0xbc, 0x0a, 0xd0, 0xb8, 0xd1, 0x85, 0x0a, 0xd0, 0xba, 0x0a, 0xd0, 0xba, 0xd0, 0xb0, 
        0xd0, 0xba, 0x0a, 0xd0, 0xba, 0xd0, 0xbe, 0x0a, 0xd0, 0xba, 0xd0, 0xbe, 0xd0, 0xb3, 0xd0, 0xb4, 
        0xd0, 0xb0, 0x0a, 0xd0, 0xba, 0xd1, 0x82, 0xd0, 0xbe, 0x0a, 0xd0, 0xbb, 0xd0, 0xb8, 0x0a, 0xd0, 
        0xbb, 0xd0, 0xb8, 0xd0, 0xb1, 0xd0, 0xbe, 0x0a, 0xd0, 0xbc, 0xd0, 0xbd, 0xd0, 0xb5, 0x0a, 0xd0, 
        0xbc, 0xd0, 0xbe, 0xd0, 0xb6, 0xd0, 0xb5, 0xd1, 0x82, 0x0a, 0xd0, 0xbc, 0xd1, 0x8b, 0x0a, 0xd0, 
        0xbd, 0xd0, 0xb0, 0x0a, 0xd0, 0xbd, 0xd0, 0xb0, 0xd0, 0xb4, 0xd0, 0xbe, 0x0a, 0xd0, 0xbd, 0xd0, 
        0xb0, 0xd1, 0x88, 0x0a, 0xd0, 0xbd, 0xd0, 0xb5, 0x0a, 0xd0, 0xbd, 0xd0, 0xb5, 0xd0, 0xb3, 0xd0, 
        0xbe, 0x0a, 0xd0, 0xbd, 0xd0, 0xb5, 0xd0, 0xb5, 0x0a, 0xd0, 0xbd, 0xd0, 0xb5, 0xd1, 0x82, 0x0a, 
        0xd0, 0xbd, 0xd0, 0xb8, 0x0a, 0xd0, 0xbd, 0xd0, 0xb8, 0xd1, 0x85, 0x0a, 0xd0, 0xbd, 0xd0, 0xbe, 
        0x0a, 0xd0, 0xbd, 0xd1, 0x83, 0x0a, 0xd0, 0xbe, 0x0a, 0xd0, 0xbe, 0xd0, 0xb1, 0x0a, 0xd0, 0xbe, 
        0xd0, 0xb4, 0xd0, 0xbd, 0xd0, 0xb0, 0xd0, 0xba, 0xd0, 0xbe, 0x0a, 0xd0, 0xbe, 0xd0, 0xbd, 0x0a, 
        0xd0, 0xbe, 0xd0, 0xbd, 0xd0, 0xb0, 0x0a, 0xd0, 0xbe, 0xd0, 0xbd, 0xd0, 0xb8, 0x0a, 0xd0, 0xbe, 
        0xd0, 0xbd, 0xd0, 0xbe, 0x0a, 0xd0, 0xbe, 0xd1, 0x82, 0x0a, 0xd0, 0xbe, 0xd1, 0x87, 0xd0, 0xb5, 
        0xd0, 0xbd, 0xd1, 0x8c, 0x0a, 0xd0, 0xbf, 0xd0, 0xbe, 0x0a, 0xd0, 0xbf, 0xd0, 0xbe, 0xd0, 0xb4, 
        0x0a, 0xd0, 0xbf, 0xd1, 0x80, 0xd0, 0xb8, 0x0a, 0xd1, 0x81, 0x0a, 0xd1, 0x81, 0xd0, 0xbe, 0x0a, 
        0xd1, 0x82, 0xd0, 0xb0, 0xd0, 0xba, 0x0a, 0xd1, 0x82, 0xd0, 0xb0, 0xd0, 0xba, 0xd0, 0xb6, 0xd0, 
        0xb5, 0x0a, 0xd1, 0x82, 0xd0, 0xb0, 0xd0, 0xba, 0xd0, 0xbe, 0xd0, 0xb9, 0x0a, 0xd1, 0x82, 0xd0, 
        0xb0, 0xd0, 0xbc, 0x0a, 0xd1, 0x82, 0xd0, 0xb5, 0x0a, 0xd1, 0x82, 0xd0, 0xb5, 0xd0, 0xbc, 0x0a, 
        0xd1, 0x82, 0xd0, 0xbe, 0x0a, 0xd1, 0x82, 0xd0, 0xbe, 0xd0, 0xb3, 0xd0, 0xbe, 0x0a, 0xd1, 0x82, 
        0xd0, 0xbe, 0xd0, 0xb6, 0xd0, 0xb5, 0x0a, 0xd1, 0x82, 0xd0, 0xbe, 0xd0, 0xb9, 0x0a, 0xd1, 0x82, 
        0xd0, 0xbe, 0xd0, 0xbb, 0xd1, 0x8c, 0xd0, 0xba, 0xd0, 0xbe, 0x0a, 0xd1, 0x82, 0xd0, 0xbe, 0xd0, 
        0xbc, 0x0a, 0xd1, 0x82, 0xd1, 0x8b, 0x0a, 0xd1, 0x83, 0x0a, 0xd1, 0x83, 0xd0, 0xb6, 0xd0, 0xb5, 
        0x0a, 0xd1, 0x85, 0xd0, 0xbe, 0xd1, 0x82, 0xd1, 0x8f, 0x0a, 0xd1, 0x87, 0xd0, 0xb5, 0xd0, 0xb3, 
        0xd0, 0xbe, 0x0a, 0xd1, 0x87, 0xd0, 0xb5, 0xd0, 0xb9, 0x0a, 0xd1, 0x87, 0xd0, 0xb5, 0xd0, 0xbc, 
        0x0a, 0xd1, 0x87, 0xd1, 0x82, 0xd0, 0xbe, 0x0a, 0xd1, 0x87, 0xd1, 0x82, 0xd0, 0xbe, 0xd0, 0xb1, 
        0xd1, 0x8b, 0x0a, 0xd1, 0x87, 0xd1, 0x8c, 0xd0, 0xb5, 0x0a, 0xd1, 0x87, 0xd1, 0x8c, 0xd1, 0x8f, 
        0x0a, 0xd1, 0x8d, 0xd1, 0x82, 0xd0, 0xb0, 0x0a, 0xd1, 0x8d, 0xd1, 0x82, 0xd0, 0xb8, 0x0a, 0xd1, 
        0x8d, 0xd1, 0x82, 0xd0, 0xbe, 0x0a, 0xd1, 0x8f, 0x0a
    };
    
    RussianAnalyzer::RussianAnalyzer(LuceneVersion::Version matchVersion)
    {
        this->stopSet = getDefaultStopSet();
        this->matchVersion = matchVersion;
    }
    
    RussianAnalyzer::RussianAnalyzer(LuceneVersion::Version matchVersion, HashSet<String> stopwords)
    {
        this->stopSet = stopwords;
        this->matchVersion = matchVersion;
    }
    
    RussianAnalyzer::~RussianAnalyzer()
    {
    }
    
    const HashSet<String> RussianAnalyzer::getDefaultStopSet()
    {
        static HashSet<String> stopSet;
        if (!stopSet)
        {
            String stopWords(UTF8_TO_STRING(DEFAULT_STOPWORD_FILE));
            Collection<String> words(StringUtils::split(stopWords, L"\n"));
            stopSet = HashSet<String>::newInstance(words.begin(), words.end());
        }
        return stopSet;
    }
    
    TokenStreamPtr RussianAnalyzer::tokenStream(const String& fieldName, ReaderPtr reader)
    {
        TokenStreamPtr result = newLucene<RussianLetterTokenizer>(reader);
        result = newLucene<LowerCaseFilter>(result);
        result = newLucene<StopFilter>(StopFilter::getEnablePositionIncrementsVersionDefault(matchVersion), result, stopSet);
        result = newLucene<RussianStemFilter>(result);
        return result;
    }
    
    TokenStreamPtr RussianAnalyzer::reusableTokenStream(const String& fieldName, ReaderPtr reader)
    {
        RussianAnalyzerSavedStreamsPtr streams(boost::dynamic_pointer_cast<RussianAnalyzerSavedStreams>(getPreviousTokenStream()));
        if (!streams)
        {
            streams = newLucene<RussianAnalyzerSavedStreams>();
            streams->source = newLucene<RussianLetterTokenizer>(reader);
            streams->result = newLucene<LowerCaseFilter>(streams->source);
            streams->result = newLucene<StopFilter>(StopFilter::getEnablePositionIncrementsVersionDefault(matchVersion), streams->result, stopSet);
            streams->result = newLucene<RussianStemFilter>(streams->result);
            setPreviousTokenStream(streams);
        }
        else
            streams->source->reset(reader);
        return streams->result;
    }
    
    RussianAnalyzerSavedStreams::~RussianAnalyzerSavedStreams()
    {
    }
}
