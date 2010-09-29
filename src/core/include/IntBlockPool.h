/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LuceneObject.h"

namespace Lucene
{
	class IntBlockPool : public LuceneObject
	{
	public:
		IntBlockPool(DocumentsWriterPtr docWriter, bool trackAllocations);
		virtual ~IntBlockPool();
		
		LUCENE_CLASS(IntBlockPool);
			
	public:
		Collection<IntArray> buffers;
		
		int32_t bufferUpto; // Which buffer we are upto
		int32_t intUpto; // Where we are in head buffer
		
		IntArray buffer; // Current head buffer
		int32_t intOffset; // Current head offset
		bool trackAllocations;
		
	protected:
		DocumentsWriterWeakPtr _docWriter;
	
	public:
		void reset();
		void nextBuffer();
	};
}
