PROJECT (lucene++-lib)

####################################
# THE lucene++ library
####################################
file(GLOB_RECURSE lucene_sources
  ${lucene++-lib_SOURCE_DIR}/search/*.cpp
  ${lucene++-lib_SOURCE_DIR}/analysis/*.cpp
  ${lucene++-lib_SOURCE_DIR}/document/*.cpp
  ${lucene++-lib_SOURCE_DIR}/index/*.cpp
  ${lucene++-lib_SOURCE_DIR}/queryparser/*.cpp
  ${lucene++-lib_SOURCE_DIR}/store/*.cpp
  ${lucene++-lib_SOURCE_DIR}/util/*.cpp)

file(GLOB_RECURSE HEADERS ${lucene++-base_SOURCE_DIR}/include/*.h ${lucene++-libs_SOURCE_DIR}/include/*.h)

#C sources...
file(GLOB_RECURSE lucene_c_sources
  ${lucene++-lib_SOURCE_DIR}/util/*.c)

  
IF ( ENABLE_NEDMALLOC )
  ADD_DEFINITIONS(-DLPP_USE_NEDMALLOC)
ENDIF()

INCLUDE_DIRECTORIES(${lucene++-lib_SOURCE_DIR}/include)
ADD_DEFINITIONS(-DLPP_BUILDING_LIB)

ADD_LIBRARY(lucene++-c STATIC
	${lucene_c_sources}
)


#################################
# lucene++ shared library
#################################
ADD_LIBRARY(lucene++ SHARED EXCLUDE_FROM_ALL
	${lucene_sources} ${HEADERS}
)
#set properties on the libraries
SET_TARGET_PROPERTIES(lucene++ PROPERTIES
    VERSION ${LUCENE++_VERSION}
    SOVERSION ${LUCENE++_SOVERSION}
)
TARGET_LINK_LIBRARIES(lucene++ ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})

#################################
# lucene++ static library
#################################
ADD_LIBRARY(lucene++-static STATIC EXCLUDE_FROM_ALL
	${lucene_sources} ${HEADERS}
)
#set properties on the libraries
SET_TARGET_PROPERTIES(lucene++-static PROPERTIES
    VERSION ${LUCENE++_VERSION}
    SOVERSION ${LUCENE++_SOVERSION}
)

