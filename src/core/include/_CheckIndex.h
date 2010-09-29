#pragma once
#include "CheckIndex.h"

#include "SegmentTermDocs.h"

namespace Lucene {

	class MySegmentTermDocs : public SegmentTermDocs
	{
	public:
		MySegmentTermDocs(SegmentReaderPtr p);
		virtual ~MySegmentTermDocs();
		
		LUCENE_CLASS(MySegmentTermDocs);
	
	public:
		int32_t delCount;
	
	public:
		virtual void seek(TermPtr term);
		virtual void skippingDoc();
	};
}

