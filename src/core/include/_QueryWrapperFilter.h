#pragma once
#include "QueryWrapperFilter.h"

namespace Lucene
{

	class QueryWrapperFilterDocIdSet : public DocIdSet
	{
	public:
		QueryWrapperFilterDocIdSet(IndexReaderPtr reader, WeightPtr weight);
		virtual ~QueryWrapperFilterDocIdSet();
	
		LUCENE_CLASS(QueryWrapperFilterDocIdSet);
	
	protected:
		IndexReaderPtr reader;
		WeightPtr weight;
	
	public:
		virtual DocIdSetIteratorPtr iterator();
		virtual bool isCacheable();
	};

}

