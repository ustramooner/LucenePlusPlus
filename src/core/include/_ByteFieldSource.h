#pragma once
#include "ByteFieldSource.h"

namespace Lucene
{

	class ByteDocValues : public DocValues
	{
	public:
		ByteDocValues(ByteFieldSourcePtr source, Collection<uint8_t> arr);
		virtual ~ByteDocValues();
	
		LUCENE_CLASS(ByteDocValues);
	
	protected:
		ByteFieldSourceWeakPtr _source;
		Collection<uint8_t> arr;
	
	public:
		virtual double doubleVal(int32_t doc);
		virtual int32_t intVal(int32_t doc);
		virtual String toString(int32_t doc);
		virtual CollectionValue getInnerArray();
	};
}

