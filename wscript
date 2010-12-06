#############################################################################
## Copyright (c) 2009-2010 Alan Wright. All rights reserved.
## Distributable under the terms of either the Apache License (Version 2.0)
## or the GNU Lesser General Public License.
#############################################################################

import sys
import os
from copy import copy
import Options
from Configure import conf
from TaskGen import feature, after
import Task

APPNAME='Lucene++'
VERSION='3.0.2'

srcdir = '.'
blddir = 'bin'

source_patterns = [
    '*.c',
    '*.cpp'
]

lucene_source_dirs = [
  'src/core/analysis',
  'src/core/analysis/standard',
  'src/core/analysis/tokenattributes',
  'src/core/document',
  'src/core/index',
  'src/core/queryparser',
  'src/core/search',
  'src/core/search/function',
  'src/core/search/payloads',
  'src/core/search/spans',
  'src/core/store',
  'src/core/util',
  'src/core/util/md5',
  'src/core/util/nedmalloc',
  'src/core/util/unicode',
  'src/core/util/zlib',

]

lucene_contrib_source_dirs = [
    'src/contrib'
]

lucene_include_dirs = [
    'include',
    'src/core/include',
    'src/contrib/include',
    'src/contrib/snowball/libstemmer_c/include',
    'src/core/util/md5',
    'src/core/util/nedmalloc',
    'src/core/util/zlib',
    'src/core/util/unicode'
]

tester_source_dirs = [
  'src/test/',
  'src/test/analysis',
  'src/test/analysis/standard',
  'src/test/analysis/tokenattributes',
  'src/test/contrib',
  'src/test/contrib/analyzers',
  'src/test/contrib/analyzers/common',
  'src/test/contrib/analyzers/common/analysis',
  'src/test/contrib/analyzers/common/analysis/ar',
  'src/test/contrib/analyzers/common/analysis/br',
  'src/test/contrib/analyzers/common/analysis/cjk',
  'src/test/contrib/analyzers/common/analysis/cn',
  'src/test/contrib/analyzers/common/analysis/cz',
  'src/test/contrib/analyzers/common/analysis/de',
  'src/test/contrib/analyzers/common/analysis/el',
  'src/test/contrib/analyzers/common/analysis/fa',
  'src/test/contrib/analyzers/common/analysis/fr',
  'src/test/contrib/analyzers/common/analysis/nl',
  'src/test/contrib/analyzers/common/analysis/reverse',
  'src/test/contrib/analyzers/common/analysis/ru',
  'src/test/contrib/highlighter',
  'src/test/contrib/memory',
  'src/test/contrib/snowball',
  'src/test/document',
  'src/test/include',
  'src/test/index',
  'src/test/queryparser',
  'src/test/search',
  'src/test/search/function',
  'src/test/search/payloads',
  'src/test/search/spans',
  'src/test/store',
  'src/test/util'
]

tester_include_dirs = [
    'include',
    'src/core/include',
    'src/contrib/include',
    'src/test/include'
]


def set_options(opt):
    opt.tool_options("boost")
    opt.tool_options('compiler_cxx')
    opt.tool_options('clang', tooldir = 'build')
    opt.add_option(
        '--debug', 
        default = False,
        action = "store_true",
        help ='debug build no optimization, etc.', 
        dest = 'debug')
    
    opt.add_option(
        '--static', 
        default = False,
        action = "store_true",
        help ='fully static build', 
        dest = 'static')


def configure(conf):
    conf.check_tool('g++')
    conf.check_tool('gcc')
    conf.check_cc(lib = 'pthread', mandatory = True)
    conf.check_tool('boost')
    conf.check_tool('clang', 'build')
    conf.check_boost(
        static = 'onlystatic',
        lib = ['filesystem', 'thread', 'regex', 'system', 'date_time', 'iostreams', 'unit_test_framework']
    )

            
