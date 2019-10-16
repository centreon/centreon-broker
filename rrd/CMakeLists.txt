##
## Copyright 2011-2013 Centreon
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## For more information : contact@centreon.com
##

# Global options.
set(INC_DIR "${PROJECT_SOURCE_DIR}/rrd/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/rrd/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/rrd/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")

# Find librrd.
include(FindPkgConfig)
if (PKG_CONFIG_FOUND)
  pkg_check_modules(LIBRRD REQUIRED "librrd")
  if (LIBRRD_INCLUDE_DIRS)
    include_directories("${LIBRRD_INCLUDE_DIRS}")
    set(LIBRRD_INCLUDE_DIRS "${LIBRRD_INCLUDE_DIRS}" PARENT_SCOPE)
  endif ()
else ()
  # Find rrd.h
  find_path(LIBRRD_INCLUDE_DIR "rrd.h")
  if (NOT LIBRRD_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find librrd's headers.")
  endif ()
  include_directories("${LIBRRD_INCLUDE_DIR}")
  set(LIBRRD_INCLUDE_DIRS "${LIBRRD_INCLUDE_DIR}" PARENT_SCOPE)

  # Find librrd.
  find_library(LIBRRD_LDFLAGS "rrd")
  if (NOT LIBRRD_LDFLAGS)
    message(FATAL_ERROR "Could not find librrd's library.")
  endif ()
  set(LIBRRD_LDFLAGS "${LIBRRD_LDFLAGS}" PARENT_SCOPE)
endif ()

# RRD module.
set(RRD "70-rrd")
set(RRD "${RRD}" PARENT_SCOPE)
add_library("${RRD}" SHARED
  # Sources.
  "${SRC_DIR}/backend.cc"
  "${SRC_DIR}/cached.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/creator.cc"
  "${SRC_DIR}/exceptions/open.cc"
  "${SRC_DIR}/exceptions/update.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/lib.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/output.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/rrd/backend.hh"
  "${INC_DIR}/com/centreon/broker/rrd/cached.hh"
  "${INC_DIR}/com/centreon/broker/rrd/connector.hh"
  "${INC_DIR}/com/centreon/broker/rrd/creator.hh"
  "${INC_DIR}/com/centreon/broker/rrd/exceptions/open.hh"
  "${INC_DIR}/com/centreon/broker/rrd/exceptions/update.hh"
  "${INC_DIR}/com/centreon/broker/rrd/factory.hh"
  "${INC_DIR}/com/centreon/broker/rrd/lib.hh"
  "${INC_DIR}/com/centreon/broker/rrd/output.hh"
  )
add_dependencies("${RRD}" Asio)
set_target_properties("${RRD}" PROPERTIES PREFIX "")

# Compile with librrd flags.
if (LIBRRD_CFLAGS)
  get_source_file_property(RRD_SOURCE_CXXFLAGS "${SRC_DIR}/lib.cc" COMPILE_FLAGS)
  if (LIBRRD_SOURCE_CXXFLAGS)
    set_source_files_properties("${SRC_DIR}/lib.cc" PROPERTIES
      COMPILE_FLAGS "${RRD_SOURCE_CXXFLAGS} ${LIBRRD_CFLAGS}")
  else ()
    set_source_files_properties("${SRC_DIR}/lib.cc" PROPERTIES
      COMPILE_FLAGS "${LIBRRD_CFLAGS}")
  endif ()
endif ()
target_link_libraries("${RRD}" ${LIBRRD_LDFLAGS})

# Testing.
if (WITH_TESTING)
  set(
    TESTS_SOURCES
    ${TESTS_SOURCES}
    "${TEST_DIR}/cached.cc"
    "${TEST_DIR}/exceptions.cc"
    "${TEST_DIR}/factory.cc"
    "${TEST_DIR}/lib.cc"
    "${TEST_DIR}/rrd.cc"
    PARENT_SCOPE
  )
  set(
    TESTS_LIBRARIES
    ${TESTS_LIBRARIES}
    "${RRD}"
    PARENT_SCOPE
  )
endif ()

# Install rule.
install(TARGETS "${RRD}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
  )
