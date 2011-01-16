/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "KeywordAnalyzer.h"
#include "KeywordTokenizer.h"

namespace Lucene
{
    KeywordAnalyzer::~KeywordAnalyzer()
    {
    }
    
    TokenStreamPtr KeywordAnalyzer::tokenStream(const String& fieldName, ReaderPtr reader)
    {
        return newLucene<KeywordTokenizer>(reader);
    }
    
    TokenStreamPtr KeywordAnalyzer::reusableTokenStream(const String& fieldName, ReaderPtr reader)
    {
        TokenizerPtr tokenizer(boost::dynamic_pointer_cast<Tokenizer>(getPreviousTokenStream()));
        if (!tokenizer)
        {
            tokenizer = newLucene<KeywordTokenizer>(reader);
            setPreviousTokenStream(tokenizer);
        }
        else
            tokenizer->reset(reader);
        return tokenizer;
    }
}
