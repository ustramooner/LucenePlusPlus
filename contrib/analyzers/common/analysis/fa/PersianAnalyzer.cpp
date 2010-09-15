/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PersianAnalyzer.h"
#include "ArabicLetterTokenizer.h"
#include "ArabicNormalizationFilter.h"
#include "LowerCaseFilter.h"
#include "StopFilter.h"
#include "PersianNormalizationFilter.h"
#include "StringUtils.h"
#include "MiscUtils.h"

namespace Lucene
{
    /// Default Persian stopwords in UTF-8 format.
    ///
    /// Generated from http://members.unine.ch/jacques.savoy/clef/index.html
    /// The stopword list is BSD-Licensed.
    const uint8_t PersianAnalyzer::DEFAULT_STOPWORD_FILE[] = 
    {
        0xd8, 0xa7, 0xd9, 0x86, 0xd8, 0xa7, 0xd9, 0x86, 0x0a, 0xd9, 0x86, 0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 
        0xb4, 0xd8, 0xaa, 0xd9, 0x87, 0x0a, 0xd8, 0xb3, 0xd8, 0xb1, 0xd8, 0xa7, 0xd8, 0xb3, 0xd8, 0xb1, 
        0x0a, 0xd8, 0xae, 0xd9, 0x8a, 0xd8, 0xa7, 0xd9, 0x87, 0x0a, 0xd8, 0xa7, 0xd9, 0x8a, 0xd8, 0xb4, 
        0xd8, 0xa7, 0xd9, 0x86, 0x0a, 0xd9, 0x88, 0xd9, 0x8a, 0x0a, 0xd8, 0xaa, 0xd8, 0xa7, 0xd9, 0x83, 
        0xd9, 0x86, 0xd9, 0x88, 0xd9, 0x86, 0x0a, 0xd8, 0xa8, 0xd9, 0x8a, 0xd8, 0xb4, 0xd8, 0xaa, 0xd8, 
        0xb1, 0xd9, 0x8a, 0x0a, 0xd8, 0xaf, 0xd9, 0x88, 0xd9, 0x85, 0x0a, 0xd9, 0xbe, 0xd8, 0xb3, 0x0a, 
        0xd9, 0x86, 0xd8, 0xa7, 0xd8, 0xb4, 0xd9, 0x8a, 0x0a, 0xd9, 0x88, 0xda, 0xaf, 0xd9, 0x88, 0x0a, 
        0xd9, 0x8a, 0xd8, 0xa7, 0x0a, 0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 0xb4, 0xd8, 0xaa, 0xd9, 0x86, 0xd8, 
        0xaf, 0x0a, 0xd8, 0xb3, 0xd9, 0xbe, 0xd8, 0xb3, 0x0a, 0xd9, 0x87, 0xd9, 0x86, 0xda, 0xaf, 0xd8, 
        0xa7, 0xd9, 0x85, 0x0a, 0xd9, 0x87, 0xd8, 0xb1, 0xda, 0xaf, 0xd8, 0xb2, 0x0a, 0xd9, 0xbe, 0xd9, 
        0x86, 0xd8, 0xac, 0x0a, 0xd9, 0x86, 0xd8, 0xb4, 0xd8, 0xa7, 0xd9, 0x86, 0x0a, 0xd8, 0xa7, 0xd9, 
        0x85, 0xd8, 0xb3, 0xd8, 0xa7, 0xd9, 0x84, 0x0a, 0xd8, 0xaf, 0xd9, 0x8a, 0xda, 0xaf, 0xd8, 0xb1, 
        0x0a, 0xda, 0xaf, 0xd8, 0xb1, 0xd9, 0x88, 0xd9, 0x87, 0xd9, 0x8a, 0x0a, 0xd8, 0xb4, 0xd8, 0xaf, 
        0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xda, 0x86, 0xd8, 0xb7, 0xd9, 0x88, 0xd8, 0xb1, 0x0a, 0xd8, 0xaf, 
        0xd9, 0x87, 0x0a, 0xd9, 0x88, 0x0a, 0xd8, 0xaf, 0xd9, 0x88, 0x0a, 0xd9, 0x86, 0xd8, 0xae, 0xd8, 
        0xb3, 0xd8, 0xaa, 0xd9, 0x8a, 0xd9, 0x86, 0x0a, 0xd9, 0x88, 0xd9, 0x84, 0xd9, 0x8a, 0x0a, 0xda, 
        0x86, 0xd8, 0xb1, 0xd8, 0xa7, 0x0a, 0xda, 0x86, 0xd9, 0x87, 0x0a, 0xd9, 0x88, 0xd8, 0xb3, 0xd8, 
        0xb7, 0x0a, 0xd9, 0x87, 0x0a, 0xd9, 0x83, 0xd8, 0xaf, 0xd8, 0xa7, 0xd9, 0x85, 0x0a, 0xd9, 0x82, 
        0xd8, 0xa7, 0xd8, 0xa8, 0xd9, 0x84, 0x0a, 0xd9, 0x8a, 0xd9, 0x83, 0x0a, 0xd8, 0xb1, 0xd9, 0x81, 
        0xd8, 0xaa, 0x0a, 0xd9, 0x87, 0xd9, 0x81, 0xd8, 0xaa, 0x0a, 0xd9, 0x87, 0xd9, 0x85, 0xda, 0x86, 
        0xd9, 0x86, 0xd9, 0x8a, 0xd9, 0x86, 0x0a, 0xd8, 0xaf, 0xd8, 0xb1, 0x0a, 0xd9, 0x87, 0xd8, 0xb2, 
        0xd8, 0xa7, 0xd8, 0xb1, 0x0a, 0xd8, 0xa8, 0xd9, 0x84, 0xd9, 0x87, 0x0a, 0xd8, 0xa8, 0xd9, 0x84, 
        0xd9, 0x8a, 0x0a, 0xd8, 0xb4, 0xd8, 0xa7, 0xd9, 0x8a, 0xd8, 0xaf, 0x0a, 0xd8, 0xa7, 0xd9, 0x85, 
        0xd8, 0xa7, 0x0a, 0xd8, 0xb4, 0xd9, 0x86, 0xd8, 0xa7, 0xd8, 0xb3, 0xd9, 0x8a, 0x0a, 0xda, 0xaf, 
        0xd8, 0xb1, 0xd9, 0x81, 0xd8, 0xaa, 0xd9, 0x87, 0x0a, 0xd8, 0xaf, 0xd9, 0x87, 0xd8, 0xaf, 0x0a, 
        0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 0xb4, 0xd8, 0xaa, 0xd9, 0x87, 0x0a, 0xd8, 0xaf, 0xd8, 0xa7, 0xd9, 
        0x86, 0xd8, 0xb3, 0xd8, 0xaa, 0x0a, 0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 0xb4, 0xd8, 0xaa, 0xd9, 0x86, 
        0x0a, 0xd8, 0xae, 0xd9, 0x88, 0xd8, 0xa7, 0xd9, 0x87, 0xd9, 0x8a, 0xd9, 0x85, 0x0a, 0xd9, 0x85, 
        0xd9, 0x8a, 0xd9, 0x84, 0xd9, 0x8a, 0xd8, 0xa7, 0xd8, 0xb1, 0xd8, 0xaf, 0x0a, 0xd9, 0x88, 0xd9, 
        0x82, 0xd8, 0xaa, 0xd9, 0x8a, 0xd9, 0x83, 0xd9, 0x87, 0x0a, 0xd8, 0xa7, 0xd9, 0x85, 0xd8, 0xaf, 
        0x0a, 0xd8, 0xae, 0xd9, 0x88, 0xd8, 0xa7, 0xd9, 0x87, 0xd8, 0xaf, 0x0a, 0xd8, 0xac, 0xd8, 0xb2, 
        0x0a, 0xd8, 0xa7, 0xd9, 0x88, 0xd8, 0xb1, 0xd8, 0xaf, 0xd9, 0x87, 0x0a, 0xd8, 0xb4, 0xd8, 0xaf, 
        0xd9, 0x87, 0x0a, 0xd8, 0xa8, 0xd9, 0x84, 0xd9, 0x83, 0xd9, 0x87, 0x0a, 0xd8, 0xae, 0xd8, 0xaf, 
        0xd9, 0x85, 0xd8, 0xa7, 0xd8, 0xaa, 0x0a, 0xd8, 0xb4, 0xd8, 0xaf, 0xd9, 0x86, 0x0a, 0xd8, 0xa8, 
        0xd8, 0xb1, 0xd8, 0xae, 0xd9, 0x8a, 0x0a, 0xd9, 0x86, 0xd8, 0xa8, 0xd9, 0x88, 0xd8, 0xaf, 0x0a, 
        0xd8, 0xa8, 0xd8, 0xb3, 0xd9, 0x8a, 0xd8, 0xa7, 0xd8, 0xb1, 0xd9, 0x8a, 0x0a, 0xd8, 0xac, 0xd9, 
        0x84, 0xd9, 0x88, 0xda, 0xaf, 0xd9, 0x8a, 0xd8, 0xb1, 0xd9, 0x8a, 0x0a, 0xd8, 0xad, 0xd9, 0x82, 
        0x0a, 0xd9, 0x83, 0xd8, 0xb1, 0xd8, 0xaf, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xd9, 0x86, 0xd9, 0x88, 
        0xd8, 0xb9, 0xd9, 0x8a, 0x0a, 0xd8, 0xa8, 0xd8, 0xb9, 0xd8, 0xb1, 0xd9, 0x8a, 0x0a, 0xd9, 0x86, 
        0xd9, 0x83, 0xd8, 0xb1, 0xd8, 0xaf, 0xd9, 0x87, 0x0a, 0xd9, 0x86, 0xd8, 0xb8, 0xd9, 0x8a, 0xd8, 
        0xb1, 0x0a, 0xd9, 0x86, 0xd8, 0xa8, 0xd8, 0xa7, 0xd9, 0x8a, 0xd8, 0xaf, 0x0a, 0xd8, 0xa8, 0xd9, 
        0x88, 0xd8, 0xaf, 0xd9, 0x87, 0x0a, 0xd8, 0xa8, 0xd9, 0x88, 0xd8, 0xaf, 0xd9, 0x86, 0x0a, 0xd8, 
        0xaf, 0xd8, 0xa7, 0xd8, 0xaf, 0x0a, 0xd8, 0xa7, 0xd9, 0x88, 0xd8, 0xb1, 0xd8, 0xaf, 0x0a, 0xd9, 
        0x87, 0xd8, 0xb3, 0xd8, 0xaa, 0x0a, 0xd8, 0xac, 0xd8, 0xa7, 0xd9, 0x8a, 0xd9, 0x8a, 0x0a, 0xd8, 
        0xb4, 0xd9, 0x88, 0xd8, 0xaf, 0x0a, 0xd8, 0xaf, 0xd9, 0x86, 0xd8, 0xa8, 0xd8, 0xa7, 0xd9, 0x84, 
        0x0a, 0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 0xaf, 0xd9, 0x87, 0x0a, 0xd8, 0xa8, 0xd8, 0xa7, 0xd9, 0x8a, 
        0xd8, 0xaf, 0x0a, 0xd8, 0xb3, 0xd8, 0xa7, 0xd8, 0xa8, 0xd9, 0x82, 0x0a, 0xd9, 0x87, 0xd9, 0x8a, 
        0xda, 0x86, 0x0a, 0xd9, 0x87, 0xd9, 0x85, 0xd8, 0xa7, 0xd9, 0x86, 0x0a, 0xd8, 0xa7, 0xd9, 0x86, 
        0xd8, 0xac, 0xd8, 0xa7, 0x0a, 0xd9, 0x83, 0xd9, 0x85, 0xd8, 0xaa, 0xd8, 0xb1, 0x0a, 0xd9, 0x83, 
        0xd8, 0xac, 0xd8, 0xa7, 0xd8, 0xb3, 0xd8, 0xaa, 0x0a, 0xda, 0xaf, 0xd8, 0xb1, 0xd8, 0xaf, 0xd8, 
        0xaf, 0x0a, 0xd9, 0x83, 0xd8, 0xb3, 0xd9, 0x8a, 0x0a, 0xd8, 0xaa, 0xd8, 0xb1, 0x0a, 0xd9, 0x85, 
        0xd8, 0xb1, 0xd8, 0xaf, 0xd9, 0x85, 0x0a, 0xd8, 0xaa, 0xd8, 0xa7, 0xd9, 0x86, 0x0a, 0xd8, 0xaf, 
        0xd8, 0xa7, 0xd8, 0xaf, 0xd9, 0x86, 0x0a, 0xd8, 0xa8, 0xd9, 0x88, 0xd8, 0xaf, 0xd9, 0x86, 0xd8, 
        0xaf, 0x0a, 0xd8, 0xb3, 0xd8, 0xb1, 0xd9, 0x8a, 0x0a, 0xd8, 0xac, 0xd8, 0xaf, 0xd8, 0xa7, 0x0a, 
        0xd9, 0x86, 0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 0xb1, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xd9, 0x85, 0xda, 
        0xaf, 0xd8, 0xb1, 0x0a, 0xd9, 0x8a, 0xd9, 0x83, 0xd8, 0xaf, 0xd9, 0x8a, 0xda, 0xaf, 0xd8, 0xb1, 
        0x0a, 0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 0xb1, 0xd8, 0xaf, 0x0a, 0xd8, 0xaf, 0xd9, 0x87, 0xd9, 0x86, 
        0xd8, 0xaf, 0x0a, 0xd8, 0xa8, 0xd9, 0x86, 0xd8, 0xa7, 0xd8, 0xa8, 0xd8, 0xb1, 0xd8, 0xa7, 0xd9, 
        0x8a, 0xd9, 0x86, 0x0a, 0xd9, 0x87, 0xd9, 0x86, 0xda, 0xaf, 0xd8, 0xa7, 0xd9, 0x85, 0xd9, 0x8a, 
        0x0a, 0xd8, 0xb3, 0xd9, 0x85, 0xd8, 0xaa, 0x0a, 0xd8, 0xac, 0xd8, 0xa7, 0x0a, 0xd8, 0xa7, 0xd9, 
        0x86, 0xda, 0x86, 0xd9, 0x87, 0x0a, 0xd8, 0xae, 0xd9, 0x88, 0xd8, 0xaf, 0x0a, 0xd8, 0xaf, 0xd8, 
        0xa7, 0xd8, 0xaf, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xd8, 0xb2, 0xd9, 0x8a, 0xd8, 0xa7, 0xd8, 0xaf, 
        0x0a, 0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 0xb1, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xd8, 0xa7, 0xd8, 0xab, 
        0xd8, 0xb1, 0x0a, 0xd8, 0xa8, 0xd8, 0xaf, 0xd9, 0x88, 0xd9, 0x86, 0x0a, 0xd8, 0xa8, 0xd9, 0x87, 
        0xd8, 0xaa, 0xd8, 0xb1, 0xd9, 0x8a, 0xd9, 0x86, 0x0a, 0xd8, 0xa8, 0xd9, 0x8a, 0xd8, 0xb4, 0xd8, 
        0xaa, 0xd8, 0xb1, 0x0a, 0xd8, 0xa7, 0xd9, 0x84, 0xd8, 0xa8, 0xd8, 0xaa, 0xd9, 0x87, 0x0a, 0xd8, 
        0xa8, 0xd9, 0x87, 0x0a, 0xd8, 0xa8, 0xd8, 0xb1, 0xd8, 0xa7, 0xd8, 0xb3, 0xd8, 0xa7, 0xd8, 0xb3, 
        0x0a, 0xd8, 0xa8, 0xd9, 0x8a, 0xd8, 0xb1, 0xd9, 0x88, 0xd9, 0x86, 0x0a, 0xd9, 0x83, 0xd8, 0xb1, 
        0xd8, 0xaf, 0x0a, 0xd8, 0xa8, 0xd8, 0xb9, 0xd8, 0xb6, 0xd9, 0x8a, 0x0a, 0xda, 0xaf, 0xd8, 0xb1, 
        0xd9, 0x81, 0xd8, 0xaa, 0x0a, 0xd8, 0xaa, 0xd9, 0x88, 0xd9, 0x8a, 0x0a, 0xd8, 0xa7, 0xd9, 0x8a, 
        0x0a, 0xd9, 0x85, 0xd9, 0x8a, 0xd9, 0x84, 0xd9, 0x8a, 0xd9, 0x88, 0xd9, 0x86, 0x0a, 0xd8, 0xa7, 
        0xd9, 0x88, 0x0a, 0xd8, 0xac, 0xd8, 0xb1, 0xd9, 0x8a, 0xd8, 0xa7, 0xd9, 0x86, 0x0a, 0xd8, 0xaa, 
        0xd9, 0x88, 0xd9, 0x84, 0x0a, 0xd8, 0xa8, 0xd8, 0xb1, 0x0a, 0xd9, 0x85, 0xd8, 0xa7, 0xd9, 0x86, 
        0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xd8, 0xa8, 0xd8, 0xb1, 0xd8, 0xa7, 0xd8, 0xa8, 0xd8, 0xb1, 0x0a, 
        0xd8, 0xa8, 0xd8, 0xa7, 0xd8, 0xb4, 0xd9, 0x8a, 0xd9, 0x85, 0x0a, 0xd9, 0x85, 0xd8, 0xaf, 0xd8, 
        0xaa, 0xd9, 0x8a, 0x0a, 0xda, 0xaf, 0xd9, 0x88, 0xd9, 0x8a, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xd8, 
        0xa7, 0xd9, 0x83, 0xd9, 0x86, 0xd9, 0x88, 0xd9, 0x86, 0x0a, 0xd8, 0xaa, 0xd8, 0xa7, 0x0a, 0xd8, 
        0xaa, 0xd9, 0x86, 0xd9, 0x87, 0xd8, 0xa7, 0x0a, 0xd8, 0xac, 0xd8, 0xaf, 0xd9, 0x8a, 0xd8, 0xaf, 
        0x0a, 0xda, 0x86, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xd8, 0xa8, 0xd9, 0x8a, 0x0a, 0xd9, 0x86, 0xd8, 
        0xb4, 0xd8, 0xaf, 0xd9, 0x87, 0x0a, 0xd9, 0x83, 0xd8, 0xb1, 0xd8, 0xaf, 0xd9, 0x86, 0x0a, 0xd9, 
        0x83, 0xd8, 0xb1, 0xd8, 0xaf, 0xd9, 0x85, 0x0a, 0xda, 0xaf, 0xd9, 0x88, 0xd9, 0x8a, 0xd8, 0xaf, 
        0x0a, 0xd9, 0x83, 0xd8, 0xb1, 0xd8, 0xaf, 0xd9, 0x87, 0x0a, 0xd9, 0x83, 0xd9, 0x86, 0xd9, 0x8a, 
        0xd9, 0x85, 0x0a, 0xd9, 0x86, 0xd9, 0x85, 0xd9, 0x8a, 0x0a, 0xd9, 0x86, 0xd8, 0xb2, 0xd8, 0xaf, 
        0x0a, 0xd8, 0xb1, 0xd9, 0x88, 0xd9, 0x8a, 0x0a, 0xd9, 0x82, 0xd8, 0xb5, 0xd8, 0xaf, 0x0a, 0xd9, 
        0x81, 0xd9, 0x82, 0xd8, 0xb7, 0x0a, 0xd8, 0xa8, 0xd8, 0xa7, 0xd9, 0x84, 0xd8, 0xa7, 0xd9, 0x8a, 
        0x0a, 0xd8, 0xaf, 0xd9, 0x8a, 0xda, 0xaf, 0xd8, 0xb1, 0xd8, 0xa7, 0xd9, 0x86, 0x0a, 0xd8, 0xa7, 
        0xd9, 0x8a, 0xd9, 0x86, 0x0a, 0xd8, 0xaf, 0xd9, 0x8a, 0xd8, 0xb1, 0xd9, 0x88, 0xd8, 0xb2, 0x0a, 
        0xd8, 0xaa, 0xd9, 0x88, 0xd8, 0xb3, 0xd8, 0xb7, 0x0a, 0xd8, 0xb3, 0xd9, 0x88, 0xd9, 0x85, 0x0a, 
        0xd8, 0xa7, 0xd9, 0x8a, 0xd9, 0x85, 0x0a, 0xd8, 0xaf, 0xd8, 0xa7, 0xd9, 0x86, 0xd9, 0x86, 0xd8, 
        0xaf, 0x0a, 0xd8, 0xb3, 0xd9, 0x88, 0xd9, 0x8a, 0x0a, 0xd8, 0xa7, 0xd8, 0xb3, 0xd8, 0xaa, 0xd9, 
        0x81, 0xd8, 0xa7, 0xd8, 0xaf, 0xd9, 0x87, 0x0a, 0xd8, 0xb4, 0xd9, 0x85, 0xd8, 0xa7, 0x0a, 0xd9, 
        0x83, 0xd9, 0x86, 0xd8, 0xa7, 0xd8, 0xb1, 0x0a, 0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 0xb1, 0xd9, 0x8a, 
        0xd9, 0x85, 0x0a, 0xd8, 0xb3, 0xd8, 0xa7, 0xd8, 0xae, 0xd8, 0xaa, 0xd9, 0x87, 0x0a, 0xd8, 0xb7, 
        0xd9, 0x88, 0xd8, 0xb1, 0x0a, 0xd8, 0xa7, 0xd9, 0x85, 0xd8, 0xaf, 0xd9, 0x87, 0x0a, 0xd8, 0xb1, 
        0xd9, 0x81, 0xd8, 0xaa, 0xd9, 0x87, 0x0a, 0xd9, 0x86, 0xd8, 0xae, 0xd8, 0xb3, 0xd8, 0xaa, 0x0a, 
        0xd8, 0xa8, 0xd9, 0x8a, 0xd8, 0xb3, 0xd8, 0xaa, 0x0a, 0xd9, 0x86, 0xd8, 0xb2, 0xd8, 0xaf, 0xd9, 
        0x8a, 0xd9, 0x83, 0x0a, 0xd8, 0xb7, 0xd9, 0x8a, 0x0a, 0xd9, 0x83, 0xd9, 0x86, 0xd9, 0x8a, 0xd8, 
        0xaf, 0x0a, 0xd8, 0xa7, 0xd8, 0xb2, 0x0a, 0xd8, 0xa7, 0xd9, 0x86, 0xd9, 0x87, 0xd8, 0xa7, 0x0a, 
        0xd8, 0xaa, 0xd9, 0x85, 0xd8, 0xa7, 0xd9, 0x85, 0xd9, 0x8a, 0x0a, 0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 
        0xb4, 0xd8, 0xaa, 0x0a, 0xd9, 0x8a, 0xd9, 0x83, 0xd9, 0x8a, 0x0a, 0xd8, 0xb7, 0xd8, 0xb1, 0xd9, 
        0x8a, 0xd9, 0x82, 0x0a, 0xd8, 0xa7, 0xd8, 0xb4, 0x0a, 0xda, 0x86, 0xd9, 0x8a, 0xd8, 0xb3, 0xd8, 
        0xaa, 0x0a, 0xd8, 0xb1, 0xd9, 0x88, 0xd8, 0xa8, 0x0a, 0xd9, 0x86, 0xd9, 0x85, 0xd8, 0xa7, 0xd9, 
        0x8a, 0xd8, 0xaf, 0x0a, 0xda, 0xaf, 0xd9, 0x81, 0xd8, 0xaa, 0x0a, 0xda, 0x86, 0xd9, 0x86, 0xd8, 
        0xaf, 0xd9, 0x8a, 0xd9, 0x86, 0x0a, 0xda, 0x86, 0xd9, 0x8a, 0xd8, 0xb2, 0xd9, 0x8a, 0x0a, 0xd8, 
        0xaa, 0xd9, 0x88, 0xd8, 0xa7, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xd8, 0xa7, 0xd9, 0x85, 0x0a, 0xd8, 
        0xa7, 0xd9, 0x8a, 0xd8, 0xa7, 0x0a, 0xd8, 0xa8, 0xd8, 0xa7, 0x0a, 0xd8, 0xa7, 0xd9, 0x86, 0x0a, 
        0xd8, 0xa7, 0xd9, 0x8a, 0xd8, 0xaf, 0x0a, 0xd8, 0xaa, 0xd8, 0xb1, 0xd9, 0x8a, 0xd9, 0x86, 0x0a, 
        0xd8, 0xa7, 0xd9, 0x8a, 0xd9, 0x86, 0xd9, 0x83, 0xd9, 0x87, 0x0a, 0xd8, 0xaf, 0xd9, 0x8a, 0xda, 
        0xaf, 0xd8, 0xb1, 0xd9, 0x8a, 0x0a, 0xd8, 0xb1, 0xd8, 0xa7, 0xd9, 0x87, 0x0a, 0xd9, 0x87, 0xd8, 
        0xa7, 0xd9, 0x8a, 0xd9, 0x8a, 0x0a, 0xd8, 0xa8, 0xd8, 0xb1, 0xd9, 0x88, 0xd8, 0xb2, 0x0a, 0xd9, 
        0x87, 0xd9, 0x85, 0xda, 0x86, 0xd9, 0x86, 0xd8, 0xa7, 0xd9, 0x86, 0x0a, 0xd9, 0xbe, 0xd8, 0xa7, 
        0xd8, 0xb9, 0xd9, 0x8a, 0xd9, 0x86, 0x0a, 0xd9, 0x83, 0xd8, 0xb3, 0x0a, 0xd8, 0xad, 0xd8, 0xaf, 
        0xd9, 0x88, 0xd8, 0xaf, 0x0a, 0xd9, 0x85, 0xd8, 0xae, 0xd8, 0xaa, 0xd9, 0x84, 0xd9, 0x81, 0x0a, 
        0xd9, 0x85, 0xd9, 0x82, 0xd8, 0xa7, 0xd8, 0xa8, 0xd9, 0x84, 0x0a, 0xda, 0x86, 0xd9, 0x8a, 0xd8, 
        0xb2, 0x0a, 0xda, 0xaf, 0xd9, 0x8a, 0xd8, 0xb1, 0xd8, 0xaf, 0x0a, 0xd9, 0x86, 0xd8, 0xaf, 0xd8, 
        0xa7, 0xd8, 0xb1, 0xd8, 0xaf, 0x0a, 0xd8, 0xb6, 0xd8, 0xaf, 0x0a, 0xd9, 0x87, 0xd9, 0x85, 0xda, 
        0x86, 0xd9, 0x88, 0xd9, 0x86, 0x0a, 0xd8, 0xb3, 0xd8, 0xa7, 0xd8, 0xb2, 0xd9, 0x8a, 0x0a, 0xd8, 
        0xb4, 0xd8, 0xa7, 0xd9, 0x86, 0x0a, 0xd9, 0x85, 0xd9, 0x88, 0xd8, 0xb1, 0xd8, 0xaf, 0x0a, 0xd8, 
        0xa8, 0xd8, 0xa7, 0xd8, 0xb1, 0xd9, 0x87, 0x0a, 0xd9, 0x85, 0xd8, 0xb1, 0xd8, 0xb3, 0xd9, 0x8a, 
        0x0a, 0xd8, 0xae, 0xd9, 0x88, 0xd9, 0x8a, 0xd8, 0xb4, 0x0a, 0xd8, 0xa8, 0xd8, 0xb1, 0xd8, 0xae, 
        0xd9, 0x88, 0xd8, 0xb1, 0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 0xb1, 0x0a, 0xda, 0x86, 0xd9, 0x88, 0xd9, 
        0x86, 0x0a, 0xd8, 0xae, 0xd8, 0xa7, 0xd8, 0xb1, 0xd8, 0xac, 0x0a, 0xd8, 0xb4, 0xd8, 0xb4, 0x0a, 
        0xd9, 0x87, 0xd9, 0x86, 0xd9, 0x88, 0xd8, 0xb2, 0x0a, 0xd8, 0xaa, 0xd8, 0xad, 0xd8, 0xaa, 0x0a, 
        0xd8, 0xb6, 0xd9, 0x85, 0xd9, 0x86, 0x0a, 0xd9, 0x87, 0xd8, 0xb3, 0xd8, 0xaa, 0xd9, 0x8a, 0xd9, 
        0x85, 0x0a, 0xda, 0xaf, 0xd9, 0x81, 0xd8, 0xaa, 0xd9, 0x87, 0x0a, 0xd9, 0x81, 0xd9, 0x83, 0xd8, 
        0xb1, 0x0a, 0xd8, 0xa8, 0xd8, 0xb3, 0xd9, 0x8a, 0xd8, 0xa7, 0xd8, 0xb1, 0x0a, 0xd9, 0xbe, 0xd9, 
        0x8a, 0xd8, 0xb4, 0x0a, 0xd8, 0xa8, 0xd8, 0xb1, 0xd8, 0xa7, 0xd9, 0x8a, 0x0a, 0xd8, 0xb1, 0xd9, 
        0x88, 0xd8, 0xb2, 0xd9, 0x87, 0xd8, 0xa7, 0xd9, 0x8a, 0x0a, 0xd8, 0xa7, 0xd9, 0x86, 0xd9, 0x83, 
        0xd9, 0x87, 0x0a, 0xd9, 0x86, 0xd8, 0xae, 0xd9, 0x88, 0xd8, 0xa7, 0xd9, 0x87, 0xd8, 0xaf, 0x0a, 
        0xd8, 0xa8, 0xd8, 0xa7, 0xd9, 0x84, 0xd8, 0xa7, 0x0a, 0xd9, 0x83, 0xd9, 0x84, 0x0a, 0xd9, 0x88, 
        0xd9, 0x82, 0xd8, 0xaa, 0xd9, 0x8a, 0x0a, 0xd9, 0x83, 0xd9, 0x8a, 0x0a, 0xda, 0x86, 0xd9, 0x86, 
        0xd9, 0x8a, 0xd9, 0x86, 0x0a, 0xd9, 0x83, 0xd9, 0x87, 0x0a, 0xda, 0xaf, 0xd9, 0x8a, 0xd8, 0xb1, 
        0xd9, 0x8a, 0x0a, 0xd9, 0x86, 0xd9, 0x8a, 0xd8, 0xb3, 0xd8, 0xaa, 0x0a, 0xd8, 0xa7, 0xd8, 0xb3, 
        0xd8, 0xaa, 0x0a, 0xd9, 0x83, 0xd8, 0xac, 0xd8, 0xa7, 0x0a, 0xd9, 0x83, 0xd9, 0x86, 0xd8, 0xaf, 
        0x0a, 0xd9, 0x86, 0xd9, 0x8a, 0xd8, 0xb2, 0x0a, 0xd9, 0x8a, 0xd8, 0xa7, 0xd8, 0xa8, 0xd8, 0xaf, 
        0x0a, 0xd8, 0xa8, 0xd9, 0x86, 0xd8, 0xaf, 0xd9, 0x8a, 0x0a, 0xd8, 0xad, 0xd8, 0xaa, 0xd9, 0x8a, 
        0x0a, 0xd8, 0xaa, 0xd9, 0x88, 0xd8, 0xa7, 0xd9, 0x86, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xd8, 0xb9, 
        0xd9, 0x82, 0xd8, 0xa8, 0x0a, 0xd8, 0xae, 0xd9, 0x88, 0xd8, 0xa7, 0xd8, 0xb3, 0xd8, 0xaa, 0x0a, 
        0xd9, 0x83, 0xd9, 0x86, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xd8, 0xa8, 0xd9, 0x8a, 0xd9, 0x86, 0x0a, 
        0xd8, 0xaa, 0xd9, 0x85, 0xd8, 0xa7, 0xd9, 0x85, 0x0a, 0xd9, 0x87, 0xd9, 0x85, 0xd9, 0x87, 0x0a, 
        0xd9, 0x85, 0xd8, 0xa7, 0x0a, 0xd8, 0xa8, 0xd8, 0xa7, 0xd8, 0xb4, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 
        0xd9, 0x85, 0xd8, 0xab, 0xd9, 0x84, 0x0a, 0xd8, 0xb4, 0xd8, 0xaf, 0x0a, 0xd8, 0xa7, 0xd8, 0xb1, 
        0xd9, 0x8a, 0x0a, 0xd8, 0xa8, 0xd8, 0xa7, 0xd8, 0xb4, 0xd8, 0xaf, 0x0a, 0xd8, 0xa7, 0xd8, 0xb1, 
        0xd9, 0x87, 0x0a, 0xd8, 0xb7, 0xd8, 0xa8, 0xd9, 0x82, 0x0a, 0xd8, 0xa8, 0xd8, 0xb9, 0xd8, 0xaf, 
        0x0a, 0xd8, 0xa7, 0xda, 0xaf, 0xd8, 0xb1, 0x0a, 0xd8, 0xb5, 0xd9, 0x88, 0xd8, 0xb1, 0xd8, 0xaa, 
        0x0a, 0xd8, 0xba, 0xd9, 0x8a, 0xd8, 0xb1, 0x0a, 0xd8, 0xac, 0xd8, 0xa7, 0xd9, 0x8a, 0x0a, 0xd8, 
        0xa8, 0xd9, 0x8a, 0xd8, 0xb4, 0x0a, 0xd8, 0xb1, 0xd9, 0x8a, 0xd8, 0xb2, 0xd9, 0x8a, 0x0a, 0xd8, 
        0xa7, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xd8, 0xb2, 0xd9, 0x8a, 0xd8, 0xb1, 0xd8, 0xa7, 0x0a, 0xda, 
        0x86, 0xda, 0xaf, 0xd9, 0x88, 0xd9, 0x86, 0xd9, 0x87, 0x0a, 0xd8, 0xa8, 0xd8, 0xa7, 0xd8, 0xb1, 
        0x0a, 0xd9, 0x84, 0xd8, 0xb7, 0xd9, 0x81, 0xd8, 0xa7, 0x0a, 0xd9, 0x85, 0xd9, 0x8a, 0x0a, 0xd8, 
        0xaf, 0xd8, 0xb1, 0xd8, 0xa8, 0xd8, 0xa7, 0xd8, 0xb1, 0xd9, 0x87, 0x0a, 0xd9, 0x85, 0xd9, 0x86, 
        0x0a, 0xd8, 0xaf, 0xd9, 0x8a, 0xd8, 0xaf, 0xd9, 0x87, 0x0a, 0xd9, 0x87, 0xd9, 0x85, 0xd9, 0x8a, 
        0xd9, 0x86, 0x0a, 0xda, 0xaf, 0xd8, 0xb0, 0xd8, 0xa7, 0xd8, 0xb1, 0xd9, 0x8a, 0x0a, 0xd8, 0xa8, 
        0xd8, 0xb1, 0xd8, 0xaf, 0xd8, 0xa7, 0xd8, 0xb1, 0xd9, 0x8a, 0x0a, 0xd8, 0xb9, 0xd9, 0x84, 0xd8, 
        0xaa, 0x0a, 0xda, 0xaf, 0xd8, 0xb0, 0xd8, 0xa7, 0xd8, 0xb4, 0xd8, 0xaa, 0xd9, 0x87, 0x0a, 0xd9, 
        0x87, 0xd9, 0x85, 0x0a, 0xd9, 0x81, 0xd9, 0x88, 0xd9, 0x82, 0x0a, 0xd9, 0x86, 0xd9, 0x87, 0x0a, 
        0xd9, 0x87, 0xd8, 0xa7, 0x0a, 0xd8, 0xb4, 0xd9, 0x88, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xd8, 0xa7, 
        0xd8, 0xa8, 0xd8, 0xa7, 0xd8, 0xaf, 0x0a, 0xd9, 0x87, 0xd9, 0x85, 0xd9, 0x88, 0xd8, 0xa7, 0xd8, 
        0xb1, 0xd9, 0x87, 0x0a, 0xd9, 0x87, 0xd8, 0xb1, 0x0a, 0xd8, 0xa7, 0xd9, 0x88, 0xd9, 0x84, 0x0a, 
        0xd8, 0xae, 0xd9, 0x88, 0xd8, 0xa7, 0xd9, 0x87, 0xd9, 0x86, 0xd8, 0xaf, 0x0a, 0xda, 0x86, 0xd9, 
        0x87, 0xd8, 0xa7, 0xd8, 0xb1, 0x0a, 0xd9, 0x86, 0xd8, 0xa7, 0xd9, 0x85, 0x0a, 0xd8, 0xa7, 0xd9, 
        0x85, 0xd8, 0xb1, 0xd9, 0x88, 0xd8, 0xb2, 0x0a, 0xd9, 0x85, 0xd8, 0xa7, 0xd9, 0x86, 0x0a, 0xd9, 
        0x87, 0xd8, 0xa7, 0xd9, 0x8a, 0x0a, 0xd9, 0x82, 0xd8, 0xa8, 0xd9, 0x84, 0x0a, 0xd9, 0x83, 0xd9, 
        0x86, 0xd9, 0x85, 0x0a, 0xd8, 0xb3, 0xd8, 0xb9, 0xd9, 0x8a, 0x0a, 0xd8, 0xaa, 0xd8, 0xa7, 0xd8, 
        0xb2, 0xd9, 0x87, 0x0a, 0xd8, 0xb1, 0xd8, 0xa7, 0x0a, 0xd9, 0x87, 0xd8, 0xb3, 0xd8, 0xaa, 0xd9, 
        0x86, 0xd8, 0xaf, 0x0a, 0xd8, 0xb2, 0xd9, 0x8a, 0xd8, 0xb1, 0x0a, 0xd8, 0xac, 0xd9, 0x84, 0xd9, 
        0x88, 0xd9, 0x8a, 0x0a, 0xd8, 0xb9, 0xd9, 0x86, 0xd9, 0x88, 0xd8, 0xa7, 0xd9, 0x86, 0x0a, 0xd8, 
        0xa8, 0xd9, 0x88, 0xd8, 0xaf, 0x0a
    };
    
