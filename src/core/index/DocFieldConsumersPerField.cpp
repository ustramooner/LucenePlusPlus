/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DocFieldConsumersPerField.h"

namespace Lucene
{
    DocFieldConsumersPerField::DocFieldConsumersPerField(DocFieldConsumersPerThreadPtr perThread, DocFieldConsumerPerFieldPtr one, DocFieldConsumerPerFieldPtr two)
    {
        this->_perThread = perThread;
        this->one = one;
        this->two = two;
    }
    
    DocFieldConsumersPerField::~DocFieldConsumersPerField()
    {
    }
    
    void DocFieldConsumersPerField::processFields(Collection<FieldablePtr> fields, int32_t count)
    {
        one->processFields(fields, count);
        two->processFields(fields, count);
    }
    
    void DocFieldConsumersPerField::abort()
    {
        LuceneException finally;
        try
        {
            one->abort();
        }
        catch (LuceneException& e)
        {
            finally = e;
        }
        try
        {
            two->abort();
        }
        catch (LuceneException& e)
        {
            finally = e;
        }
        finally.throwException();
    }
}
