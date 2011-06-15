/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermRangeFilter.h"
#include "TermRangeQuery.h"
#include "VariantUtils.h"

namespace Lucene
{
    TermRangeFilter::TermRangeFilter(const String& fieldName, StringValue lowerTerm, StringValue upperTerm, bool includeLower, 
                                     bool includeUpper, CollatorPtr collator) : 
                                     MultiTermQueryWrapperFilter(newLucene<TermRangeQuery>(fieldName, lowerTerm, upperTerm, 
                                     includeLower, includeUpper, collator))
    {
    }
    
    TermRangeFilter::~TermRangeFilter()
    {
    }
    
    TermRangeFilterPtr TermRangeFilter::Less(const String& fieldName, StringValue upperTerm)
    {
        return newLucene<TermRangeFilter>(fieldName, VariantUtils::null(), upperTerm, false, true);
    }
    
    TermRangeFilterPtr TermRangeFilter::More(const String& fieldName, StringValue lowerTerm)
    {
        return newLucene<TermRangeFilter>(fieldName, lowerTerm, VariantUtils::null(), true, false);
    }
    
    String TermRangeFilter::getField()
    {
        return boost::static_pointer_cast<TermRangeQuery>(query)->getField();
    }
    
    String TermRangeFilter::getLowerTerm()
    {
        return boost::static_pointer_cast<TermRangeQuery>(query)->getLowerTerm();
    }
    
    String TermRangeFilter::getUpperTerm()
    {
        return boost::static_pointer_cast<TermRangeQuery>(query)->getUpperTerm();
    }
    
    bool TermRangeFilter::includesLower()
    {
        return boost::static_pointer_cast<TermRangeQuery>(query)->includesLower();
    }
    
    bool TermRangeFilter::includesUpper()
    {
        return boost::static_pointer_cast<TermRangeQuery>(query)->includesUpper();
    }
    
    CollatorPtr TermRangeFilter::getCollator()
    {
        return boost::static_pointer_cast<TermRangeQuery>(query)->getCollator();
    }
}
