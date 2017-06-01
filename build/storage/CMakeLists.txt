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
set(INC_DIR "${PROJECT_SOURCE_DIR}/storage/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/storage/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/storage/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/rrd/inc")
set(INC_DIR "${INC_DIR}/com/centreon/broker/storage")

# Storage module.
set(STORAGE "20-storage")
set(STORAGE "${STORAGE}" PARENT_SCOPE)
add_library("${STORAGE}" SHARED
  # Sources.
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/exceptions/perfdata.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/index_mapping.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/metric.cc"
  "${SRC_DIR}/metric_mapping.cc"
  "${SRC_DIR}/parser.cc"
  "${SRC_DIR}/perfdata.cc"
  "${SRC_DIR}/rebuild.cc"
  "${SRC_DIR}/rebuilder.cc"
  "${SRC_DIR}/remove_graph.cc"
  "${SRC_DIR}/status.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/events.hh"
  "${INC_DIR}/exceptions/perfdata.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/index_mapping.hh"
  "${INC_DIR}/internal.hh"
  "${INC_DIR}/metric.hh"
  "${INC_DIR}/metric_mapping.hh"
  "${INC_DIR}/parser.hh"
  "${INC_DIR}/perfdata.hh"
  "${INC_DIR}/rebuild.hh"
  "${INC_DIR}/rebuilder.hh"
  "${INC_DIR}/remove_graph.hh"
  "${INC_DIR}/status.hh"
  "${INC_DIR}/stream.hh"
)
target_link_libraries("${STORAGE}" ${QT_LIBRARIES})
set_target_properties("${STORAGE}" PROPERTIES PREFIX "")

# Testing.
set(
  TESTS_SOURCES
  ${TESTS_SOURCES}
  "${TEST_DIR}/parser/parse_perfdata.cc"
  PARENT_SCOPE
)
set(
  TESTS_LIBRARIES
  ${TESTS_LIBRARIES}
  "${STORAGE}"
  PARENT_SCOPE
)

if (0) # WITH_TESTING)
  set(TEST_LIBRARIES "${STORAGE}" "10-neb" "rokerbase" "roker" "rokerbase")

  # Perfdata exception tests.
  #   Throw.
  set(TEST_NAME "storage_exceptions_perfdata_throw")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/exceptions/perfdata/throw.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Rethrow.
  set(TEST_NAME "storage_exceptions_perfdata_rethrow")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/exceptions/perfdata/rethrow.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Clone.
  set(TEST_NAME "storage_exceptions_perfdata_clone")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/exceptions/perfdata/clone.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Perfdata object tests.
  #   Default construction.
  set(TEST_NAME "storage_perfdata_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/perfdata/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "storage_perfdata_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/perfdata/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "storage_perfdata_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/perfdata/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Metric event tests.
  #   Default construction.
  set(TEST_NAME "storage_metric_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/metric/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "storage_metric_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/metric/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "storage_metric_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/metric/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Rebuild event tests.
  #   Default construction.
  set(TEST_NAME "storage_rebuild_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/rebuild/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "storage_rebuild_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/rebuild/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "storage_rebuild_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/rebuild/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Remove graph event tests.
  #   Default construction.
  set(TEST_NAME "storage_remove_graph_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/remove_graph/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "storage_remove_graph_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/remove_graph/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "storage_remove_graph_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/remove_graph/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Status event tests.
  #   Default construction.
  set(TEST_NAME "storage_status_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/status/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "storage_status_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/status/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "storage_status_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/status/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Parser tests.
  #   Simple perfdata parsing.
  set(TEST_NAME "storage_parser_parse_simple")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/parser/parse_simple.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Complex perfdata parsing.
  set(TEST_NAME "storage_parser_parse_complex")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/parser/parse_complex.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Incorrect perfdata parsing.
  set(TEST_NAME "storage_parser_parse_incorrect")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/parser/parse_incorrect.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Loop perfdata parsing.
  set(TEST_NAME "storage_parser_parse_loop")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/parser/parse_loop.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
endif ()

# Install rule.
install(TARGETS "${STORAGE}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
