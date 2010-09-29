#pragma once
#include "SimpleFSDirectory.h"

namespace Lucene
{

	class InputFile : public LuceneObject
	{
	public:
		InputFile(const String& path);
		virtual ~InputFile();
		
		LUCENE_CLASS(InputFile);
				
	public:
		static const int32_t FILE_EOF;
		static const int32_t FILE_ERROR;
	
	protected:
		ifstreamPtr file;
		int64_t position;
		int64_t length;
	
	public:
		void setPosition(int64_t position);
		int64_t getPosition();
		int64_t getLength();
		int32_t read(uint8_t* b, int32_t offset, int32_t length);
		void close();
		bool isValid();
	};
	
	class OutputFile : public LuceneObject
	{
	public:
		OutputFile(const String& path);
		virtual ~OutputFile();
		
		LUCENE_CLASS(OutputFile);
				
	protected:
		ofstreamPtr file;
		String path;
	
	public:
		bool write(const uint8_t* b, int32_t offset, int32_t length);
		void close();
		void setPosition(int64_t position);
		int64_t getLength();
		void setLength(int64_t length);
		void flush();
		bool isValid();
	};
	
	class SimpleFSIndexInput : public BufferedIndexInput
	{
	public:
		SimpleFSIndexInput();
		SimpleFSIndexInput(const String& path, int32_t bufferSize, int32_t chunkSize);
		virtual ~SimpleFSIndexInput();
		
		LUCENE_CLASS(SimpleFSIndexInput);
				
	protected:
		String path;
		InputFilePtr file;
		bool isClone;
		int32_t chunkSize;
	
	protected:
		virtual void readInternal(uint8_t* b, int32_t offset, int32_t length);
		virtual void seekInternal(int64_t pos);
		
	public:
		virtual int64_t length();
		virtual void close();
		
		/// Method used for testing.
		bool isValid();
		
		/// Returns a clone of this stream.
		virtual LuceneObjectPtr clone(LuceneObjectPtr other = LuceneObjectPtr());
	};
		
	class SimpleFSIndexOutput : public BufferedIndexOutput
	{
	public:
		SimpleFSIndexOutput(const String& path);
		virtual ~SimpleFSIndexOutput();
		
		LUCENE_CLASS(SimpleFSIndexOutput);
				
	protected:
		OutputFilePtr file;
		bool isOpen;
	
	public:
		virtual void flushBuffer(const uint8_t* b, int32_t offset, int32_t length);
		virtual void close();
		virtual void seek(int64_t pos);
		virtual int64_t length();
		virtual void setLength(int64_t length);
	};		

}
