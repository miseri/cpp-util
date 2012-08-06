cmake_minimum_required(VERSION 2.6)

PROJECT( cpp-util )

message("Source Dir:" ${cpp-util_SOURCE_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${cpp-util_SOURCE_DIR}/../bin)
SET(LIBRARY_OUTPUT_PATH ${cpp-util_SOURCE_DIR}/../lib)
SET(RUNTIME_OUTPUT_DIRECTORY ${cpp-util_SOURCE_DIR}/../bin)

SET(CPPUTIL_BIN ${cpp-util_SOURCE_DIR}/../bin)

message("cpp-util_SOURCE_DIR directories:" ${cpp-util_SOURCE_DIR})

# definitions
IF (UNIX)
add_definitions( -std=c++0x )
ENDIF (UNIX)

SET(CppUtilIncludes 
${cpp-util_SOURCE_DIR}/../include
)
message("CppUtilLibIncludes directories:" ${CppUtilIncludes})

SET(LibDir 
${CppUtil_SOURCE_DIR}/../lib
$(BOOST_LIB_DIR) 
)
message("LibDir directories:" ${LibDir})

# Include directories
INCLUDE_DIRECTORIES(
${cpp-util_SOURCE_DIR}/../include
)

ADD_SUBDIRECTORY( test )

