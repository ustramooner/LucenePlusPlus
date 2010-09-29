/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LuceneTestFixture.h"
#include "NumericUtils.h"
#include "OpenBitSet.h"

using namespace Lucene;

BOOST_FIXTURE_TEST_SUITE(NumericUtilsTest, LuceneTestFixture)

class CheckLongRangeBuilder : public LongRangeBuilder
{
public:
    CheckLongRangeBuilder(int64_t lower, int64_t upper, OpenBitSetPtr bits, Collection<int64_t>::iterator neededBounds)
    {
        this->lower = lower;
        this->upper = upper;
        this->bits = bits;
        this->neededBounds = neededBounds;
    }
    
    virtual ~CheckLongRangeBuilder()
    {
    }

protected:
    int64_t lower;
    int64_t upper;
    OpenBitSetPtr bits;
    Collection<int64_t>::iterator neededBounds;

public:
    virtual void addRange(int64_t min, int64_t max, int32_t shift)
    {
        BOOST_CHECK(min >= lower && min <= upper && max >= lower && max <= upper);
        if (bits)
        {
            for (int64_t l = min; l <= max; ++l)
                BOOST_CHECK(!bits->getAndSet((int32_t)(l - lower)));
        }
        // make unsigned longs for easier display and understanding
        min ^= 0x8000000000000000LL;
        max ^= 0x8000000000000000LL;
        
        BOOST_CHECK_EQUAL(*neededBounds++, (uint64_t)min >> shift); // inner min bound
        BOOST_CHECK_EQUAL(*neededBounds++, (uint64_t)max >> shift); // inner max bound
    }
};

static void checkLongRangeSplit(int64_t lower, int64_t upper, int32_t precisionStep, bool useBitSet, Collection<int64_t> neededBounds)
{
    OpenBitSetPtr bits = useBitSet ? newLucene<OpenBitSet>((int32_t)(upper - lower + 1)) : OpenBitSetPtr();
    NumericUtils::splitLongRange(newLucene<CheckLongRangeBuilder>(lower, upper, bits, neededBounds.begin()), precisionStep, lower, upper);
    
    if (useBitSet)
    {
        // after flipping all bits in the range, the cardinality should be zero
        bits->flip(0, (int32_t)(upper - lower + 1));
        BOOST_CHECK(bits->isEmpty());
    }	
}

class CheckIntRangeBuilder : public IntRangeBuilder
{
public:
    CheckIntRangeBuilder(int32_t lower, int32_t upper, OpenBitSetPtr bits, Collection<int32_t>::iterator neededBounds)
    {
        this->lower = lower;
        this->upper = upper;
        this->bits = bits;
        this->neededBounds = neededBounds;
    }
    
    virtual ~CheckIntRangeBuilder()
    {
    }

protected:
    int32_t lower;
    int32_t upper;
    OpenBitSetPtr bits;
    Collection<int32_t>::iterator neededBounds;

public:
    virtual void addRange(int32_t min, int32_t max, int32_t shift)
    {
        BOOST_CHECK(min >= lower && min <= upper && max >= lower && max <= upper);
        if (bits)
        {
            for (int32_t l = min; l <= max; ++l)
                BOOST_CHECK(!bits->getAndSet((int32_t)(l - lower)));
        }
        // make unsigned longs for easier display and understanding
        min ^= 0x80000000;
        max ^= 0x80000000;
        
        BOOST_CHECK_EQUAL(*neededBounds++, (uint32_t)min >> shift); // inner min bound
        BOOST_CHECK_EQUAL(*neededBounds++, (uint32_t)max >> shift); // inner max bound
    }
};

