/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SegmentReader.h"

namespace Lucene
{
	class ReadOnlySegmentReader : public SegmentReader
	{
	public:
		virtual ~ReadOnlySegmentReader();
		
		LUCENE_CLASS(ReadOnlySegmentReader);
			
	public:
		static void noWrite();
		
		virtual void acquireWriteLock();
		virtual bool isDeleted(int32_t n);
	};
}