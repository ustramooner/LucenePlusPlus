/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DocFieldConsumerPerThread.h"
#include "AttributeSource.h"

namespace Lucene
{
	/// This is a DocFieldConsumer that inverts each field, separately, from a Document, and accepts a
	/// InvertedTermsConsumer to process those terms.
	class DocInverterPerThread : public DocFieldConsumerPerThread
	{
	public:
		DocInverterPerThread(DocFieldProcessorPerThreadPtr docFieldProcessorPerThread, DocInverterPtr docInverter);
		virtual ~DocInverterPerThread();
		
		LUCENE_CLASS(DocInverterPerThread);
			
	public:
		DocInverterWeakPtr _docInverter;
		InvertedDocConsumerPerThreadPtr consumer;
		InvertedDocEndConsumerPerThreadPtr endConsumer;
		SingleTokenAttributeSourcePtr singleToken;
		
		DocStatePtr docState;
		FieldInvertStatePtr fieldState;
		
		/// Used to read a string value for a field
		ReusableStringReaderPtr stringReader;
	
	public:
		virtual void initialize();
		virtual void startDocument();
		virtual DocWriterPtr finishDocument();
		virtual void abort();
		virtual DocFieldConsumerPerFieldPtr addField(FieldInfoPtr fi);		
	};
	
	class SingleTokenAttributeSource : public AttributeSource
	{
	public:
		SingleTokenAttributeSource();
		virtual ~SingleTokenAttributeSource();
		
		LUCENE_CLASS(SingleTokenAttributeSource);
				
	public:
		TermAttributePtr termAttribute;
		OffsetAttributePtr offsetAttribute;
	
	public:
		void reinit(const String& stringValue, int32_t startOffset, int32_t endOffset);
	};
}
