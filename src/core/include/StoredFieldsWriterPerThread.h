/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LuceneObject.h"

namespace Lucene
{
	class StoredFieldsWriterPerThread : public LuceneObject
	{
	public:
		StoredFieldsWriterPerThread(DocStatePtr docState, StoredFieldsWriterPtr storedFieldsWriter);
		virtual ~StoredFieldsWriterPerThread();
		
		LUCENE_CLASS(StoredFieldsWriterPerThread);
				
	public:
		FieldsWriterPtr localFieldsWriter;
		StoredFieldsWriterWeakPtr _storedFieldsWriter;
		DocStatePtr docState;

		StoredFieldsWriterPerDocPtr doc;
		
	public:
		void startDocument();
		void addField(FieldablePtr field, FieldInfoPtr fieldInfo);
		DocWriterPtr finishDocument();
		void abort();
	};
}
