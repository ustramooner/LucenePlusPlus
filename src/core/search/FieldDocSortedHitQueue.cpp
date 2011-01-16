/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "VariantUtils.h"
#include "FieldDocSortedHitQueue.h"
#include "FieldDoc.h"
#include "SortField.h"
#include "Collator.h"

namespace Lucene
{
    FieldDocSortedHitQueue::FieldDocSortedHitQueue(int32_t size) : PriorityQueue<FieldDocPtr>(size)
    {
    }
    
    FieldDocSortedHitQueue::~FieldDocSortedHitQueue()
    {
    }
    
    void FieldDocSortedHitQueue::setFields(Collection<SortFieldPtr> fields)
    {
        this->fields = fields;
        this->collators = hasCollators(fields);	
    }
    
    Collection<SortFieldPtr> FieldDocSortedHitQueue::getFields()
    {
        return fields;
    }
    
    Collection<CollatorPtr> FieldDocSortedHitQueue::hasCollators(Collection<SortFieldPtr> fields)
    {
        if (!fields)
            return Collection<CollatorPtr>();
        Collection<CollatorPtr> ret(Collection<CollatorPtr>::newInstance(fields.size()));
        for (int32_t i = 0; i < fields.size(); ++i)
        {
            localePtr locale(fields[i]->getLocale());
            if (locale)
                ret[i] = newInstance<Collator>(*locale);
        }		
        return ret;
    }
    
    bool FieldDocSortedHitQueue::lessThan(const FieldDocPtr& first, const FieldDocPtr& second)
    {
        int32_t n = fields.size();
        int32_t c = 0;
        for (int32_t i = 0; i < n && c == 0; ++i)
        {
            int32_t type = fields[i]->getType();
            if (type == SortField::STRING)
            {
                String s1(VariantUtils::get<String>(first->fields[i]));
                String s2(VariantUtils::get<String>(second->fields[i]));
                if (!fields[i]->getLocale())
                    c = s1.compare(s2);
                else
                    c = collators[i]->compare(s1, s2);
            }
            else
            {
                c = VariantUtils::compareTo(first->fields[i], second->fields[i]);
                if (type == SortField::SCORE)
                    c = -c;
            }
            
            // reverse sort
            if (fields[i]->getReverse())
                c = -c;
        }
        
        // avoid random sort order that could lead to duplicates
        if (c == 0)
            return (first->doc > second->doc);
        
        return (c > 0);
    }
}
