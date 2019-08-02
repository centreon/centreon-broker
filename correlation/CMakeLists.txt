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
set(INC_DIR "${PROJECT_SOURCE_DIR}/correlation/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/correlation/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/correlation/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
set(INC_DIR "${INC_DIR}/com/centreon/broker/correlation")

# Some files must be processed by Qt's moc.
unset(QT_WRAPPED_SOURCES)

# Correlation module.
set(CORRELATION "30-correlation")
set(CORRELATION "${CORRELATION}" PARENT_SCOPE)
add_library("${CORRELATION}" SHARED
  # Sources.
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/engine_state.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/issue.cc"
  "${SRC_DIR}/issue_parent.cc"
  "${SRC_DIR}/log_issue.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/node.cc"
  "${SRC_DIR}/state.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/engine_state.hh"
  "${INC_DIR}/events.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/internal.hh"
  "${INC_DIR}/issue.hh"
  "${INC_DIR}/issue_parent.hh"
  "${INC_DIR}/log_issue.hh"
  "${INC_DIR}/node.hh"
  "${INC_DIR}/state.hh"
  "${INC_DIR}/stream.hh"
  # Qt-processed files.
  ${QT_WRAPPED_SOURCES}
)
target_link_libraries("${CORRELATION}" ${QT_LIBRARIES})
set_target_properties("${CORRELATION}" PROPERTIES PREFIX "")

# Testing.
if (0) # WITH_TESTING)
  include_directories("${PROJECT_SOURCE_DIR}/correlation")
  set(TEST_LIBRARIES "${NOTIFICATION}" "${CORRELATION}" "${NEB}" "rokerbase" "roker")

  # Generate files.
  configure_file("${TEST_DIR}/parser/parse_include.xml.in"
    "${TEST_DIR}/parser/parse_include.xml")

  # state tests.
  #   Default construction.
  set(TEST_NAME "correlation_state_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/state/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "correlation_state_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/state/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment.
  set(TEST_NAME "correlation_state_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/state/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Equality.
  set(TEST_NAME "correlation_state_equality")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/state/equality.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Non-equality.
  set(TEST_NAME "correlation_state_non_equality")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/state/non_equality.cc")
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
  set(TEST_NAME "correlation_issue_assignment")
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
  #   Include directive.
  set(TEST_NAME "correlation_parser_parse_include")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/parser/parse_include.cc"
    "${TEST_DIR}/parser/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_parser"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # correlator tests.
  add_library("ccb_correlation_correlator"
    STATIC
    "${TEST_DIR}/correlator/common.cc")
  #   Set correlator state.
  set(TEST_NAME "correlation_correlator_set_state")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/set_state.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Start/stop.
  set(TEST_NAME "correlation_correlator_start_stop")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/start_stop.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_correlator"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Issue creation and deletion.
  set(TEST_NAME "correlation_correlator_issue")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/issue.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_correlator"
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Use UNKNOWN for services and UNREACHABLE for hosts.
  set(TEST_NAME "correlation_correlator_unknown_unreachable")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/unknown_unreachable.cc"
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
  # #   Downtime handling.
  # set(TEST_NAME "correlation_correlator_downtime")
  # add_executable("${TEST_NAME}"
  #   "${TEST_DIR}/correlator/downtime.cc"
  #   "${TEST_DIR}/correlator/common.hh")
  # target_link_libraries("${TEST_NAME}"
  #   "ccb_correlation_correlator"
  #   ${TEST_LIBRARIES})
  # add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Acknowledgement handling.
  set(TEST_NAME "correlation_correlator_acknowledgement")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/correlator/acknowledgement.cc"
    "${TEST_DIR}/correlator/common.hh")
  target_link_libraries("${TEST_NAME}"
    "ccb_correlation_correlator"
    ${NOTIFICATION}
    ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
endif ()

# Install rule.
install(TARGETS "${CORRELATION}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
