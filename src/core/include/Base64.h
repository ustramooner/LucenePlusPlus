/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LuceneObject.h"

namespace Lucene
{
	class Base64 : public LuceneObject
	{
	public:
		virtual ~Base64();
		LUCENE_CLASS(Base64);
	
	protected:
		static const String BASE64_CHARS;
		
	public:
		static String encode(ByteArray bytes);
		static String encode(const uint8_t* bytes, int32_t length);
		static ByteArray decode(const String& str);
	
	protected:
		static bool isBase64(wchar_t ch);
	};
}
