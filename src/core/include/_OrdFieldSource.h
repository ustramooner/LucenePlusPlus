#pragma once
#include "OrdFieldSource.h"

namespace Lucene
{

	class OrdDocValues : public DocValues
	{
	public:
		OrdDocValues(OrdFieldSourcePtr source, Collection<int32_t> arr);
		virtual ~OrdDocValues();
	
		LUCENE_CLASS(OrdDocValues);
	
	protected:
		OrdFieldSourceWeakPtr _source;
		Collection<int32_t> arr;
	
	public:
		virtual double doubleVal(int32_t doc);
		virtual String strVal(int32_t doc);
		virtual String toString(int32_t doc);
		virtual CollectionValue getInnerArray();
	};

}

