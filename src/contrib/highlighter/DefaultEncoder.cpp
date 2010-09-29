/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DefaultEncoder.h"

namespace Lucene
{
    DefaultEncoder::~DefaultEncoder()
    {
    }
    
    String DefaultEncoder::encodeText(const String& originalText)
    {
        return originalText;
    }
}
