/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <iostream>
#include "TestUtils.h"
#include "FileUtils.h"
#include "StringUtils.h"
#include "MiscUtils.h"

#define BOOST_TEST_MODULE "Lucene"
#define BOOST_TEST_NO_MAIN

#include <boost/test/included/unit_test.hpp>
#include <boost/algorithm/string.hpp>

using namespace Lucene;

int main(int argc, char* argv[])
{
    String testDir;
    uint64_t startTime = MiscUtils::currentTimeMillis();
    
    for (int32_t i = 0; i < argc; ++i)
    {
        if (strncmp(argv[i], "--test_dir", 9) == 0)
        {
            String testParam = StringUtils::toUnicode(argv[i]);
            Collection<String> vals = StringUtils::split(testParam, L"=");
            if (vals.size() == 2)
            {
                testDir = vals[1];
                boost::replace_all(testDir, L"\"", L"");
                boost::trim(testDir);
                break;
            }			
        }
    }
    
    if (testDir.empty())
    {  
        testDir = L"../../test/testfiles";
        if (!FileUtils::isDirectory(testDir))
        {
            testDir = L"../test/testfiles";
            if (!FileUtils::isDirectory(testDir))
                testDir = L"./test/testfiles";
        }
    }
    
    if (!FileUtils::isDirectory(testDir))
    {
        std::wcout << L"Test directory not found. (override default by using --test_dir=\"./test/testfiles\")\n";
        return 1;
    }
    
    setTestDir(testDir);
    
    int testMain = boost::unit_test::unit_test_main(init_unit_test_suite, argc, argv);
    
    std::wcout << L"*** Test duration: " << (MiscUtils::currentTimeMillis() - startTime) / 1000 << L" sec\n";
    
    return testMain;
}