def build(bld):
    if Options.options.static :
      target_type = 'cstaticlib'
    else:
      target_type = 'cshlib'
    if Options.options.debug:
      debug_define = '_DEBUG'
    else:
      debug_define = 'NDEBUG'
    
    if Options.options.debug:
         compile_flags = ['-O0', '-g'] 
    else:
         compile_flags = ['-O2']
    
    lucene_sources = []
    for source_dir in lucene_source_dirs:
        source_dir = bld.path.find_dir(source_dir)
        lucene_sources.extend(source_dir.ant_glob(source_patterns))
    
    bld(
        name = 'lucene++',
        features = ['cxx', 'cc'] + [target_type],
        source = [source.relpath_gen(bld.path) for source in lucene_sources],
        target = 'lucene++',
        includes = lucene_include_dirs + bld.env["CPPPATH_BOOST"],
        ccflags = compile_flags,
        cxxflags = compile_flags,
        defines = ['LPP_BUILDING_LIB', 'LPP_HAVE_GXXCLASSVISIBILITY'] + [debug_define],
        uselib = 'BOOST_FILESYSTEM BOOST_THREAD BOOST_REGEX BOOST_SYSTEM BOOST_DATE_TIME BOOST_IOSTREAMS PTHREAD'
        )
    
    lucene_contrib_sources = []
    for source_dir in lucene_contrib_source_dirs:
        source_dir = bld.path.find_dir(source_dir)
        lucene_contrib_sources.extend(source_dir.ant_glob(source_patterns))
    
    bld(
        name = 'lucene_contrib',
        features = ['cxx', 'cc'] + [target_type],
        source = [source.relpath_gen(bld.path) for source in lucene_contrib_sources],
        target = 'lucene_contrib',
        includes = lucene_include_dirs + bld.env["CPPPATH_BOOST"],
        ccflags = compile_flags,
        cxxflags = compile_flags,
        defines = ['LPP_BUILDING_LIB', 'LPP_HAVE_GXXCLASSVISIBILITY'] + [debug_define],
        uselib = 'BOOST_FILESYSTEM BOOST_THREAD BOOST_REGEX BOOST_SYSTEM BOOST_DATE_TIME BOOST_IOSTREAMS PTHREAD',
        uselib_local = 'lucene++'
        )
    
    tester_sources = []
    for source_dir in tester_source_dirs:
        source_dir = bld.path.find_dir(source_dir)
        tester_sources.extend(source_dir.ant_glob(source_patterns))
    
    bld(
        name = 'lucene_tester',
        features = ['cxx', 'cc', 'cprogram'],
        source = [source.relpath_gen(bld.path) for source in tester_sources],
        target = 'lucene_tester',
        includes = tester_include_dirs + bld.env["CPPPATH_BOOST"],
        ccflags = compile_flags,
        cxxflags = compile_flags,
        defines = ['LPP_HAVE_GXXCLASSVISIBILITY'] + ['LPP_EXPOSE_INTERNAL'] + [debug_define],
        uselib = 'BOOST_FILESYSTEM BOOST_THREAD BOOST_REGEX BOOST_SYSTEM BOOST_DATE_TIME BOOST_IOSTREAMS BOOST_UNIT_TEST_FRAMEWORK PTHREAD',
        uselib_local = 'lucene++ lucene_contrib'
        )
    
    bld(
        name = 'deletefiles',
        features = ['cxx', 'cc', 'cprogram'],
        source = bld.path.find_resource('src/demo/deletefiles/main.cpp').relpath_gen(bld.path),
        target = 'deletefiles',
        includes = ['include'] + bld.env["CPPPATH_BOOST"],
        ccflags = compile_flags,
        cxxflags = compile_flags,
        defines = ['LPP_HAVE_GXXCLASSVISIBILITY'] + [debug_define],
        uselib = 'BOOST_FILESYSTEM BOOST_THREAD BOOST_REGEX BOOST_SYSTEM BOOST_DATE_TIME BOOST_IOSTREAMS PTHREAD',
        uselib_local = 'lucene++'
        )

    bld(
        name = 'indexfiles',
        features = ['cxx', 'cc', 'cprogram'],
        source = bld.path.find_resource('src/demo/indexfiles/main.cpp').relpath_gen(bld.path),
        target = 'indexfiles',
        includes = ['include'] + bld.env["CPPPATH_BOOST"],
        ccflags = compile_flags,
        cxxflags = compile_flags,
        defines = ['LPP_HAVE_GXXCLASSVISIBILITY'] + [debug_define],
        uselib = 'BOOST_FILESYSTEM BOOST_THREAD BOOST_REGEX BOOST_SYSTEM BOOST_DATE_TIME BOOST_IOSTREAMS PTHREAD',
        uselib_local = 'lucene++'
        )

    bld(
        name = 'searchfiles',
        features = ['cxx', 'cc', 'cprogram'],
        source = bld.path.find_resource('src/demo/searchfiles/main.cpp').relpath_gen(bld.path),
        target = 'searchfiles',
        includes = ['include'] + bld.env["CPPPATH_BOOST"],
        ccflags = compile_flags,
        cxxflags = compile_flags,
        defines = ['LPP_HAVE_GXXCLASSVISIBILITY'] + [debug_define],
        uselib = 'BOOST_FILESYSTEM BOOST_THREAD BOOST_REGEX BOOST_SYSTEM BOOST_DATE_TIME BOOST_IOSTREAMS PTHREAD',
        uselib_local = 'lucene++'
        )
