/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LuceneObject.h"

namespace Lucene
{
	class DocConsumer : public LuceneObject
	{
	public:
		virtual ~DocConsumer();
		
		LUCENE_CLASS(DocConsumer);
		
	public:
		virtual DocConsumerPerThreadPtr addThread(DocumentsWriterThreadStatePtr perThread) = 0;
		virtual void flush(Collection<DocConsumerPerThreadPtr> threads, SegmentWriteStatePtr state) = 0;
		virtual void closeDocStore(SegmentWriteStatePtr state) = 0;
		virtual void abort() = 0;
		virtual bool freeRAM() = 0;
	};
}
