#pragma once

#include "BaseCharFilter.h"

namespace Lucene
{
	class OffCorrectMap : public LuceneObject
	{
	public:
		OffCorrectMap(int32_t off, int32_t cumulativeDiff);
		virtual ~OffCorrectMap();
	
	public:
		int32_t off;
		int32_t cumulativeDiff;
	
	public:
		virtual String toString();
	};
}

