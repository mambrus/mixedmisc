cmake_minimum_required (VERSION 2.6)
project (posixtest C)
INCLUDE (CheckLibraryExists)

################################################################################
# Configuration options
################################################################################

# This option is needed when building for other system than default for the
# compiler.  This is usually the case for X-builds. I.e. this enables
# Android NDK arm-gcc to be used also for chrome-book, rPI e.t.a.
set(SYSROOT
	${DETECTED_SYSROOT}
	CACHE PATH
	"System path (--system=)")

################################################################################
if (NOT SYSROOT STREQUAL "")
	message( "** INFO: SYSROOT was either set or defaulted from toolchain file" )
	set(CMAKE_SYSROOT "${SYSROOT}")
	set(CMAKE_STAGING_PREFIX /tmp/cmake_stage/Android)
	set(CMAKE_EXTRA_C_FLAGS "${CMAKE_EXTRA_C_FLAGS} --sysroot=${CMAKE_SYSROOT}")
endif (NOT SYSROOT STREQUAL "")

set(CMAKE_C_FLAGS "${CMAKE_EXTRA_C_FLAGS} -no-integrated-cpp -Wno-unused-function -g3 -ggdb3 -O0 -Wall")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_EXTRA_C_FLAGS} -no-integrated-cpp -Wno-unused-function -g3 -ggdb3 -O0 -Wall")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_EXTRA_C_FLAGS} -no-integrated-cpp -Wno-unused-function -g3 -ggdb3 -O0 -Wall")


################################################################################
# Libraries (src binding)
################################################################################


################################################################################
# Project
################################################################################
# The version number.
set (posixtest_VERSION_MAJOR 0)
set (posixtest_VERSION_MINOR 0)
set (posixtest_VERSION_PATCH 1)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

set(POSIXTEST_SOURCE
	concurrent_sort.c
)

install(PROGRAMS ${PROJECT_BINARY_DIR}/posixtest DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

add_executable(posixtest ${POSIXTEST_SOURCE})
target_link_libraries (posixtest ${EXTRA_LIBS})

################################################################################
# External libraries
################################################################################
CHECK_LIBRARY_EXISTS(rt clock_gettime "" HAVE_LIB_RT)
if (HAVE_LIB_RT)
    target_link_libraries(posixtest rt)
endif (HAVE_LIB_RT)

