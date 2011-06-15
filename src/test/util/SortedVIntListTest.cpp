/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "TestInc.h"
#include "LuceneTestFixture.h"
#include "SortedVIntList.h"
#include "BitSet.h"
#include "DocIdSetIterator.h"

using namespace Lucene;

BOOST_FIXTURE_TEST_SUITE(SortedVIntListTest, LuceneTestFixture)

static const int32_t VB1 = 0x7F;
static const int32_t BIT_SHIFT = 7;
static const int32_t VB2 = (VB1 << BIT_SHIFT) | VB1;
static const int32_t VB3 = (VB2 << BIT_SHIFT) | VB1;
static const int32_t VB4 = (VB3 << BIT_SHIFT) | VB1;

static int32_t vIntByteSize(int32_t i)
{
    BOOST_CHECK(i >= 0);
    if (i <= VB1)
        return 1;
    if (i <= VB2)
        return 2;
    if (i <= VB3)
        return 3;
    if (i <= VB4)
        return 4;
    return 5;
}

static int32_t vIntListByteSize(Collection<int32_t> ints)
{
    int32_t byteSize = 0;
    int32_t last = 0;
    for (int32_t i = 0; i < ints.size(); ++i)
    {
        byteSize += vIntByteSize(ints[i] - last);
        last = ints[i];
    }
    return byteSize;
}

static void tstIterator(SortedVIntListPtr vintList, Collection<int32_t> ints)
{
    for (int32_t i = 0; i < ints.size(); ++i)
    {
        if ((i > 0) && (ints[i - 1] == ints[i]))
            return; // DocNrSkipper should not skip to same document.
    }
    DocIdSetIteratorPtr m = vintList->iterator();
    for (int32_t i = 0; i < ints.size(); ++i)
    {
        BOOST_CHECK(m->nextDoc() != DocIdSetIterator::NO_MORE_DOCS);
        BOOST_CHECK_EQUAL(ints[i], m->docID());
    }
    BOOST_CHECK_EQUAL(m->nextDoc(), DocIdSetIterator::NO_MORE_DOCS);
}

static void tstVIntList(SortedVIntListPtr vintList, Collection<int32_t> ints, int32_t expectedByteSize)
{
    BOOST_CHECK_EQUAL(ints.size(), vintList->size());
    BOOST_CHECK_EQUAL(expectedByteSize, vintList->getByteSize());
    tstIterator(vintList, ints);
}

static void tstViaBitSet(Collection<int32_t> ints, int32_t expectedByteSize)
{
    int32_t MAX_INT_FOR_BITSET = 1024 * 1024;
    BitSetPtr bs = newLucene<BitSet>();
    for (int32_t i = 0; i < ints.size(); ++i)
    {
        if (ints[i] > MAX_INT_FOR_BITSET)
            return; // BitSet takes too much memory
        if ((i > 0) && (ints[i - 1] == ints[i]))
            return; // BitSet cannot store duplicate.
        bs->set(ints[i]);
    }
    SortedVIntListPtr svil = newLucene<SortedVIntList>(bs);
    tstVIntList(svil, ints, expectedByteSize);
    tstVIntList(newLucene<SortedVIntList>(svil->iterator()), ints, expectedByteSize);
}
          
static void tstInts(Collection<int32_t> ints)
{
    int32_t expectedByteSize = vIntListByteSize(ints);
    tstVIntList(newLucene<SortedVIntList>(ints), ints, expectedByteSize);
    tstViaBitSet(ints, expectedByteSize);
}

static Collection<int32_t> fibArray(int32_t a, int32_t b, int32_t size)
{
    Collection<int32_t> fib = Collection<int32_t>::newInstance(size);
    fib[0] = a;
    fib[1] = b;
    for (int32_t i = 2; i < size; ++i)
        fib[i] = fib[i - 1] + fib[i - 2];
    return fib;
}

/// reverse the order of the successive differences
static Collection<int32_t> reverseDiffs(Collection<int32_t> ints)
{
    Collection<int32_t> res = Collection<int32_t>::newInstance(ints.size());
    for (int32_t i = 0; i < ints.size(); ++i)
        res[i] = ints[ints.size() - 1] + (ints[0] - ints[ints.size() - 1 - i]);
    return res;
}

static void tstIllegalArgExc(Collection<int32_t> ints)
{
    BOOST_CHECK_EXCEPTION(newLucene<SortedVIntList>(ints), IllegalArgumentException, check_exception(LuceneException::IllegalArgument));
}

BOOST_AUTO_TEST_CASE(test01)
{
    tstInts(Collection<int32_t>::newInstance());
}

BOOST_AUTO_TEST_CASE(test02)
{
    tstInts(newCollection<int32_t>(0));
}

BOOST_AUTO_TEST_CASE(test04a)
{
    tstInts(newCollection<int32_t>(0, VB2 - 1));
}

BOOST_AUTO_TEST_CASE(test04b)
{
    tstInts(newCollection<int32_t>(0, VB2));
}

BOOST_AUTO_TEST_CASE(test04c)
{
    tstInts(newCollection<int32_t>(0, VB2 + 1));
}

BOOST_AUTO_TEST_CASE(test05)
{
    tstInts(fibArray(0, 1, 7)); // includes duplicate value 1
}

BOOST_AUTO_TEST_CASE(test05b)
{
    tstInts(reverseDiffs(fibArray(0, 1, 7))); // includes duplicate value 1
}

BOOST_AUTO_TEST_CASE(test06)
{
    tstInts(fibArray(1, 2, 45)); // no duplicates, size 46 exceeds max int.
}

BOOST_AUTO_TEST_CASE(test06b)
{
    tstInts(reverseDiffs(fibArray(1, 2, 45))); // includes duplicate value 1
}

BOOST_AUTO_TEST_CASE(test07a)
{
    tstInts(newCollection<int32_t>(0, VB3));
}

BOOST_AUTO_TEST_CASE(test07b)
{
    tstInts(newCollection<int32_t>(1, VB3 + 2));
}

BOOST_AUTO_TEST_CASE(test07c)
{
    tstInts(newCollection<int32_t>(2, VB3 + 4));
}

BOOST_AUTO_TEST_CASE(test08a)
{
    tstInts(newCollection<int32_t>(0, VB4 + 1));
}

BOOST_AUTO_TEST_CASE(test08b)
{
    tstInts(newCollection<int32_t>(1, VB4 + 1));
}

BOOST_AUTO_TEST_CASE(test08c)
{
    tstInts(newCollection<int32_t>(2, VB4 + 1));
}

BOOST_AUTO_TEST_CASE(test10)
{
    tstIllegalArgExc(newCollection<int32_t>(-1));
}

BOOST_AUTO_TEST_CASE(test11)
{
    tstIllegalArgExc(newCollection<int32_t>(1, 0));
}

BOOST_AUTO_TEST_CASE(test12)
{
    tstIllegalArgExc(newCollection<int32_t>(0, 1, 1, 2, 3, 5, 8, 0));
}

BOOST_AUTO_TEST_SUITE_END()
