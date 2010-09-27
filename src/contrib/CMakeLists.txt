project(lucene++-contrib)

####################################
# THE lucene++-contrib library
####################################
file(GLOB_RECURSE lucene_sources
  ${lucene++-contrib_SOURCE_DIR}/*.cpp
  ${lucene++-contrib_SOURCE_DIR}/snowball/libstemmer_c/libstemmer/*.c
  ${lucene++-contrib_SOURCE_DIR}/snowball/libstemmer_c/src_c/*.c
  ${lucene++-contrib_SOURCE_DIR}/snowball/libstemmer_c/runtime/*.c)
file(GLOB_RECURSE HEADERS ${lucene++-contrib_SOURCE_DIR}/*.h)

ADD_DEFINITIONS(-DLPP_BUILDING_LIB)
INCLUDE_DIRECTORIES(${lucene++-lib_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${lucene++-contrib_SOURCE_DIR}/include)


#################################
# lucene++ shared library
#################################
ADD_LIBRARY(lucene++-contrib SHARED EXCLUDE_FROM_ALL
	${lucene_sources} ${HEADERS}
)
#set properties on the libraries
SET_TARGET_PROPERTIES(lucene++-contrib PROPERTIES
    VERSION ${LUCENE++_VERSION}
    SOVERSION ${LUCENE++_SOVERSION}
)
TARGET_LINK_LIBRARIES(lucene++-contrib 
  ${CMAKE_THREAD_LIBS_INIT})

#################################
# lucene++ static library
#################################
ADD_LIBRARY(lucene++-contrib-static STATIC EXCLUDE_FROM_ALL
	${lucene_sources} ${HEADERS}
)
#set properties on the libraries
SET_TARGET_PROPERTIES(lucene++-contrib-static PROPERTIES
    VERSION ${LUCENE++_VERSION}
    SOVERSION ${LUCENE++_SOVERSION}
)

