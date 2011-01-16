/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/algorithm/string.hpp>
#include "PrefixTermEnum.h"
#include "IndexReader.h"
#include "Term.h"

namespace Lucene
{
    PrefixTermEnum::PrefixTermEnum(IndexReaderPtr reader, TermPtr prefix)
    {
        this->_endEnum = false;
        this->prefix = prefix;
        
        setEnum(reader->terms(newLucene<Term>(prefix->field(), prefix->text())));
    }
    
    PrefixTermEnum::~PrefixTermEnum()
    {
    }
    
    double PrefixTermEnum::difference()
    {
        return 1.0;
    }
    
    bool PrefixTermEnum::endEnum()
    {
        return _endEnum;
    }
    
    TermPtr PrefixTermEnum::getPrefixTerm()
    {
        return prefix;
    }
    
    bool PrefixTermEnum::termCompare(TermPtr term)
    {
        if (term->field() == prefix->field() && boost::starts_with(term->text(), prefix->text()))
            return true;
        _endEnum = true;
        return false;
    }
}
