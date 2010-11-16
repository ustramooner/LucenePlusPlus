/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2010 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#pragma warning(disable:4005)
#pragma warning(disable:4996)
#endif

#if defined(_WIN32) || defined(_WIN64)
#define LPP_IMPORT __declspec(dllimport)
#define LPP_EXPORT __declspec(dllexport)
#else
#ifdef LPP_HAVE_GXXCLASSVISIBILITY
#define LPP_IMPORT __attribute__ ((visibility("default")))
#define LPP_EXPORT __attribute__ ((visibility("default")))
#else
#define LPP_IMPORT
#define LPP_EXPORT
#endif
#endif

// Define LPPAPI for dll builds
#if defined(LPP_HAVE_DLL)
  #ifdef LPP_BUILDING_LIB
  #define LPPAPI LPP_EXPORT
  #else
  #define LPPAPI LPP_IMPORT
  #endif
  #else
  #define LPPAPI
#endif
#if defined(LPP_HAVE_DLL)
  #ifdef LPP_BUILDING_LIB
  #define LPPCONTRIBAPI LPP_EXPORT
  #else
  #define LPPCONTRIBAPI LPP_IMPORT
  #endif
  #else
  #define LPPCONTRIBAPI
#endif

// Check windows
#if defined(_WIN32)
#define LPP_UNICODE_CHAR_SIZE_2
#if defined(_WIN64)
#define LPP_BUILD_64
#else
#define LPP_BUILD_32
#endif
#endif

// Check GCC
#if defined(__GNUC__)
#define LPP_UNICODE_CHAR_SIZE_4
#if defined(__x86_64__) || defined(__ppc64__)
#define LPP_BUILD_64
#else
#define LPP_BUILD_32
#endif
#endif

// Default to 32-bit platforms
#if !defined(LPP_BUILD_32) && !defined(LPP_BUILD_64)
#define LPP_BUILD_32
#endif

// Default to 4-byte unicode format
#if !defined(LPP_UNICODE_CHAR_SIZE_2) && !defined(LPP_UNICODE_CHAR_SIZE_4)
#define LPP_UNICODE_CHAR_SIZE_4
#endif

// Define to disable cyclic checking in debug builds
// #define LPP_NO_CYCLIC_CHECK

#if defined(_DEBUG) && !defined(LPP_NO_CYCLIC_CHECK)
#define LPP_USE_CYCLIC_CHECK
#endif

// Define to use nedmalloc memory allocator
// #define LPP_USE_NEDMALLOC

#ifdef LPP_USE_NEDMALLOC
#define EXTSPEC LPPAPI
#endif

// Make internal bitset storage public
#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS

//use tr1 for hash map & hash set
//#define USE_TR1
#ifdef USE_TR1
	#define USE_TR1_PREFIX std::tr1
#else
	#define USE_TR1_PREFIX boost
#endif

