/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TokenFilter.h"

namespace Lucene
{
    TokenFilter::TokenFilter(TokenStreamPtr input) : TokenStream(input)
    {
        this->input = input;
    }
    
    TokenFilter::~TokenFilter()
    {
    }
    
    void TokenFilter::end()
    {
        input->end();
    }
    
    void TokenFilter::close()
    {
        input->close();
    }
    
    void TokenFilter::reset()
    {
        input->reset();
    }
}