    PersianAnalyzer::PersianAnalyzer(LuceneVersion::Version matchVersion)
    {
        this->stoptable = getDefaultStopSet();
        this->matchVersion = matchVersion;
    }
    
    PersianAnalyzer::PersianAnalyzer(LuceneVersion::Version matchVersion, HashSet<String> stopwords)
    {
        this->stoptable = stopwords;
        this->matchVersion = matchVersion;
    }
    
    PersianAnalyzer::~PersianAnalyzer()
    {
    }
    
    HashSet<String> PersianAnalyzer::getDefaultStopSet()
    {
        static HashSet<String> stopSet;
        if (!stopSet)
        {
            String stopWords(UTF8_TO_STRING(DEFAULT_STOPWORD_FILE));
            Collection<String> words(StringUtils::split(stopWords, L"\n"));
            stopSet = HashSet<String>::newInstance(words.begin(), words.end());
        }
        return stopSet;
    }
    
    TokenStreamPtr PersianAnalyzer::tokenStream(const String& fieldName, ReaderPtr reader)
    {
        TokenStreamPtr result = newLucene<ArabicLetterTokenizer>(reader);
        result = newLucene<LowerCaseFilter>(result);
        result = newLucene<ArabicNormalizationFilter>(result);
        // additional Persian-specific normalization
        result = newLucene<PersianNormalizationFilter>(result);
         // the order here is important: the stopword list is not normalized
        result = newLucene<StopFilter>(StopFilter::getEnablePositionIncrementsVersionDefault(matchVersion), result, stoptable);
        return result;
    }
    
    TokenStreamPtr PersianAnalyzer::reusableTokenStream(const String& fieldName, ReaderPtr reader)
    {
        PersianAnalyzerSavedStreamsPtr streams(boost::dynamic_pointer_cast<PersianAnalyzerSavedStreams>(getPreviousTokenStream()));
        if (!streams)
        {
            streams = newLucene<PersianAnalyzerSavedStreams>();
            streams->source = newLucene<ArabicLetterTokenizer>(reader);
            streams->result = newLucene<LowerCaseFilter>(streams->source);
            streams->result = newLucene<ArabicNormalizationFilter>(streams->result);
            // additional Persian-specific normalization
            streams->result = newLucene<PersianNormalizationFilter>(streams->result);
            // the order here is important: the stopword list is not normalized
            streams->result = newLucene<StopFilter>(StopFilter::getEnablePositionIncrementsVersionDefault(matchVersion), streams->result, stoptable);
            setPreviousTokenStream(streams);
        }
        else
            streams->source->reset(reader);
        return streams->result;
    }
    
    PersianAnalyzerSavedStreams::~PersianAnalyzerSavedStreams()
    {
    }
}
