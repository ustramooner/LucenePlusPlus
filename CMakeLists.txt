project(lucene++)

####################################
# VERSION information
#Rules for version:
#MAJOR and MINOR versions are purely political
#REVISION version MUST be revised if the headers or compatibility change
#PATCH should be 0 unless a patch is made that doesn't affect the public signature (i.e. clients don't need to re-compile).
SET(LUCENE++_VERSION_MAJOR "3")
SET(LUCENE++_VERSION_MINOR "0")
SET(LUCENE++_VERSION_REVISION "2")
SET(LUCENE++_VERSION_PATCH "0")

#derived versions
MATH(EXPR LUCENE++_INT_VERSION "(${LUCENE++_VERSION_MAJOR} * 1000000) + (${LUCENE++_VERSION_MINOR} * 10000) + (${LUCENE++_VERSION_REVISION} * 100) + (${LUCENE++_VERSION_PATCH})" )
SET(LUCENE++_VERSION "${LUCENE++_VERSION_MAJOR}.${LUCENE++_VERSION_MINOR}.${LUCENE++_VERSION_REVISION}.${LUCENE++_VERSION_PATCH}")
SET(LUCENE++_SOVERSION "${LUCENE++_VERSION_MAJOR}.${LUCENE++_VERSION_MINOR}.${LUCENE++_VERSION_REVISION}")
####################################

####################################
# Build system options and includes
####################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)
#build policies
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# include specific modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

#define options...
INCLUDE (Lucene++Docs)
INCLUDE (FindThreads)
INCLUDE (TestCXXAcceptsFlag)

#Single output directory for building all executables and libraries.
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Executable Output Directory" FORCE)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Library Output Directory" FORCE)
####################################


####################################
#user specified build options
####################################
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ELSE(NOT CMAKE_BUILD_TYPE)
    MESSAGE( "Compiling as ${CMAKE_BUILD_TYPE}" )
ENDIF(NOT CMAKE_BUILD_TYPE)

SET ( ENABLE_COMPILE_TESTS_VALUE ON )
IF ( MSVC_IDE )
	#this is annoying...
	SET ( ENABLE_COMPILE_TESTS_VALUE OFF )
ENDIF( MSVC_IDE )
OPTION(ENABLE_COMPILE_TESTS
  "enable various projects that test alternative build switches"
  ${ENABLE_COMPILE_TESTS_VALUE})

OPTION(ENABLE_PACKAGING
  "create build scripts for creating lucene++ packages"
  OFF)
#install path options
SET(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
SET(LIB_DESTINATION "lib${LIB_SUFFIX}")
####################################


####################################
# PLATFORM specific options
####################################
#add a debug build postfix
if(WIN32)
 set(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

IF(CYGWIN)
  ADD_DEFINITIONS(-D__LARGE64_FILES)
ENDIF(CYGWIN)

#set ansi mode 
SET(ENABLE_ANSI_MODE OFF)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(ENABLE_ANSI_MODE ON)
  
  #exceptions:
  IF(MINGW OR CYGWIN)
    SET(ENABLE_ANSI_MODE OFF)
  ENDIF(MINGW OR CYGWIN)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
IF ( CMAKE_COMPILER_IS_GNUCC )
   IF( ENABLE_ANSI_MODE )
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi")
   ENDIF ( ENABLE_ANSI_MODE )
ENDIF(CMAKE_COMPILER_IS_GNUCC) 

####################################
#find boost
####################################
SET(BOOST_USE_DATE_TIME ON) #     Link against the date_time Library
SET(BOOST_USE_FILESYSTEM ON) #     Link against the filesystem Library
SET(BOOST_USE_IOSTREAMS ON) #     Link against the iostreams Library
SET(BOOST_USE_REGEX ON) #     Link against the regex Library
SET(BOOST_USE_THREAD ON) #     Link against the thread Library
SET(BOOST_USE_UNIT_TEST_FRAMEWORK ON) #     Link against the  unit_test_framework Library
SET(BOOST_LIBRARIES_SUFFIX "")
SET(BOOST_CURRENT_VERSION "")
INCLUDE(MXAFindBoost)

IF ( NOT BOOST_DATE_TIME_FOUND OR
     NOT BOOST_FILESYSTEM_FOUND OR
     NOT BOOST_IOSTREAMS_FOUND OR
     NOT BOOST_REGEX_FOUND OR
     NOT BOOST_THREAD_FOUND OR
     NOT BOOST_UNIT_TEST_FRAMEWORK_FOUND )
     MESSAGE(FATAL_ERROR "A required boost library was not found")
ENDIF ( NOT BOOST_DATE_TIME_FOUND OR
     NOT BOOST_FILESYSTEM_FOUND OR
     NOT BOOST_IOSTREAMS_FOUND OR
     NOT BOOST_REGEX_FOUND OR
     NOT BOOST_THREAD_FOUND OR
     NOT BOOST_UNIT_TEST_FRAMEWORK_FOUND )

#set global include paths
INCLUDE_DIRECTORIES(${lucene++_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${lucene++_SOURCE_DIR}/includesrc)
INCLUDE_DIRECTORIES(${lucene++_SOURCE_DIR}/util/md5)
INCLUDE_DIRECTORIES(${lucene++_SOURCE_DIR}/util/nedmalloc)
INCLUDE_DIRECTORIES(${lucene++_SOURCE_DIR}/util/zlib)
INCLUDE_DIRECTORIES(${lucene++_SOURCE_DIR}/util/utf8)
INCLUDE_DIRECTORIES(${lucene++_SOURCE_DIR}/util/ucs)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )

#include sub-projects
ADD_SUBDIRECTORY (lib)
ADD_SUBDIRECTORY (contrib EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY (demo EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY (test)

####################################
# Custom targets
####################################
#add uninstall command
CONFIGURE_FILE(
  "${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")


####################################
# Finalise build script
####################################

#this must go last...
IF (ENABLE_PACKAGING)
  INCLUDE(CreateLucene++Packages)
ENDIF ( ENABLE_PACKAGING)