static void checkIntRangeSplit(int32_t lower, int32_t upper, int32_t precisionStep, bool useBitSet, Collection<int32_t> neededBounds)
{
    OpenBitSetPtr bits = useBitSet ? newLucene<OpenBitSet>((int32_t)(upper - lower + 1)) : OpenBitSetPtr();
    NumericUtils::splitIntRange(newLucene<CheckIntRangeBuilder>(lower, upper, bits, neededBounds.begin()), precisionStep, lower, upper);
    
    if (useBitSet)
    {
        // after flipping all bits in the range, the cardinality should be zero
        bits->flip(0, (int32_t)(upper - lower + 1));
        BOOST_CHECK(bits->isEmpty());
    }	
}
    
BOOST_AUTO_TEST_CASE(testLongConversionAndOrdering)
{
    // generate a series of encoded longs, each numerical one bigger than the one before
    String last;
    for (int64_t l = -100000; l < 100000; ++l)
    {
        String act = NumericUtils::longToPrefixCoded(l);
        if (!last.empty())
        {
            // test if smaller
            BOOST_CHECK(last.compare(act) < 0);
        }
        // test is back and forward conversion works
        BOOST_CHECK_EQUAL(l, NumericUtils::prefixCodedToLong(act));
        // next step
        last = act;
    }
}

BOOST_AUTO_TEST_CASE(testIntConversionAndOrdering)
{
    // generate a series of encoded ints, each numerical one bigger than the one before
    String last;
    for (int32_t l = -100000; l < 100000; ++l)
    {
        String act = NumericUtils::intToPrefixCoded(l);
        if (!last.empty())
        {
            // test if smaller
            BOOST_CHECK(last.compare(act) < 0);
        }
        // test is back and forward conversion works
        BOOST_CHECK_EQUAL(l, NumericUtils::prefixCodedToInt(act));
        // next step
        last = act;
    }
}

BOOST_AUTO_TEST_CASE(testLongSpecialValues)
{
    static const int64_t vals[] = {LLONG_MIN, LLONG_MIN + 1, LLONG_MIN + 2, -5003400000000LL, -4000LL, -3000LL, 
                                   -2000LL, -1000LL, -1LL, 0LL, 1LL, 10LL, 300LL, 50006789999999999LL, 
                                   LLONG_MAX - 2, LLONG_MAX - 1, LLONG_MAX};
    int32_t length = SIZEOF_ARRAY(vals);
    Collection<String> prefixVals = Collection<String>::newInstance(length);
    for (int32_t i = 0; i < length; ++i)
    {
        prefixVals[i] = NumericUtils::longToPrefixCoded(vals[i]);

        // check forward and back conversion
        BOOST_CHECK_EQUAL(vals[i], NumericUtils::prefixCodedToLong(prefixVals[i]));
        
        // test if decoding values as long fails correctly
        BOOST_CHECK_EXCEPTION(NumericUtils::prefixCodedToInt(prefixVals[i]), NumberFormatException, check_exception(LuceneException::NumberFormat));
    }
    
    // check sort order (prefixVals should be ascending)
    for (int32_t i = 1; i < prefixVals.size(); ++i)
        BOOST_CHECK(prefixVals[i - 1].compare(prefixVals[i]) < 0);
    
    // check the prefix encoding, lower precision should have the difference to original 
    // value equal to the lower removed bits
    for (int32_t i = 0; i < length; ++i)
    {
        for (int32_t j = 0; j < 32; ++j)
        {
            int64_t prefixVal = NumericUtils::prefixCodedToLong(NumericUtils::longToPrefixCoded(vals[i], j));
            int64_t mask = ((int64_t)1 << j) - 1;
            BOOST_CHECK_EQUAL(vals[i] & mask, vals[i] - prefixVal);
        }
    }
}

