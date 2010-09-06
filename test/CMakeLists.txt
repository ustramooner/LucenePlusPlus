project(lucene++-tester)

####################################
# THE lucene++tester library
####################################
file(GLOB_RECURSE lucene_sources
  ${lucene++-tester_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${lucene++-tester_SOURCE_DIR}/../include/*.h)
file(GLOB_RECURSE HEADERS ${lucene++-tester_SOURCE_DIR}/include/*.h)

INCLUDE_DIRECTORIES(${lucene++-tester_SOURCE_DIR}/include)

ADD_EXECUTABLE(lucene++-tester EXCLUDE_FROM_ALL
	${lucene_sources} ${HEADERS}
)
ADD_DEFINITIONS(-DLPP_HAVE_GXXCLASSVISIBILITY)

#set properties on the libraries
SET_TARGET_PROPERTIES(lucene++-tester PROPERTIES
    VERSION ${LUCENE++_VERSION}
    SOVERSION ${LUCENE++_SOVERSION}
)
TARGET_LINK_LIBRARIES(lucene++-tester lucene++ lucene++-contrib ${CMAKE_THREAD_LIBS_INIT})

ENABLE_TESTING()
ADD_TEST(Lucene++Tester lucene++-tester)

