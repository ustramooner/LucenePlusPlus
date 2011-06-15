/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MultiTermQueryWrapperFilter.h"
#include "MultiTermQuery.h"
#include "IndexReader.h"
#include "TermEnum.h"
#include "TermDocs.h"
#include "Term.h"
#include "FilteredTermEnum.h"
#include "DocIdSet.h"
#include "OpenBitSet.h"
#include "MiscUtils.h"

namespace Lucene
{
    MultiTermQueryWrapperFilter::MultiTermQueryWrapperFilter(MultiTermQueryPtr query)
    {
        this->query = query;
    }
    
    MultiTermQueryWrapperFilter::~MultiTermQueryWrapperFilter()
    {
    }
    
    String MultiTermQueryWrapperFilter::toString()
    {
        // query->toString should be ok for the filter, too, if the query boost is 1.0
        return query->toString();
    }
    
    bool MultiTermQueryWrapperFilter::equals(LuceneObjectPtr other)
    {
        if (Filter::equals(other))
            return true;
        if (!other)
            return false;
        if (!MiscUtils::equalTypes(shared_from_this(), other))
            return false;
        MultiTermQueryWrapperFilterPtr otherMultiTermQueryWrapperFilter(boost::dynamic_pointer_cast<MultiTermQueryWrapperFilter>(other));
        if (otherMultiTermQueryWrapperFilter)
            return query->equals(otherMultiTermQueryWrapperFilter->query);
        return false;
    }
    
    int32_t MultiTermQueryWrapperFilter::hashCode()
    {
        return query->hashCode();
    }
    
    int32_t MultiTermQueryWrapperFilter::getTotalNumberOfTerms()
    {
        return query->getTotalNumberOfTerms();
    }
    
    void MultiTermQueryWrapperFilter::clearTotalNumberOfTerms()
    {
        query->clearTotalNumberOfTerms();
    }
    
    DocIdSetPtr MultiTermQueryWrapperFilter::getDocIdSet(IndexReaderPtr reader)
    {
        TermEnumPtr enumerator(query->getEnum(reader));
        OpenBitSetPtr bitSet;
        LuceneException finally;
        try
        {
            // if current term in enum is null, the enum is empty -> shortcut
            if (!enumerator->term())
                return DocIdSet::EMPTY_DOCIDSET();
            // else fill into a OpenBitSet
            bitSet = newLucene<OpenBitSet>(reader->maxDoc());
            Collection<int32_t> docs(Collection<int32_t>::newInstance(32));
            Collection<int32_t> freqs(Collection<int32_t>::newInstance(32));
            TermDocsPtr termDocs(reader->termDocs());
            try
            {
                int32_t termCount = 0;
                do
                {
                    TermPtr term(enumerator->term());
                    if (!term)
                        break;
                    ++termCount;
                    termDocs->seek(term);
                    while (true)
                    {
                        int32_t count = termDocs->read(docs, freqs);
                        if (count != 0)
                        {
                            for (int32_t i = 0; i < count; ++i)
                                bitSet->set(docs[i]);
                        }
                        else
                            break;
                    }
                }
                while (enumerator->next());
                
                query->incTotalNumberOfTerms(termCount);
            }
            catch (LuceneException& e)
            {
                finally = e;
            }
            termDocs->close();
        }
        catch (LuceneException& e)
        {
            finally = e;
        }
        enumerator->close();
        finally.throwException();
        return bitSet;
    }
}