BOOST_AUTO_TEST_CASE(testIntSpecialValues)
{
    static const int32_t vals[] = {INT_MIN, INT_MIN + 1, INT_MIN + 2, -64765767, -4000, -3000, -2000,
                                   -1000, -1, 0, 1, 10, 300, 765878989, INT_MAX - 2, INT_MAX- 1, INT_MAX};
    int32_t length = SIZEOF_ARRAY(vals);
    Collection<String> prefixVals = Collection<String>::newInstance(length);
    for (int32_t i = 0; i < length; ++i)
    {
        prefixVals[i] = NumericUtils::intToPrefixCoded(vals[i]);

        // check forward and back conversion
        BOOST_CHECK_EQUAL(vals[i], NumericUtils::prefixCodedToInt(prefixVals[i]));
        
        // test if decoding values as long fails correctly
        BOOST_CHECK_EXCEPTION(NumericUtils::prefixCodedToLong(prefixVals[i]), NumberFormatException, check_exception(LuceneException::NumberFormat));
    }
    
    // check sort order (prefixVals should be ascending)
    for (int32_t i = 1; i < prefixVals.size(); ++i)
        BOOST_CHECK(prefixVals[i - 1].compare(prefixVals[i]) < 0);
    
    // check the prefix encoding, lower precision should have the difference to original 
    // value equal to the lower removed bits
    for (int32_t i = 0; i < length; ++i)
    {
        for (int32_t j = 0; j < 32; ++j)
        {
            int32_t prefixVal = NumericUtils::prefixCodedToInt(NumericUtils::intToPrefixCoded(vals[i], j));
            int32_t mask = ((int32_t)1 << j) - 1;
            BOOST_CHECK_EQUAL(vals[i] & mask, vals[i] - prefixVal);
        }
    }
}

BOOST_AUTO_TEST_CASE(testDoubles)
{
    static const double vals[] = {-std::numeric_limits<double>::infinity(), -2.3E25, -1.0E15, -1.0, 
                                  -1.0E-1, -1.0E-2, -0.0, +0.0, 1.0E-2, 1.0E-1, 1.0, 1.0E15, 2.3E25, 
                                  std::numeric_limits<double>::infinity()};
    int32_t length = SIZEOF_ARRAY(vals);
    Collection<int64_t> longVals = Collection<int64_t>::newInstance(length);
    
    // check forward and back conversion
    for (int32_t i = 0; i < length; ++i)
    {
        longVals[i] = NumericUtils::doubleToSortableLong(vals[i]);
        BOOST_CHECK_EQUAL(vals[i], NumericUtils::sortableLongToDouble(longVals[i]));
    }
    
    // check sort order (longVals should be ascending)
    for (int32_t i = 1; i < longVals.size(); ++i)
        BOOST_CHECK(longVals[i - 1] < longVals[i]);
}

BOOST_AUTO_TEST_CASE(testSplitLongRange)
{
    Collection<int64_t> neededBounds = Collection<int64_t>::newInstance(14);
    neededBounds[0] = 0x7fffffffffffec78LL;
    neededBounds[1] = 0x7fffffffffffec7fLL;
    neededBounds[2] = 0x8000000000002510LL;
    neededBounds[3] = 0x800000000000251cLL;
    neededBounds[4] = 0x7fffffffffffec8LL;
    neededBounds[5] = 0x7fffffffffffecfLL;
    neededBounds[6] = 0x800000000000250LL;
    neededBounds[7] = 0x800000000000250LL;
    neededBounds[8] = 0x7fffffffffffedLL;
    neededBounds[9] = 0x7fffffffffffefLL;
    neededBounds[10] = 0x80000000000020LL;
    neededBounds[11] = 0x80000000000024LL;
    neededBounds[12] = 0x7ffffffffffffLL;
    neededBounds[13] = 0x8000000000001LL;
        
    // a hard-coded "standard" range
    checkLongRangeSplit(-5000, 9500, 4, true, neededBounds);

    // the same with no range splitting
    checkLongRangeSplit(-5000, 9500, 64, true, newCollection<int64_t>(0x7fffffffffffec78LL, 0x800000000000251cLL));
    
    // this tests optimized range splitting, if one of the inner bounds
    // is also the bound of the next lower precision, it should be used completely
    checkLongRangeSplit(0, 1024 + 63, 4, true, newCollection<int64_t>(0x800000000000040LL, 0x800000000000043LL,
                        0x80000000000000LL, 0x80000000000003LL));
    
    // the full long range should only consist of a lowest precision range; 
    // no bitset testing here, as too much memory needed
    checkLongRangeSplit(LLONG_MIN, LLONG_MAX, 8, false, newCollection<int64_t>(0x00LL, 0xffLL));
    
    // the same with precisionStep=4
    checkLongRangeSplit(LLONG_MIN, LLONG_MAX, 4, false, newCollection<int64_t>(0x00LL, 0xfLL));
    
    // the same with precisionStep=2
    checkLongRangeSplit(LLONG_MIN, LLONG_MAX, 2, false, newCollection<int64_t>(0x00LL, 0x3LL));
    
    // the same with precisionStep=1
    checkLongRangeSplit(LLONG_MIN, LLONG_MAX, 1, false, newCollection<int64_t>(0x00LL, 0x1LL));
    
    // a inverse range should produce no sub-ranges
    checkLongRangeSplit(9500, -5000, 4, false,  Collection<int64_t>::newInstance());
    
    // a 0-length range should reproduce the range itself
    checkLongRangeSplit(9500, 9500, 4, false, newCollection<int64_t>(0x800000000000251cLL, 0x800000000000251cLL));
}

