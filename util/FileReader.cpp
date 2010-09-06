/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileReader.h"
#include "StringUtils.h"
#include "FileUtils.h"

namespace Lucene
{
    const int32_t FileReader::FILE_EOF = Reader::READER_EOF;
    const int32_t FileReader::FILE_ERROR = -1;
    
    FileReader::FileReader(const String& fileName)
    {
        this->file = newInstance<std::ifstream>(StringUtils::toUTF8(fileName).c_str(), std::ios::binary | std::ios::in);
        if (!file->is_open())
            boost::throw_exception(FileNotFoundException(fileName));
        _length = FileUtils::fileLength(fileName);
    }
    
    FileReader::~FileReader()
    {
    }
    
    int32_t FileReader::read()
    {
        wchar_t buffer;
        return read(&buffer, 0, 1) == FILE_EOF ? FILE_EOF : buffer;
    }
    
    int32_t FileReader::read(wchar_t* buffer, int32_t offset, int32_t length)
    {
        try
        {
            if (file->eof())
                return FILE_EOF;
            if (!fileBuffer)
                fileBuffer = ByteArray::newInstance(length);
            if (length > fileBuffer.length())
                fileBuffer.resize(length);
            file->read((char*)fileBuffer.get(), length);
            int32_t readLength = StringUtils::toUnicode(fileBuffer.get(), file->gcount(), buffer + offset);
            return readLength == 0 ? FILE_EOF : readLength;
        }
        catch (...)
        {
            return FILE_ERROR;
        }
    }
    
    void FileReader::close()
    {
        file->close();
    }
    
    bool FileReader::markSupported()
    {
        return false;
    }
    
    void FileReader::reset()
    {
        file->seekg((std::streamoff)0);
    }
    
    int64_t FileReader::length()
    {
        return _length;
    }
}
