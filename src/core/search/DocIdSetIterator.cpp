/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DocIdSetIterator.h"

namespace Lucene
{
    /// When returned by {@link #nextDoc()}, {@link #advance(int)} and {@link #docID()} it means there
    /// docs in the iterator.
    const int32_t DocIdSetIterator::NO_MORE_DOCS = INT_MAX;
    
    DocIdSetIterator::~DocIdSetIterator()
    {
    }
}
