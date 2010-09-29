#pragma once
#include "IndexReader.h"

namespace Lucene {
	class FindSegmentsModified : public FindSegmentsFileT<uint64_t>
	{
	public:
		FindSegmentsModified(SegmentInfosPtr infos, DirectoryPtr directory);
		virtual ~FindSegmentsModified();
		
		LUCENE_CLASS(FindSegmentsModified);
				
	public:
		virtual uint64_t doBody(const String& segmentFileName);
	};
}

