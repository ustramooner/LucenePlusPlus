/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <iostream>
#include "InfoStream.h"
#include "StringUtils.h"

namespace Lucene
{
    InfoStream::~InfoStream()
    {
    }
    
    InfoStreamFile::InfoStreamFile(const String& path) : file(StringUtils::toUTF8(path).c_str())
    {
    }
    
    InfoStreamFile::~InfoStreamFile()
    {
    }
    
    InfoStreamFile& InfoStreamFile::operator<< (const String& t)
    {
        file << t;
        return *this;
    }
    
    InfoStreamOut::~InfoStreamOut()
    {
    }
    
    InfoStreamOut& InfoStreamOut::operator<< (const String& t)
    {
        std::wcout << t;
        return *this;
    }
}
