##
## Copyright 2011 Centreon
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/ndo/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/ndo/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/ndo/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/bam/inc")
include_directories("${PROJECT_SOURCE_DIR}/correlation/inc")
include_directories("${PROJECT_SOURCE_DIR}/dumper/inc")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")

# Some files must be processed by Qt's moc.
unset(QT_WRAPPED_SOURCES)
qt4_wrap_cpp(QT_WRAPPED_SOURCES
  "${INC_DIR}/com/centreon/broker/ndo/acceptor.hh")

# NDO module.
set(NDO "80-ndo")
set(NDO "${NDO}" PARENT_SCOPE)
add_library("${NDO}" SHARED
  # Sources.
  "${SRC_DIR}/acceptor.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/input.cc"
  "${SRC_DIR}/internal.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/output.cc"
  "${SRC_DIR}/stream.cc"
  "${SRC_DIR}/temp_mapping.cc"
  "${PROJECT_SOURCE_DIR}/core/src/mapping.cc"
  ${QT_WRAPPED_SOURCES}
  # Headers.
  "${INC_DIR}/com/centreon/broker/ndo/acceptor.hh"
  "${INC_DIR}/com/centreon/broker/ndo/connector.hh"
  "${INC_DIR}/com/centreon/broker/ndo/factory.hh"
  "${INC_DIR}/com/centreon/broker/ndo/input.hh"
  "${INC_DIR}/com/centreon/broker/ndo/internal.hh"
  "${INC_DIR}/com/centreon/broker/ndo/output.hh"
  "${INC_DIR}/com/centreon/broker/ndo/stream.hh"
)
target_link_libraries("${NDO}" ${QT_LIBRARIES})
set_target_properties("${NDO}" PROPERTIES PREFIX "")
get_property(NDO_MODULE_PATH
  TARGET "${NDO}"
  PROPERTY LOCATION)
set(NDO_MODULE_PATH "${NDO_MODULE_PATH}" PARENT_SCOPE)

# Testing.
if (WITH_TESTING)
  include_directories("${PROJECT_SOURCE_DIR}/file/inc")
  include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
  set(TEST_LIBRARIES "${NDO}" "${BAM}" "${STORAGE}" "${NEB}" "${CORRELATION}" "roker")

  # stream tests.
  #   Parse erased input.
  set(TEST_NAME "ndo_stream_erased_input")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/stream/erased_input.cc")
  target_link_libraries("${TEST_NAME}" "${FILE}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
endif ()

# Install rule.
install(TARGETS "${NDO}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
