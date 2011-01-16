/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef UTF8STREAM_H
#define UTF8STREAM_H

#include "LuceneObject.h"

namespace Lucene
{
  class UTF8Base : public LuceneObject
	{
	public:
        virtual ~UTF8Base();
        LUCENE_CLASS(UTF8Base);
    
    public:
        static const uint16_t LEAD_SURROGATE_MIN;
        static const uint16_t LEAD_SURROGATE_MAX;
        static const uint16_t TRAIL_SURROGATE_MIN;
        static const uint16_t TRAIL_SURROGATE_MAX;
        static const uint16_t LEAD_OFFSET;
        static const uint32_t SURROGATE_OFFSET;
        static const uint32_t CODE_POINT_MAX;
        
        static const wchar_t UNICODE_REPLACEMENT_CHAR;
        static const wchar_t UNICODE_TERMINATOR;
    
    protected:
        virtual uint32_t readNext() = 0;
        
        inline uint8_t mask8(uint32_t b)
        {
            return static_cast<uint8_t>(0xff & b);
        }

        inline uint16_t mask16(uint32_t c)
        {
            return static_cast<uint16_t>(0xffff & c);
        }

        inline bool isTrail(uint32_t b)
        {
            return ((mask8(b) >> 6) == 0x2);
        }
        
        inline bool isSurrogate(uint32_t cp)
        {
            return (cp >= LEAD_SURROGATE_MIN && cp <= TRAIL_SURROGATE_MAX);
        }

        inline bool isLeadSurrogate(uint32_t cp)
        {
            return (cp >= LEAD_SURROGATE_MIN && cp <= LEAD_SURROGATE_MAX);
        }

        inline bool isTrailSurrogate(uint32_t cp)
        {
            return (cp >= TRAIL_SURROGATE_MIN && cp <= TRAIL_SURROGATE_MAX);
        }
        
        inline bool isValidCodePoint(uint32_t cp)
        {
            return (cp <= CODE_POINT_MAX && !isSurrogate(cp) && cp != 0xfffe && cp != 0xffff);
        }
        
        inline bool isOverlongSequence(uint32_t cp, int32_t length)
        {
            if (cp < 0x80)
            {
                if (length != 1) 
                    return true;
            }
            else if (cp < 0x800)
            {
                if (length != 2) 
                    return true;
            }
            else if (cp < 0x10000)
            {
                if (length != 3) 
                    return true;
            }
            return false;
        }
	};
	
  class UTF8Encoder : public UTF8Base
	{
	public:
    UTF8Encoder(const wchar_t* unicodeBegin, const wchar_t* unicodeEnd);
		virtual ~UTF8Encoder();
		
		LUCENE_CLASS(UTF8Encoder);
    
    protected:
        const wchar_t* unicodeBegin;
        const wchar_t* unicodeEnd;
    
    public:
        int32_t encode(uint8_t* utf8, int32_t length);
        
        int32_t utf16to8(uint8_t* utf8, int32_t length);
        int32_t utf32to8(uint8_t* utf8, int32_t length);
        
    protected:
        virtual uint32_t readNext();
        uint8_t* appendChar(uint8_t* utf8, uint32_t cp);
	};
	
  class UTF8EncoderStream : public UTF8Encoder
  {
  public:
      UTF8EncoderStream(ReaderPtr reader);
      virtual ~UTF8EncoderStream();
      
      LUCENE_CLASS(UTF8EncoderStream);
  
  protected:
      ReaderPtr reader;
  
  protected:
      virtual uint32_t readNext();
  };
    
  class UTF8Decoder : public UTF8Base
	{
	public:
    UTF8Decoder(const uint8_t* utf8Begin, const uint8_t* utf8End);
		virtual ~UTF8Decoder();
		
		LUCENE_CLASS(UTF8Decoder);
    
    protected:
        const uint8_t* utf8Begin;
        const uint8_t* utf8End;
    
    public:
        int32_t decode(wchar_t* unicode, int32_t length);
        
        int32_t utf8to16(wchar_t* unicode, int32_t length);
        int32_t utf8to32(wchar_t* unicode, int32_t length);
    
    protected:
        inline int32_t sequenceLength(uint32_t cp){
          uint8_t lead = mask8(cp);
          if (lead < 0x80)
              return 1;
          else if ((lead >> 5) == 0x6)
              return 2;
          else if ((lead >> 4) == 0xe)
              return 3;
          else if ((lead >> 3) == 0x1e)
              return 4;
          return 0;
        }
        inline bool isValidNext(uint32_t& cp){
          // Determine the sequence length based on the lead octet
          int32_t length = sequenceLength(cp);
          if (length < 1 && length > 4)
              return false;

          // Now that we have a valid sequence length, get trail octets and calculate the code point
          if (!getSequence(cp, length))
              return false;
          
          // Decoding succeeded, now security checks
          return (isValidCodePoint(cp) && !isOverlongSequence(cp, length));
        }
        
        inline bool getSequence(uint32_t& cp, int32_t length)
        {
            cp = mask8(cp);
            if (length == 1)
                return true;
            uint32_t next = readNext();
            if (next == UNICODE_TERMINATOR)
                return false;
            if (!isTrail(next))
                return false;
            if (length == 2)
            {
                cp = ((cp << 6) & 0x7ff) + (next & 0x3f);
                return true;
            }
            if (length == 3)
                cp = ((cp << 12) & 0xffff) + ((mask8(next) << 6) & 0xfff);
            else
                cp = ((cp << 18) & 0x1fffff) + ((mask8(next) << 12) & 0x3ffff);
            next = readNext();
            if (next == UNICODE_TERMINATOR)
                return false;
            if (!isTrail(next))
                return false;
            if (length == 3)
            {
                cp += next & 0x3f;
                return true;
            }
            cp += (mask8(next) << 6) & 0xfff;
            next = readNext();
            if (next == UNICODE_TERMINATOR)
                return false;
            if (!isTrail(next))
                return false;
            cp += next & 0x3f;
            return true;
        }
        
        virtual uint32_t readNext();
	};
	
  class UTF8DecoderStream : public UTF8Decoder
  {
  public:
      UTF8DecoderStream(ReaderPtr reader);
      virtual ~UTF8DecoderStream();
      
      LUCENE_CLASS(UTF8DecoderStream);
  
  protected:
      ReaderPtr reader;
  
  protected:
      virtual uint32_t readNext();
  };
  
  class UTF16Decoder : public UTF8Base
	{
	public:
    UTF16Decoder(const uint16_t* utf16Begin, const uint16_t* utf16End);
		virtual ~UTF16Decoder();
		
		LUCENE_CLASS(UTF16Decoder);
    
    protected:
        const uint16_t* utf16Begin;
        const uint16_t* utf16End;
    
    public:
        int32_t decode(wchar_t* unicode, int32_t length);
        
        int32_t utf16to16(wchar_t* unicode, int32_t length);
        int32_t utf16to32(wchar_t* unicode, int32_t length);
    
    protected:
        virtual uint32_t readNext();
	};
}

#endif
