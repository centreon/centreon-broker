##
## Copyright 2011 Merethis
##
## This file is part of Centreon Broker.
##
## Centreon Broker is free software: you can redistribute it and/or
## modify it under the terms of the GNU General Public License version 2
## as published by the Free Software Foundation.
##
## Centreon Broker is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Centreon Broker. If not, see
## <http://www.gnu.org/licenses/>.
##

# Global options.
set(INC_DIR "${PROJECT_SOURCE_DIR}/file/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/file/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/file/test")
include_directories("${INC_DIR}")

# File module.
set(FILE "50-file")
set(FILE "${FILE}" PARENT_SCOPE)
add_library("${FILE}" SHARED
  # Sources.
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/opener.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/file/factory.hh"
  "${INC_DIR}/com/centreon/broker/file/opener.hh"
  "${INC_DIR}/com/centreon/broker/file/stream.hh"
)
target_link_libraries("${FILE}" ${QT_LIBRARIES})
set_target_properties("${FILE}" PROPERTIES PREFIX "")

if (WITH_TESTING)
  set(TEST_LIBRARIES "${FILE}" "roker")

  # stream tests.
  #   Writing.
  set(TEST_NAME "file_stream_write")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/stream/write.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Reading.
  set(TEST_NAME "file_stream_read")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/stream/read.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Mixed reads/writes.
  set(TEST_NAME "file_stream_mixed")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/stream/mixed.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
endif ()

# Install rule.
install(TARGETS "${FILE}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
