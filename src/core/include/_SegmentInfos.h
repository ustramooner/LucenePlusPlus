#pragma once
#include "SegmentInfos.h"

namespace Lucene {

	/// Utility class for executing code that needs to do something with the current segments file.  This is necessary with 
	/// lock-less commits because from the time you locate the current segments file name, until you actually open it, read 
	/// its contents, or check modified time, etc., it could have been deleted due to a writer commit finishing.
	class FindSegmentsRead : public FindSegmentsFileT<int64_t>
	{
	public:
		FindSegmentsRead(SegmentInfosPtr infos, DirectoryPtr directory);
		virtual ~FindSegmentsRead();
		
		LUCENE_CLASS(FindSegmentsRead);
				
	public:
		virtual int64_t doBody(const String& segmentFileName);
	};
}

