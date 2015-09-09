##
## Copyright 2011-2014 Centreon
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/compression/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/compression/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/compression/test")
include_directories("${INC_DIR}")

# Compression module.
set(COMPRESSION "60-compression")
set(COMPRESSION "${COMPRESSION}" PARENT_SCOPE)
add_library("${COMPRESSION}" SHARED
  # Sources.
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/opener.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/compression/factory.hh"
  "${INC_DIR}/com/centreon/broker/compression/opener.hh"
  "${INC_DIR}/com/centreon/broker/compression/stream.hh"
)
target_link_libraries("${COMPRESSION}" ${QT_LIBRARIES})
set_target_properties("${COMPRESSION}" PROPERTIES PREFIX "")
get_property(COMPRESSION_MODULE_PATH
  TARGET "${COMPRESSION}"
  PROPERTY LOCATION)
set(COMPRESSION_MODULE_PATH "${COMPRESSION_MODULE_PATH}" PARENT_SCOPE)

if (WITH_TESTING)
  include_directories("${PROJECT_SOURCE_DIR}/file/inc")

  # stream tests.
  #   Compress/uncompress.
  set(TEST_NAME "compression_stream_io")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/stream/io.cc")
  target_link_libraries("${TEST_NAME}"
    "${FILE}"
    "${COMPRESSION}"
    "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Buffer is read back.
  set(TEST_NAME "compression_buffer_is_read_back")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/stream/buffer_is_read_back.cc")
  target_link_libraries("${TEST_NAME}"
    "${FILE}"
    "${COMPRESSION}"
    "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
endif ()

# Install rule.
install(TARGETS "${COMPRESSION}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
