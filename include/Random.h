/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LuceneObject.h"
#include "MiscUtils.h"
#include <limits.h>

namespace Lucene
{
	/// Utility class to generate a stream of pseudorandom numbers.
	class LPPAPI Random : public LuceneObject
	{
		public:
			Random(int64_t seed = (int64_t)MiscUtils::currentTimeMillis());
			virtual ~Random();

		protected:
			int64_t seed;

		public:
			void setSeed(int64_t seed);
			int32_t nextInt(int32_t limit = INT_MAX);
			double nextDouble();

		protected:
			int32_t next(int32_t bits);
	};
}
