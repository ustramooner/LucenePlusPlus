/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NORMALIZECHARMAP_H
#define NORMALIZECHARMAP_H

#include "LuceneObject.h"

namespace Lucene
{
	/// Holds a map of String input to String output, to be used with {@link MappingCharFilter}.
	class LPPAPI NormalizeCharMap : public LuceneObject
	{
		MapCharNormalizeCharMap submap;
		String normStr;
		int32_t diff;
	public:
		NormalizeCharMap();
		virtual ~NormalizeCharMap();
		
		LUCENE_CLASS(NormalizeCharMap);

		/// Records a replacement to be applied to the inputs stream.  Whenever singleMatch occurs in the input, it 
		/// will be replaced with replacement.
		///
		/// @param singleMatch input String to be replaced
		/// @param replacement output String
		void add(const String& singleMatch, const String& replacement);

		friend class MappingCharFilter;
	};
}

#endif
