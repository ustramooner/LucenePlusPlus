#pragma once
#include "IntFieldSource.h"

namespace Lucene
{

	class IntDocValues : public DocValues
	{
	public:
		IntDocValues(IntFieldSourcePtr source, Collection<int32_t> arr);
		virtual ~IntDocValues();
	
		LUCENE_CLASS(IntDocValues);
	
	protected:
		IntFieldSourceWeakPtr _source;
		Collection<int32_t> arr;
	
	public:
		virtual double doubleVal(int32_t doc);
		virtual int32_t intVal(int32_t doc);
		virtual String toString(int32_t doc);
		virtual CollectionValue getInnerArray();
	};
}

