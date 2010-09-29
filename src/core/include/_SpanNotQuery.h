#pragma once
#include "SpanNotQuery.h"

namespace Lucene
{

	class NotSpans : public Spans
	{
	public:
		NotSpans(SpanNotQueryPtr query, SpansPtr includeSpans, SpansPtr excludeSpans);
		virtual ~NotSpans();
		
		LUCENE_CLASS(NotSpans);
	
	protected:
		SpanNotQueryPtr query;
		SpansPtr includeSpans;
		bool moreInclude;
		SpansPtr excludeSpans;
		bool moreExclude;
	
	public:
		virtual bool next();
		virtual bool skipTo(int32_t target);
		virtual int32_t doc();
		virtual int32_t start();
		virtual int32_t end();
		virtual Collection<ByteArray> getPayload();
		virtual bool isPayloadAvailable();
		virtual String toString();
	};
}

