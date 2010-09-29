#pragma once
#include "ReverseOrdFieldSource.h"

namespace Lucene
{

	class ReverseOrdDocValues : public DocValues
	{
	public:
		ReverseOrdDocValues(ReverseOrdFieldSourcePtr source, Collection<int32_t> arr, int32_t end);
		virtual ~ReverseOrdDocValues();
	
		LUCENE_CLASS(ReverseOrdDocValues);
	
	protected:
		ReverseOrdFieldSourceWeakPtr _source;
		Collection<int32_t> arr;
		int32_t end;
	
	public:
		virtual double doubleVal(int32_t doc);
		virtual int32_t intVal(int32_t doc);
		virtual String strVal(int32_t doc);
		virtual String toString(int32_t doc);
		virtual CollectionValue getInnerArray();
	};
}

