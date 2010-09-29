#pragma once
#include "SpanOrQuery.h"

namespace Lucene
{

	class SpanQueue : public PriorityQueue<SpansPtr>
	{
	public:
		SpanQueue(int32_t size);
		virtual ~SpanQueue();
		
		LUCENE_CLASS(SpanQueue);
	
	protected:
		virtual bool lessThan(const SpansPtr& first, const SpansPtr& second);
	};
	
	class OrSpans : public Spans
	{
	public:
		OrSpans(SpanOrQueryPtr query, IndexReaderPtr reader);
		virtual ~OrSpans();
		
		LUCENE_CLASS(OrSpans);
	
	protected:
		SpanOrQueryPtr query;
		IndexReaderPtr reader;
		SpanQueuePtr queue;
	
	public:
		virtual bool next();
		virtual bool skipTo(int32_t target);
		virtual int32_t doc();
		virtual int32_t start();
		virtual int32_t end();
		virtual Collection<ByteArray> getPayload();
		virtual bool isPayloadAvailable();
		virtual String toString();
	
	protected:
		bool initSpanQueue(int32_t target);
		SpansPtr top();
	};
}

