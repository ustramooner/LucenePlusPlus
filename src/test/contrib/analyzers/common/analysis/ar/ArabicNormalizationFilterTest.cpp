/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseTokenStreamFixture.h"
#include "ArabicLetterTokenizer.h"
#include "ArabicNormalizationFilter.h"
#include "StringReader.h"

#include "StringReader.h"

using namespace Lucene;

class ArabicNormalizationFilterFixture : public BaseTokenStreamFixture
{
public:
	virtual ~ArabicNormalizationFilterFixture()
	{
	}

public:
	void check(const String& input, const String& expected)
    {
        ArabicLetterTokenizerPtr tokenStream  = newLucene<ArabicLetterTokenizer>(newLucene<StringReader>(input));
        ArabicNormalizationFilterPtr filter = newLucene<ArabicNormalizationFilter>(tokenStream);
        checkTokenStreamContents(filter, newCollection<String>(expected));
    }
};

BOOST_FIXTURE_TEST_SUITE(ArabicNormalizationFilterTest, ArabicNormalizationFilterFixture)

BOOST_AUTO_TEST_CASE(testAlifMadda)
{
    const uint8_t first[] = {0xd8, 0xa2, 0xd8, 0xac, 0xd9, 0x86};
    const uint8_t second[] = {0xd8, 0xa7, 0xd8, 0xac, 0xd9, 0x86};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testAlifHamzaAbove)
{
    const uint8_t first[] = {0xd8, 0xa3, 0xd8, 0xad, 0xd9, 0x85, 0xd8, 0xaf};
    const uint8_t second[] = {0xd8, 0xa7, 0xd8, 0xad, 0xd9, 0x85, 0xd8, 0xaf};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testAlifHamzaBelow)
{
    const uint8_t first[] = {0xd8, 0xa5, 0xd8, 0xb9, 0xd8, 0xa7, 0xd8, 0xb0};
    const uint8_t second[] = {0xd8, 0xa7, 0xd8, 0xb9, 0xd8, 0xa7, 0xd8, 0xb0};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testAlifMaksura)
{
    const uint8_t first[] = {0xd8, 0xa8, 0xd9, 0x86, 0xd9, 0x89};
    const uint8_t second[] = {0xd8, 0xa8, 0xd9, 0x86, 0xd9, 0x8a};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testTehMarbuta)
{
    const uint8_t first[] = {0xd9, 0x81, 0xd8, 0xa7, 0xd8, 0xb7, 0xd9, 0x85, 0xd8, 0xa9};
    const uint8_t second[] = {0xd9, 0x81, 0xd8, 0xa7, 0xd8, 0xb7, 0xd9, 0x85, 0xd9, 0x87};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testTatweel)
{
    const uint8_t first[] = {0xd8, 0xb1, 0xd9, 0x88, 0xd8, 0xa8, 0xd8, 0xb1, 0xd9, 0x80, 0xd9, 0x80, 0xd9, 0x80, 0xd9, 0x80, 0xd9, 0x80, 0xd8, 0xaa};
    const uint8_t second[] = {0xd8, 0xb1, 0xd9, 0x88, 0xd8, 0xa8, 0xd8, 0xb1, 0xd8, 0xaa};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testFatha)
{
    const uint8_t first[] = {0xd9, 0x85, 0xd9, 0x8e, 0xd8, 0xa8, 0xd9, 0x86, 0xd8, 0xa7};
    const uint8_t second[] = {0xd9, 0x85, 0xd8, 0xa8, 0xd9, 0x86, 0xd8, 0xa7};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testKasra)
{
    const uint8_t first[] = {0xd8, 0xb9, 0xd9, 0x84, 0xd9, 0x90, 0xd9, 0x8a};
    const uint8_t second[] = {0xd8, 0xb9, 0xd9, 0x84, 0xd9, 0x8a};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testDamma)
{
    const uint8_t first[] = {0xd8, 0xa8, 0xd9, 0x8f, 0xd9, 0x88, 0xd8, 0xa7, 0xd8, 0xaa};
    const uint8_t second[] = {0xd8, 0xa8, 0xd9, 0x88, 0xd8, 0xa7, 0xd8, 0xaa};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testFathatan)
{
    const uint8_t first[] = {0xd9, 0x88, 0xd9, 0x84, 0xd8, 0xaf, 0xd8, 0xa7, 0xd9, 0x8b};
    const uint8_t second[] = {0xd9, 0x88, 0xd9, 0x84, 0xd8, 0xaf, 0xd8, 0xa7};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testKasratan)
{
    const uint8_t first[] = {0xd9, 0x88, 0xd9, 0x84, 0xd8, 0xaf, 0xd9, 0x8d};
    const uint8_t second[] = {0xd9, 0x88, 0xd9, 0x84, 0xd8, 0xaf};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testDammatan)
{
    const uint8_t first[] = {0xd9, 0x88, 0xd9, 0x84, 0xd8, 0xaf, 0xd9, 0x8c};
    const uint8_t second[] = {0xd9, 0x88, 0xd9, 0x84, 0xd8, 0xaf};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testSukun)
{
    const uint8_t first[] = {0xd9, 0x86, 0xd9, 0x84, 0xd9, 0x92, 0xd8, 0xb3, 0xd9, 0x88, 0xd9, 0x86};
    const uint8_t second[] = {0xd9, 0x86, 0xd9, 0x84, 0xd8, 0xb3, 0xd9, 0x88, 0xd9, 0x86};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_CASE(testShaddah)
{
    const uint8_t first[] = {0xd9, 0x87, 0xd8, 0xaa, 0xd9, 0x85, 0xd9, 0x8a, 0xd9, 0x91};
    const uint8_t second[] = {0xd9, 0x87, 0xd8, 0xaa, 0xd9, 0x85, 0xd9, 0x8a};
    check(UTF8_TO_STRING(first), UTF8_TO_STRING(second));
}

BOOST_AUTO_TEST_SUITE_END()
