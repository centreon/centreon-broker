##
## Copyright 2011-2013 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/bam/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/bam/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/bam/test")
include_directories("${INC_DIR}")
set(INC_DIR "${INC_DIR}/com/centreon/broker/bam")

# Bam module.
set(BAM "20-bam")
set(BAM "${BAM}" PARENT_SCOPE)
add_library("${BAM}" SHARED
  # Sources.
  "${SRC_DIR}/configuration/ba.cc"
  "${SRC_DIR}/configuration/bool_expression.cc"
  "${SRC_DIR}/configuration/db.cc"
  "${SRC_DIR}/configuration/kpi.cc"
  "${SRC_DIR}/configuration/reader.cc"
  "${SRC_DIR}/configuration/state.cc"
  # Headers.
  "${INC_DIR}/configuration/ba.hh"
  "${INC_DIR}/configuration/bool_expression.hh"
  "${INC_DIR}/configuration/db.hh"
  "${INC_DIR}/configuration/kpi.hh"
  "${INC_DIR}/configuration/reader.hh"
  "${INC_DIR}/configuration/state.hh"

)
target_link_libraries("${BAM}" ${QT_LIBRARIES})
set_target_properties("${BAM}" PROPERTIES PREFIX "")
get_property(BAM_MODULE_PATH
  TARGET "${BAM}"
  PROPERTY LOCATION)
set(BAM_MODULE_PATH "${BAM_MODULE_PATH}" PARENT_SCOPE)

if (WITH_TESTING)
  set(TEST_LIBRARIES "${BAM}" "roker")

  # Perfdata exception tests.
  #   Throw.
  set(TEST_NAME "bam_exceptions_perfdata_throw")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/exceptions/perfdata/throw.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Rethrow.
  set(TEST_NAME "bam_exceptions_perfdata_rethrow")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/exceptions/perfdata/rethrow.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Clone.
  set(TEST_NAME "bam_exceptions_perfdata_clone")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/exceptions/perfdata/clone.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Perfdata object tests.
  #   Default construction.
  set(TEST_NAME "bam_perfdata_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/perfdata/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "bam_perfdata_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/perfdata/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "bam_perfdata_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/perfdata/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Metric event tests.
  #   Default construction.
  set(TEST_NAME "bam_metric_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/metric/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "bam_metric_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/metric/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "bam_metric_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/metric/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Rebuild event tests.
  #   Default construction.
  set(TEST_NAME "bam_rebuild_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/rebuild/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "bam_rebuild_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/rebuild/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "bam_rebuild_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/rebuild/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Remove graph event tests.
  #   Default construction.
  set(TEST_NAME "bam_remove_graph_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/remove_graph/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "bam_remove_graph_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/remove_graph/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "bam_remove_graph_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/remove_graph/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Status event tests.
  #   Default construction.
  set(TEST_NAME "bam_status_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/status/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "bam_status_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/status/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "bam_status_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/status/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Parser tests.
  #   Simple perfdata parsing.
  set(TEST_NAME "bam_parser_parse_simple")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/parser/parse_simple.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Complex perfdata parsing.
  set(TEST_NAME "bam_parser_parse_complex")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/parser/parse_complex.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Incorrect perfdata parsing.
  set(TEST_NAME "bam_parser_parse_incorrect")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/parser/parse_incorrect.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Loop perfdata parsing.
  set(TEST_NAME "bam_parser_parse_loop")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/parser/parse_loop.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
endif ()

# Install rule.
install(TARGETS "${BAM}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
