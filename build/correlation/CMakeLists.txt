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
set(INC_DIR "${PROJECT_SOURCE_DIR}/correlation/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/correlation/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/correlation/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")

# Some files must be processed by Qt's moc.
unset(QT_WRAPPED_SOURCES)
qt4_wrap_cpp(QT_WRAPPED_SOURCES
  "${INC_DIR}/com/centreon/broker/correlation/correlator.hh")

# Correlation module.
set(CORRELATION "20-correlation")
set(CORRELATION "${CORRELATION}" PARENT_SCOPE)
add_library("${CORRELATION}" SHARED
  # Sources.
  "${SRC_DIR}/correlator.cc"
  "${SRC_DIR}/host_state.cc"
  "${SRC_DIR}/issue.cc"
  "${SRC_DIR}/issue_parent.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/node.cc"
  "${SRC_DIR}/parser.cc"
  "${SRC_DIR}/service_state.cc"
  "${SRC_DIR}/state.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/correlation/correlator.hh"
  "${INC_DIR}/com/centreon/broker/correlation/host_state.hh"
  "${INC_DIR}/com/centreon/broker/correlation/issue.hh"
  "${INC_DIR}/com/centreon/broker/correlation/issue_parent.hh"
  "${INC_DIR}/com/centreon/broker/correlation/node.hh"
  "${INC_DIR}/com/centreon/broker/correlation/parser.hh"
  "${INC_DIR}/com/centreon/broker/correlation/service_state.hh"
  "${INC_DIR}/com/centreon/broker/correlation/state.hh"
  # Qt-processed files.
  ${QT_WRAPPED_SOURCES}
)
set_target_properties("${CORRELATION}" PROPERTIES PREFIX "")

# Testing.
if (WITH_TESTING)
  include_directories("${PROJECT_SOURCE_DIR}/correlation")
  set(TEST_LIBRARIES "${CORRELATION}" "${NEB}" "roker")

  # host_state tests.
  #   Default construction.
  set(TEST_NAME "correlation_host_state_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_state/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "correlation_host_state_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_state/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment.
  set(TEST_NAME "correlation_host_state_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_state/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Equality.
  set(TEST_NAME "correlation_host_state_equality")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_state/equality.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Non-equality.
  set(TEST_NAME "correlation_host_state_non_equality")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_state/non_equality.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # service_state tests.
  #   Default construction.
  set(TEST_NAME "correlation_service_state_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_state/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "correlation_service_state_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_state/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment.
  set(TEST_NAME "correlation_service_state_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_state/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Equality.
  set(TEST_NAME "correlation_service_state_equality")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_state/equality.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Non-equality.
  set(TEST_NAME "correlation_service_state_non_equality")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_state/non_equality.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # issue tests.
  #   Default construction.
  set(TEST_NAME "correlation_issue_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/issue/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "correlation_issue_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/issue/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment.
  set(TEST_NAME "correlation_issue_ctor_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/issue/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Equality.
  set(TEST_NAME "correlation_issue_equality")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/issue/equality.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Non-equality.
  set(TEST_NAME "correlation_issue_non_equality")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/issue/non_equality.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # issue_parent tests.
  #   Default construction.
  set(TEST_NAME "correlation_issue_parent_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/issue_parent/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "correlation_issue_parent_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/issue_parent/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment.
  set(TEST_NAME "correlation_issue_parent_ctor_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/issue_parent/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # node tests.
  #   Default construction.
  set(TEST_NAME "correlation_node_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Child link creation.
  set(TEST_NAME "correlation_node_add_child")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/add_child.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Depended link creation.
  set(TEST_NAME "correlation_node_add_depended")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/add_depended.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Dependency link creation.
  set(TEST_NAME "correlation_node_add_dependency")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/add_dependency")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Parent link creation.
  set(TEST_NAME "correlation_node_add_parent")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/add_parent.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Child link deletion.
  set(TEST_NAME "correlation_node_remove_child")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/remove_child.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Depended link deletion.
  set(TEST_NAME "correlation_node_remove_depended")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/remove_depended.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Dependency link deletion.
  set(TEST_NAME "correlation_node_remove_dependency")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/remove_dependency")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Parent link deletion.
  set(TEST_NAME "correlation_node_remove_parent")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/remove_parent.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "correlation_node_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment.
  set(TEST_NAME "correlation_node_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   On child destruction.
  set(TEST_NAME "correlation_node_on_destroy_child")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/on_destroy_child.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   On depended destruction.
  set(TEST_NAME "correlation_node_on_destroy_depended")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/on_destroy_depended.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   On dependency destruction.
  set(TEST_NAME "correlation_node_on_destroy_dependency")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/on_destroy_dependency.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   On parent destruction.
  set(TEST_NAME "correlation_node_on_destroy_parent")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/on_destroy_parent.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Equality.
  set(TEST_NAME "correlation_node_equality")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/equality.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Non-equality.
  set(TEST_NAME "correlation_node_non_equality")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node/non_equality.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # parser tests.
  add_library("ccb_correlation_parser"
    STATIC
    "${TEST_DIR}/parser/common.cc")
  #   Non-retention file loading.
  set(TEST_NAME "correlation_parser_parse_non_retention")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/parser/parse_non_retention.cc"
    "${TEST_DIR}/parser/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_parser"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Retention file loading.
  set(TEST_NAME "correlation_parser_parse_retention")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/parser/parse_retention.cc"
    "${TEST_DIR}/parser/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_parser"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # correlator tests.
  add_library("ccb_correlation_correlator"
    STATIC
    "${TEST_DIR}/correlator/common.cc")
  #   Issue creation and deletion.
  set(TEST_NAME "correlation_correlator_issue")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/issue.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_correlator"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Single parent.
  set(TEST_NAME "correlation_correlator_parent_single")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/parent_single.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_correlator"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Single dependency.
  set(TEST_NAME "correlation_correlator_dependency_single")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/dependency_single.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_correlator"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Multiple parents.
  set(TEST_NAME "correlation_correlator_parent_multiple")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/parent_multiple.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_correlator"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Multiple dependencies.
  set(TEST_NAME "correlation_correlator_dependency_multiple")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/dependency_multiple.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_correlator"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Write retention file..
  set(TEST_NAME "correlation_correlator_retention_write")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/retention_write.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_parser"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Read retention file.
  set(TEST_NAME "correlation_correlator_retention_read")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/retention_read.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_parser"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   State creation.
  set(TEST_NAME "correlation_correlator_state")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/state.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_correlator"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Downtime handling.
  set(TEST_NAME "correlation_correlator_downtime")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/downtime.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_correlator"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Acknowledgement handling.
  set(TEST_NAME "correlation_correlator_acknowledgement")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/acknowledgement.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_correlator"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
endif ()

# Install rule.
install(TARGETS "${CORRELATION}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
