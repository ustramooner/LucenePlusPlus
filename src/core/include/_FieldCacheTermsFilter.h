#pragma once
#include "FieldCacheTermsFilter.h"

namespace Lucene
{

	class FieldCacheTermsFilterDocIdSet : public DocIdSet
	{
	public:
		FieldCacheTermsFilterDocIdSet(Collection<String> terms, StringIndexPtr fcsi);
		virtual ~FieldCacheTermsFilterDocIdSet();
		
		LUCENE_CLASS(FieldCacheTermsFilterDocIdSet);
	
	protected:
		StringIndexPtr fcsi;
		OpenBitSetPtr openBitSet;
		
	public:
		virtual DocIdSetIteratorPtr iterator();
		
		/// This DocIdSet implementation is cacheable.
		virtual bool isCacheable();
	};
	
	class FieldCacheTermsFilterDocIdSetIterator : public DocIdSetIterator
	{
	public:
		FieldCacheTermsFilterDocIdSetIterator(StringIndexPtr fcsi, OpenBitSetPtr openBitSet);
		virtual ~FieldCacheTermsFilterDocIdSetIterator();
		
		LUCENE_CLASS(FieldCacheTermsFilterDocIdSetIterator);
	
	protected:
		StringIndexPtr fcsi;
		OpenBitSetPtr openBitSet;
		int32_t doc;
	
	public:
		virtual int32_t docID();
		virtual int32_t nextDoc();
		virtual int32_t advance(int32_t target);
	};
}

