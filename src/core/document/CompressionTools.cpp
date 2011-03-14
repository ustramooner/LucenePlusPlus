/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "CompressionTools.h"
#include "UnicodeUtils.h"
#include "StringUtils.h"
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/copy.hpp>

namespace io = boost::iostreams;

namespace Lucene
{
    const int32_t CompressionTools::COMPRESS_BUFFER = 4096;
    
    String ZLibToMessage(int error){
      if ( error == io::zlib::okay )
          return L"okay";
      else if ( error == io::zlib::stream_end )
          return L"stream_end";
      else if ( error == io::zlib::stream_error )
          return L"stream_error";
      else if ( error == io::zlib::version_error )
          return L"version_error";
      else if ( error == io::zlib::data_error )
          return L"data_error";
      else if ( error == io::zlib::mem_error )
          return L"mem_error";
      else if ( error == io::zlib::buf_error )
          return L"buf_error";
      else
          return L"unknown";
    }
    
    class BufferArraySink : public io::sink
    {
        size_t allocSize;
    public:
        ByteArray& buffer;
        std::streamsize& position;
        
        BufferArraySink(ByteArray& _buffer, std::streamsize& _position, size_t allocSize):
          buffer(_buffer),
          position(_position)
        {
          this->allocSize = allocSize;
          this->buffer.resize((int32_t)allocSize);
        }
        
        std::streamsize write(const char* s, std::streamsize n)
        {
            if (position + n >= (std::streamsize)allocSize){
              //grow buffer
              allocSize <<= 1;
              buffer.resize((int32_t)allocSize);
            }
            memcpy(buffer.get() + position, s, n);
            position += n;
            return n;
        }
        
    };
    
    CompressionTools::~CompressionTools()
    {
    }
    
    ByteArray CompressionTools::compress(uint8_t* value, int32_t offset, int32_t length, int32_t compressionLevel)
    {
        //setup the outStream
        io::filtering_ostreambuf outStream; 
        io::zlib_compressor zcompressor(compressionLevel);
        outStream.push(zcompressor);
        
        //and the output buffer
        ByteArray buffer(ByteArray::newInstance(COMPRESS_BUFFER));
        std::streamsize position = 0;
        outStream.push(BufferArraySink(buffer, position, COMPRESS_BUFFER)); 
        
        //setup the source stream, and then copy it to the outStream
        io::stream< io::array_source > source ( (char*)(value + offset), length);
        
        try{
          io::copy(source, outStream); 
        }catch(io::zlib_error& err){
          boost::throw_exception(CompressionException(L"deflate failure: " + ZLibToMessage(err.error())));
        }
        
        buffer.resize((int32_t)position);
        
        return buffer;
    }
    
    ByteArray CompressionTools::compress(uint8_t* value, int32_t offset, int32_t length)
    {
        return compress(value, offset, length, boost::iostreams::zlib::best_compression);
    }
    
    ByteArray CompressionTools::compress(ByteArray value)
    {
        return compress(value.get(), 0, value.size(), boost::iostreams::zlib::best_compression);
    }
    
    ByteArray CompressionTools::compressString(const String& value)
    {
        return compressString(value, boost::iostreams::zlib::best_compression);
    }
    
    ByteArray CompressionTools::compressString(const String& value, int32_t compressionLevel)
    {
        UTF8ResultPtr utf8Result(newLucene<UTF8Result>());
        StringUtils::toUTF8(value.c_str(), (int32_t)value.length(), utf8Result);
        return compress(utf8Result->result.get(), 0, utf8Result->length, compressionLevel);
    }
    
    ByteArray CompressionTools::decompress(ByteArray value)
    {
        //setup the outStream
        io::filtering_ostreambuf outStream; 
        outStream.push(io::zlib_decompressor());
        
        //and the output buffer
        ByteArray buffer(ByteArray::newInstance(COMPRESS_BUFFER));
        std::streamsize position = 0;
        outStream.push(BufferArraySink(buffer, position, COMPRESS_BUFFER)); 
        
        //setup the source stream, and then copy it to the outStream
        io::stream< io::array_source > source ( (char*)value.get(), value.size() );
        
        try{
          io::copy(source, outStream); 
        }catch(io::zlib_error& err){
          boost::throw_exception(CompressionException(L"deflate failure: " + ZLibToMessage(err.error())));
        }
        
        buffer.resize((int32_t)position);
        
        return buffer;
    }
    
    String CompressionTools::decompressString(ByteArray value)
    {
        ByteArray bytes(decompress(value));
        return StringUtils::toUnicode(bytes.get(), bytes.size());
    }
}
