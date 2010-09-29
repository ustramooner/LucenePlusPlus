#pragma once
#include "DocIdSet.h"

namespace Lucene
{

	class EmptyDocIdSetIterator : public DocIdSetIterator
	{
	public:
		virtual ~EmptyDocIdSetIterator();
		LUCENE_CLASS(EmptyDocIdSetIterator);
	
	public:
		virtual int32_t advance(int32_t target);
		virtual int32_t docID();
		virtual int32_t nextDoc();
	};
	
	/// An empty {@code DocIdSet} instance for easy use, eg. in Filters that hit no documents.
	class EmptyDocIdSet : public DocIdSet
	{
	public:
		virtual ~EmptyDocIdSet();
		LUCENE_CLASS(EmptyDocIdSet);
	
	public:
		virtual DocIdSetIteratorPtr iterator();
		virtual bool isCacheable();
	};
}

