/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WhitespaceTokenizer.h"
#include "UnicodeUtils.h"
#include <locale>

namespace Lucene
{
    WhitespaceTokenizer::WhitespaceTokenizer(ReaderPtr input) : CharTokenizer(input)
    {
    }
    
    WhitespaceTokenizer::WhitespaceTokenizer(AttributeSourcePtr source, ReaderPtr input) : CharTokenizer(source, input)
    {
    }
    
    WhitespaceTokenizer::WhitespaceTokenizer(AttributeFactoryPtr factory, ReaderPtr input) : CharTokenizer(factory, input)
    {
    }
    
    WhitespaceTokenizer::~WhitespaceTokenizer()
    {
    }
    
    bool WhitespaceTokenizer::isTokenChar(wchar_t c)
    {
        return !UnicodeUtil::isSpace(c);
    }
}
