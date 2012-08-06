# source files for Test

SET(TEST_CPPUTIL_SRCS
main.cpp
)

IF(WIN32)
# Lib directories
ELSEIF(UNIX)
# Lib directories
LINK_DIRECTORIES(
/usr/local/lib
/usr/lib
)
ENDIF(WIN32)

ADD_EXECUTABLE(TestCppUtil ${TEST_CPPUTIL_SRCS} ${TEST_CPPUTIL_HEADERS})

IF(WIN32)
# Do windows specific includes
TARGET_LINK_LIBRARIES (
TestCppUtil
glog
#boost_chrono boost_date_time boost_filesystem boost_regex boost_signals boost_system boost_thread boost_unit_test_framework
)
ELSEIF(UNIX)
# Do linux specific includes
TARGET_LINK_LIBRARIES (
TestCppUtil
glog
# Do linux specific includes
boost_chrono boost_date_time boost_filesystem boost_regex boost_signals boost_system boost_thread boost_unit_test_framework
)
ENDIF(WIN32)
 
install(TARGETS TestCppUtil DESTINATION cpp-util_SOURCE_DIR}/../test})

