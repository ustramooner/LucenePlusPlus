#pragma once
#include "CachingSpanFilter.h"

#include "_CachingWrapperFilter.h"
namespace Lucene
{

	class FilterCacheSpanFilterResult : public FilterCache
	{
	public:
	    FilterCacheSpanFilterResult(CachingWrapperFilter::DeletesMode deletesMode);
	    virtual ~FilterCacheSpanFilterResult();
	    
	    LUCENE_CLASS(FilterCacheSpanFilterResult);

    protected:
        virtual LuceneObjectPtr mergeDeletes(IndexReaderPtr reader, LuceneObjectPtr value);
	};
}

