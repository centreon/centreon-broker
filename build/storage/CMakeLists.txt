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
set(INC_DIR "${PROJECT_SOURCE_DIR}/storage/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/storage/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/storage/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/rrd/inc")

# Storage module.
set(STORAGE "20-storage")
set(STORAGE "${STORAGE}" PARENT_SCOPE)
add_library("${STORAGE}" SHARED
  # Sources.
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/exceptions/perfdata.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/metric.cc"
  "${SRC_DIR}/parser.cc"
  "${SRC_DIR}/perfdata.cc"
  "${SRC_DIR}/rebuild.cc"
  "${SRC_DIR}/rebuilder.cc"
  "${SRC_DIR}/remove_graph.cc"
  "${SRC_DIR}/status.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/storage/connector.hh"
  "${INC_DIR}/com/centreon/broker/storage/exceptions/perfdata.hh"
  "${INC_DIR}/com/centreon/broker/storage/factory.hh"
  "${INC_DIR}/com/centreon/broker/storage/metric.hh"
  "${INC_DIR}/com/centreon/broker/storage/parser.hh"
  "${INC_DIR}/com/centreon/broker/storage/perfdata.hh"
  "${INC_DIR}/com/centreon/broker/storage/rebuild.hh"
  "${INC_DIR}/com/centreon/broker/storage/rebuilder.hh"
  "${INC_DIR}/com/centreon/broker/storage/remove_graph.hh"
  "${INC_DIR}/com/centreon/broker/storage/status.hh"
  "${INC_DIR}/com/centreon/broker/storage/stream.hh"
)
target_link_libraries("${STORAGE}" ${QT_LIBRARIES})
set_target_properties("${STORAGE}" PROPERTIES PREFIX "")

if (WITH_TESTING)
  set(TEST_LIBRARIES "${STORAGE}" "roker")

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
