#pragma once
#include "SpanFirstQuery.h"

namespace Lucene
{

	class FirstSpans : public Spans
	{
	public:
		FirstSpans(SpanFirstQueryPtr query, SpansPtr spans);
		virtual ~FirstSpans();
		
		LUCENE_CLASS(FirstSpans);
	
	protected:
		SpanFirstQueryPtr query;
		SpansPtr spans;
	
	public:
		virtual bool next();
		virtual bool skipTo(int32_t target);
		virtual int32_t doc();
		virtual int32_t start();
		virtual int32_t end();
		virtual Collection<ByteArray> getPayload();
		virtual bool isPayloadAvailable();
	};
}

