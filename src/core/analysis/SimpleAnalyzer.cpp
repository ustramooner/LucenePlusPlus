/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SimpleAnalyzer.h"
#include "LowerCaseTokenizer.h"

namespace Lucene
{
    SimpleAnalyzer::~SimpleAnalyzer()
    {
    }
    
    TokenStreamPtr SimpleAnalyzer::tokenStream(const String& fieldName, ReaderPtr reader)
    {
        return newLucene<LowerCaseTokenizer>(reader);
    }
    
    TokenStreamPtr SimpleAnalyzer::reusableTokenStream(const String& fieldName, ReaderPtr reader)
    {
        TokenizerPtr tokenizer(boost::dynamic_pointer_cast<Tokenizer>(getPreviousTokenStream()));
        if (!tokenizer)
        {
            tokenizer = newLucene<LowerCaseTokenizer>(reader);
            setPreviousTokenStream(tokenizer);
        }
        else
            tokenizer->reset(reader);
        return tokenizer;
    }
}
