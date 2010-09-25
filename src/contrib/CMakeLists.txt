project(lucene++-contrib)

####################################
# THE lucene++-contrib library
####################################
file(GLOB_RECURSE lucene_sources
  ${lucene++-contrib_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${lucene++-contrib_SOURCE_DIR}/*.h)

ADD_LIBRARY(lucene++-contrib SHARED EXCLUDE_FROM_ALL
	${lucene_sources} ${HEADERS}
)
ADD_DEFINITIONS(-DLPP_BUILDING_LIB)
ADD_DEFINITIONS(-DLPP_HAVE_GXXCLASSVISIBILITY)

#set properties on the libraries
SET_TARGET_PROPERTIES(lucene++-contrib PROPERTIES
    VERSION ${LUCENE++_VERSION}
    SOVERSION ${LUCENE++_SOVERSION}
)
TARGET_LINK_LIBRARIES(lucene++-contrib ${CMAKE_THREAD_LIBS_INIT})

