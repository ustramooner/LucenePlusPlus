/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "DocIdBitSet.h"

namespace Lucene
{
	class DocIdBitSetIterator : public DocIdSetIterator
	{
	public:
		DocIdBitSetIterator(BitSetPtr bitSet);
		virtual ~DocIdBitSetIterator();
		
		LUCENE_CLASS(DocIdBitSetIterator);
	
	protected:
		int32_t docId;
		BitSetPtr bitSet;
	
	public:
		virtual int32_t docID();
		virtual int32_t nextDoc();
		virtual int32_t advance(int32_t target);
	};
}

