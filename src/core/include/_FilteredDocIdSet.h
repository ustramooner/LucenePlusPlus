#pragma once
#include "FilteredDocIdSet.h"

namespace Lucene
{

	/// Implementation of the contract to build a DocIdSetIterator.
	class DefaultFilteredDocIdSetIterator : public FilteredDocIdSetIterator
	{
	public:
		DefaultFilteredDocIdSetIterator(FilteredDocIdSetPtr filtered, DocIdSetIteratorPtr innerIter);
		virtual ~DefaultFilteredDocIdSetIterator();
	
		LUCENE_CLASS(DefaultFilteredDocIdSetIterator);
	
	protected:
		FilteredDocIdSetPtr filtered;
	
	protected:
		virtual bool match(int32_t docid);
	};
}

