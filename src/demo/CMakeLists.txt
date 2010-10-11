project(lucene++-demo)

####################################
# THE lucene++demo applications
####################################
file(GLOB_RECURSE HEADERS ${lucene++-demo_SOURCE_DIR}/../include/*.h)
ADD_DEFINITIONS(-DLPP_HAVE_DLL)
ADD_EXECUTABLE(indexfiles EXCLUDE_FROM_ALL
	${lucene++-demo_SOURCE_DIR}/indexfiles/main.cpp ${HEADERS}
)
TARGET_LINK_LIBRARIES(indexfiles lucene++)

ADD_EXECUTABLE(searchfiles EXCLUDE_FROM_ALL
	${lucene++-demo_SOURCE_DIR}/searchfiles/main.cpp ${HEADERS}
)
TARGET_LINK_LIBRARIES(searchfiles lucene++)

ADD_EXECUTABLE(deletefiles EXCLUDE_FROM_ALL
	${lucene++-demo_SOURCE_DIR}/deletefiles/main.cpp ${HEADERS}
)
TARGET_LINK_LIBRARIES(deletefiles lucene++)

ADD_CUSTOM_TARGET(
   demo
   DEPENDS indexfiles searchfiles deletefiles
)