BOOST_AUTO_TEST_CASE(testSplitIntRange)
{
    Collection<int32_t> neededBounds = Collection<int32_t>::newInstance(14);
    neededBounds[0] = 0x7fffec78;
    neededBounds[1] = 0x7fffec7f;
    neededBounds[2] = 0x80002510;
    neededBounds[3] = 0x8000251c;
    neededBounds[4] = 0x7fffec8;
    neededBounds[5] = 0x7fffecf;
    neededBounds[6] = 0x8000250;
    neededBounds[7] = 0x8000250;
    neededBounds[8] = 0x7fffed;
    neededBounds[9] = 0x7fffef;
    neededBounds[10] = 0x800020;
    neededBounds[11] = 0x800024;
    neededBounds[12] = 0x7ffff;
    neededBounds[13] = 0x80001;
        
    // a hard-coded "standard" range
    checkIntRangeSplit(-5000, 9500, 4, true, neededBounds);

    // the same with no range splitting
    checkIntRangeSplit(-5000, 9500, 32, true, newCollection<int32_t>(0x7fffec78, 0x8000251c));
    
    // this tests optimized range splitting, if one of the inner bounds
    // is also the bound of the next lower precision, it should be used completely
    checkIntRangeSplit(0, 1024 + 63, 4, true, newCollection<int32_t>(0x8000040, 0x8000043,
                        0x800000, 0x800003));
    
    // the full long range should only consist of a lowest precision range; 
    // no bitset testing here, as too much memory needed
    checkIntRangeSplit(INT_MIN, INT_MAX, 8, false, newCollection<int32_t>(0x00, 0xff));
    
    // the same with precisionStep=4
    checkIntRangeSplit(INT_MIN, INT_MAX, 4, false, newCollection<int32_t>(0x00, 0xf));
    
    // the same with precisionStep=2
    checkIntRangeSplit(INT_MIN, INT_MAX, 2, false, newCollection<int32_t>(0x00, 0x3));
    
    // the same with precisionStep=1
    checkIntRangeSplit(INT_MIN, INT_MAX, 1, false, newCollection<int32_t>(0x00, 0x1));
    
    // a inverse range should produce no sub-ranges
    checkIntRangeSplit(9500, -5000, 4, false,  Collection<int32_t>::newInstance());
    
    // a 0-length range should reproduce the range itself
    checkIntRangeSplit(9500, 9500, 4, false, newCollection<int32_t>(0x8000251c, 0x8000251c));
}

BOOST_AUTO_TEST_SUITE_END()