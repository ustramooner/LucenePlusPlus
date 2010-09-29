/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "SortedVIntList.h"

namespace Lucene
{
	class SortedDocIdSetIterator : public DocIdSetIterator
	{
	public:
		SortedDocIdSetIterator(SortedVIntListPtr list);
		virtual ~SortedDocIdSetIterator();

		LUCENE_CLASS(SortedDocIdSetIterator);

	public:
		SortedVIntListWeakPtr _list;
		int32_t bytePos;
		int32_t lastInt;
		int32_t doc;

	public:
		virtual int32_t docID();
		virtual int32_t nextDoc();
		virtual int32_t advance(int32_t target);

	protected:
		void advance();
	};
}

