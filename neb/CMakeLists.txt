##
## Copyright 2009-2013,2015 Centreon
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/neb/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/neb/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/neb/test")
include_directories("${INC_DIR}")

# NEB sources.
set(NEB_SOURCES
  # Sources.
  "${SRC_DIR}/acknowledgement.cc"
  "${SRC_DIR}/check.cc"
  "${SRC_DIR}/comment.cc"
  "${SRC_DIR}/custom_variable.cc"
  "${SRC_DIR}/custom_variable_status.cc"
  "${SRC_DIR}/dependency.cc"
  "${SRC_DIR}/downtime.cc"
  "${SRC_DIR}/event_handler.cc"
  "${SRC_DIR}/flapping_status.cc"
  "${SRC_DIR}/group.cc"
  "${SRC_DIR}/group_member.cc"
  "${SRC_DIR}/host.cc"
  "${SRC_DIR}/host_check.cc"
  "${SRC_DIR}/host_dependency.cc"
  "${SRC_DIR}/host_group.cc"
  "${SRC_DIR}/host_group_member.cc"
  "${SRC_DIR}/host_parent.cc"
  "${SRC_DIR}/host_service.cc"
  "${SRC_DIR}/host_service_status.cc"
  "${SRC_DIR}/host_status.cc"
  "${SRC_DIR}/instance.cc"
  "${SRC_DIR}/instance_configuration.cc"
  "${SRC_DIR}/instance_status.cc"
  "${SRC_DIR}/log_entry.cc"
  "${SRC_DIR}/module.cc"
  "${SRC_DIR}/responsive_instance.cc"
  "${SRC_DIR}/service.cc"
  "${SRC_DIR}/service_check.cc"
  "${SRC_DIR}/service_dependency.cc"
  "${SRC_DIR}/service_group.cc"
  "${SRC_DIR}/service_group_member.cc"
  "${SRC_DIR}/service_status.cc"
  "${SRC_DIR}/status.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/neb/acknowledgement.hh"
  "${INC_DIR}/com/centreon/broker/neb/check.hh"
  "${INC_DIR}/com/centreon/broker/neb/comment.hh"
  "${INC_DIR}/com/centreon/broker/neb/custom_variable.hh"
  "${INC_DIR}/com/centreon/broker/neb/custom_variable_status.hh"
  "${INC_DIR}/com/centreon/broker/neb/dependency.hh"
  "${INC_DIR}/com/centreon/broker/neb/downtime.hh"
  "${INC_DIR}/com/centreon/broker/neb/event_handler.hh"
  "${INC_DIR}/com/centreon/broker/neb/events.hh"
  "${INC_DIR}/com/centreon/broker/neb/flapping_status.hh"
  "${INC_DIR}/com/centreon/broker/neb/group.hh"
  "${INC_DIR}/com/centreon/broker/neb/group_member.hh"
  "${INC_DIR}/com/centreon/broker/neb/host.hh"
  "${INC_DIR}/com/centreon/broker/neb/host_check.hh"
  "${INC_DIR}/com/centreon/broker/neb/host_dependency.hh"
  "${INC_DIR}/com/centreon/broker/neb/host_group.hh"
  "${INC_DIR}/com/centreon/broker/neb/host_group_member.hh"
  "${INC_DIR}/com/centreon/broker/neb/host_parent.hh"
  "${INC_DIR}/com/centreon/broker/neb/host_service.hh"
  "${INC_DIR}/com/centreon/broker/neb/host_service_status.hh"
  "${INC_DIR}/com/centreon/broker/neb/host_status.hh"
  "${INC_DIR}/com/centreon/broker/neb/instance.hh"
  "${INC_DIR}/com/centreon/broker/neb/instance_configuration.hh"
  "${INC_DIR}/com/centreon/broker/neb/instance_status.hh"
  "${INC_DIR}/com/centreon/broker/neb/internal.hh"
  "${INC_DIR}/com/centreon/broker/neb/log_entry.hh"
  "${INC_DIR}/com/centreon/broker/neb/module.hh"
  "${INC_DIR}/com/centreon/broker/neb/responsive_instance.hh"
  "${INC_DIR}/com/centreon/broker/neb/service.hh"
  "${INC_DIR}/com/centreon/broker/neb/service_check.hh"
  "${INC_DIR}/com/centreon/broker/neb/service_dependency.hh"
  "${INC_DIR}/com/centreon/broker/neb/service_group.hh"
  "${INC_DIR}/com/centreon/broker/neb/service_group_member.hh"
  "${INC_DIR}/com/centreon/broker/neb/service_status.hh"
  "${INC_DIR}/com/centreon/broker/neb/status.hh"
)

# Static library.
add_library("nebbase" STATIC ${NEB_SOURCES})
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fPIC" COMPILER_HAS_FPIC)
if (COMPILER_HAS_FPIC)
  get_property(NEBBASE_CXXFLAGS
    TARGET "nebbase"
    PROPERTY COMPILE_FLAGS)
  if (NEBBASE_CXXFLAGS)
    set(NEBBASE_CXXFLAGS "${NEBBASE_CXXFLAGS} -fPIC")
  else ()
    set(NEBBASE_CXXFLAGS "-fPIC")
  endif ()
  set_property(TARGET "nebbase"
    PROPERTY COMPILE_FLAGS "${NEBBASE_CXXFLAGS}")
endif ()

# Centreon Broker module.
set(NEB "10-neb")
set(NEB "${NEB}" PARENT_SCOPE)
add_library("${NEB}" SHARED
  # Main source.
  "${SRC_DIR}/broker.cc"
  "${SRC_DIR}/downtime_scheduler.cc"
  "${SRC_DIR}/node_cache.cc"
  "${SRC_DIR}/node_events_connector.cc"
  "${SRC_DIR}/node_events_factory.cc"
  "${SRC_DIR}/node_events_stream.cc"
  "${SRC_DIR}/node_id.cc"
  "${SRC_DIR}/timeperiod_serializable.cc"
  "${SRC_DIR}/downtime_serializable.cc"
  "${SRC_DIR}/downtime_map.cc"

  # Inc
  "${INC_DIR}/com/centreon/broker/neb/downtime_scheduler.hh"
  "${INC_DIR}/com/centreon/broker/neb/node_cache.hh"
  "${INC_DIR}/com/centreon/broker/neb/node_events_connector.hh"
  "${INC_DIR}/com/centreon/broker/neb/node_events_factory.hh"
  "${INC_DIR}/com/centreon/broker/neb/node_events_stream.hh"
  "${INC_DIR}/com/centreon/broker/neb/node_id.hh"
  "${INC_DIR}/com/centreon/broker/neb/timeperiod_serializable.hh"
  "${INC_DIR}/com/centreon/broker/neb/downtime_serializable.hh"
  "${INC_DIR}/com/centreon/broker/neb/downtime_map.hh"
)
if (CMAKE_COMPILER_IS_GNUCXX)
  # Flags needed to include all symbols in binary.
  target_link_libraries("${NEB}"
    "-Wl,--whole-archive" "nebbase" "-Wl,--no-whole-archive")
else ()
  target_link_libraries("${NEB}" "nebbase")
endif ()
target_link_libraries("${NEB}" ${QT_LIBRARIES})
set_target_properties("${NEB}" PROPERTIES PREFIX "")
install(TARGETS "${NEB}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)

# Centreon Engine/Nagios module.
set(CBMOD "cbmod")
add_library("${CBMOD}" SHARED
  # Sources.
  "${PROJECT_SOURCE_DIR}/core/src/config/applier/init.cc"
  "${PROJECT_SOURCE_DIR}/core/src/config/applier/logger.cc"
  "${SRC_DIR}/callback.cc"
  "${SRC_DIR}/callbacks.cc"
  "${SRC_DIR}/initial.cc"
  "${SRC_DIR}/internal.cc"
  "${SRC_DIR}/monitoring_logger.cc"
  "${SRC_DIR}/neb.cc"
  "${SRC_DIR}/set_log_data.cc"
  "${SRC_DIR}/engcmd/factory.cc"
  "${SRC_DIR}/engcmd/endpoint.cc"
  "${SRC_DIR}/engcmd/engine_command.cc"
  "${SRC_DIR}/engcmd/internal.cc"
  "${SRC_DIR}/statistics/active_host_execution_time.cc"
  "${SRC_DIR}/statistics/active_host_latency.cc"
  "${SRC_DIR}/statistics/active_host_state_change.cc"
  "${SRC_DIR}/statistics/active_hosts_last.cc"
  "${SRC_DIR}/statistics/active_service_execution_time.cc"
  "${SRC_DIR}/statistics/active_service_latency.cc"
  "${SRC_DIR}/statistics/active_service_state_change.cc"
  "${SRC_DIR}/statistics/active_services_last.cc"
  "${SRC_DIR}/statistics/command_buffers.cc"
  "${SRC_DIR}/statistics/generator.cc"
  "${SRC_DIR}/statistics/hosts.cc"
  "${SRC_DIR}/statistics/hosts_actively_checked.cc"
  "${SRC_DIR}/statistics/hosts_checked.cc"
  "${SRC_DIR}/statistics/hosts_flapping.cc"
  "${SRC_DIR}/statistics/hosts_scheduled.cc"
  "${SRC_DIR}/statistics/passive_host_latency.cc"
  "${SRC_DIR}/statistics/passive_host_state_change.cc"
  "${SRC_DIR}/statistics/passive_hosts_last.cc"
  "${SRC_DIR}/statistics/passive_service_latency.cc"
  "${SRC_DIR}/statistics/passive_service_state_change.cc"
  "${SRC_DIR}/statistics/passive_services_last.cc"
  "${SRC_DIR}/statistics/plugin.cc"
  "${SRC_DIR}/statistics/services.cc"
  "${SRC_DIR}/statistics/services_actively_checked.cc"
  "${SRC_DIR}/statistics/services_checked.cc"
  "${SRC_DIR}/statistics/services_flapping.cc"
  "${SRC_DIR}/statistics/services_scheduled.cc"
  "${SRC_DIR}/statistics/total_host_state_change.cc"
  "${SRC_DIR}/statistics/total_hosts.cc"
  "${SRC_DIR}/statistics/total_service_state_change.cc"
  "${SRC_DIR}/statistics/total_services.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/neb/callback.hh"
  "${INC_DIR}/com/centreon/broker/neb/callbacks.hh"
  "${INC_DIR}/com/centreon/broker/neb/initial.hh"
  "${INC_DIR}/com/centreon/broker/neb/internal.hh"
  "${INC_DIR}/com/centreon/broker/neb/monitoring_logger.hh"
  "${INC_DIR}/com/centreon/broker/neb/set_log_data.hh"
  "${INC_DIR}/com/centreon/broker/neb/engcmd/factory.hh"
  "${INC_DIR}/com/centreon/broker/neb/engcmd/endpoint.hh"
  "${INC_DIR}/com/centreon/broker/neb/engcmd/engine_command.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/active_host_execution_time.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/active_host_latency.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/active_host_state_change.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/active_hosts_last.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/active_service_execution_time.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/active_service_latency.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/active_service_state_change.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/active_services_last.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/command_buffers.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/compute_value.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/generator.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/hosts.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/hosts_actively_checked.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/hosts_checked.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/hosts_flapping.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/hosts_scheduled.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/passive_host_latency.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/passive_host_state_change.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/passive_hosts_last.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/passive_service_latency.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/passive_service_state_change.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/passive_services_last.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/plugin.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/services.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/services_actively_checked.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/services_checked.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/services_flapping.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/services_scheduled.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/total_host_state_change.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/total_hosts.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/total_service_state_change.hh"
  "${INC_DIR}/com/centreon/broker/neb/statistics/total_services.hh"
)
get_property(CBMOD_DEFINES
  TARGET "${CBMOD}"
  PROPERTY COMPILE_DEFINITIONS)
list(APPEND CBMOD_DEFINES CBMOD)
set_property(TARGET "${CBMOD}"
  PROPERTY COMPILE_DEFINITIONS "${CBMOD_DEFINES}")
if (CMAKE_COMPILER_IS_GNUCXX)
  # Flags needed to include all symbols in shared library.
  target_link_libraries("${CBMOD}"
    "-Wl,--whole-archive" "nebbase" "rokerbase" "-Wl,--no-whole-archive")
else ()
  target_link_libraries("${CBMOD}" "nebbase" "rokerbase")
endif ()
set_target_properties("${CBMOD}" PROPERTIES PREFIX "")

# Testing.
set(
    TESTS_SOURCES
    ${TESTS_SOURCES}
    "${TEST_DIR}/randomize.cc"
    "${TEST_DIR}/custom_variable.cc"
    "${TEST_DIR}/custom_variable_status.cc"
    "${TEST_DIR}/event_handler.cc"
    "${TEST_DIR}/flapping_status.cc"

    "${TEST_DIR}/randomize.hh"
    PARENT_SCOPE
   )
set(
    TESTS_LIBRARIES
    ${TESTS_LIBRARIES}
    "${NEB}"
    PARENT_SCOPE
   )

# Unit tests.
if (0) # WITH_TESTING)
  include_directories("${PROJECT_SOURCE_DIR}/neb")
  include_directories("${PROJECT_SOURCE_DIR}/correlation/inc")
  include_directories("${PROJECT_SOURCE_DIR}/dumper/inc")
  set(TEST_LIBRARIES "${NEB}" "rokerbase" "roker" "rokerbase")
  # Host Tests.
  #   Default constructor.
  set(TEST_NAME "neb_host_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_host_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment.
  set(TEST_NAME "neb_host_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Host Check Tests.
  #   Default constructor.
  set(TEST_NAME "neb_host_check_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_check/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_host_check_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_check/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "neb_host_check_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_check/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Host Dependency Tests.
  #   Default constructor.
  set(TEST_NAME "neb_host_dependency_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_dependency/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_host_dependency_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_dependency/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "neb_host_dependency_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_dependency/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Host Parent Tests.
  #   Default constructor.
  set(TEST_NAME "neb_host_parent_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_parent/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_host_parent_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_parent/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "neb_host_parent_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_parent/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Host Status Tests.
  #   Default constructor.
  set(TEST_NAME "neb_host_status_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_status/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_host_status_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_status/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "neb_host_status_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/host_status/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Instance Tests.
  #   Default constructor.
  set(TEST_NAME "neb_instance_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/instance/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_instance_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/instance/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "neb_instance_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/instance/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Instance Status Tests.
  #   Default constructor.
  set(TEST_NAME "neb_instance_status_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/instance_status/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_instance_status_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/instance_status/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "neb_instance_status_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/instance_status/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Log Entry Tests.
  #   Default constructor.
  set(TEST_NAME "neb_log_entry_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/log_entry/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_log_entry_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/log_entry/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "neb_log_entry_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/log_entry/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Module Tests.
  #   Default constructor.
  set(TEST_NAME "neb_module_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/module/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_module_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/module/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "neb_module_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/module/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Service Tests.
  #   Default constructor.
  set(TEST_NAME "neb_service_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_service_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "neb_service_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Service Check Tests.
  #   Default constructor.
  set(TEST_NAME "neb_service_check_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_check/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_service_check_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_check/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "neb_service_check_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_check/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Service Dependency Tests.
  #   Default constructor.
  set(TEST_NAME "neb_service_dependency_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_dependency/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_service_dependency_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_dependency/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "neb_service_dependency_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_dependency/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Service Status Tests.
  #   Default constructor.
  set(TEST_NAME "neb_service_status_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_status/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "neb_service_status_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_status/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "neb_service_status_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/service_status/assignment.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # set_log_data tests.
  #   set_log_data library.
  add_library("ccb_neb_set_log_data" STATIC
    "${SRC_DIR}/internal.cc"
    "${SRC_DIR}/set_log_data.cc")
  set(SETLOGDATA_LIBRARIES "ccb_neb_set_log_data" "${NEB}" "rokerbase" "roker" "rokerbase")
  #   Host alert.
  set(TEST_NAME "neb_set_log_data_host_alert")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/set_log_data/dummy_engine.cc"
    "${TEST_DIR}/set_log_data/host_alert.cc")
  target_link_libraries("${TEST_NAME}" ${SETLOGDATA_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Service alert.
  set(TEST_NAME "neb_set_log_data_service_alert")
  add_executable("${TEST_NAME}"
    "${SRC_DIR}/internal.cc"
    "${SRC_DIR}/set_log_data.cc"
    "${TEST_DIR}/set_log_data/dummy_engine.cc"
    "${TEST_DIR}/set_log_data/service_alert.cc")
  target_link_libraries("${TEST_NAME}" ${SETLOGDATA_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Initial host state.
  set(TEST_NAME "neb_set_log_data_initial_host_state")
  add_executable("${TEST_NAME}"
    "${SRC_DIR}/internal.cc"
    "${SRC_DIR}/set_log_data.cc"
    "${TEST_DIR}/set_log_data/dummy_engine.cc"
    "${TEST_DIR}/set_log_data/initial_host_state.cc")
  target_link_libraries("${TEST_NAME}" ${SETLOGDATA_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Initial service state.
  set(TEST_NAME "neb_set_log_data_initial_service_state")
  add_executable("${TEST_NAME}"
    "${SRC_DIR}/internal.cc"
    "${SRC_DIR}/set_log_data.cc"
    "${TEST_DIR}/set_log_data/dummy_engine.cc"
    "${TEST_DIR}/set_log_data/initial_service_state.cc")
  target_link_libraries("${TEST_NAME}" ${SETLOGDATA_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Default logs.
  set(TEST_NAME "neb_set_log_data_default")
  add_executable("${TEST_NAME}"
    "${SRC_DIR}/internal.cc"
    "${SRC_DIR}/set_log_data.cc"
    "${TEST_DIR}/set_log_data/dummy_engine.cc"
    "${TEST_DIR}/set_log_data/default.cc")
  target_link_libraries("${TEST_NAME}" ${SETLOGDATA_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Node events.
  set(TEST_NAME "neb_downtime_fixed")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node_events/downtime_fixed.cc"
    "${TEST_DIR}/node_events/common.hh"
    "${TEST_DIR}/node_events/common.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  set(TEST_NAME "neb_downtime_floating")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node_events/downtime_floating.cc"
    "${TEST_DIR}/node_events/common.hh"
    "${TEST_DIR}/node_events/common.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  set(TEST_NAME "neb_downtime_recurring")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node_events/recurring_downtime.cc"
    "${TEST_DIR}/node_events/common.hh"
    "${TEST_DIR}/node_events/common.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  set(TEST_NAME "neb_downtime_file")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/node_events/file_downtime.cc"
    "${TEST_DIR}/node_events/common.hh"
    "${TEST_DIR}/node_events/common.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  configure_file("${TEST_DIR}/node_events/vars.hh.in"
    "${TEST_DIR}/node_events/vars.hh")
endif ()

# Install rules.
install(TARGETS "${CBMOD}"
  LIBRARY DESTINATION "${PREFIX_LIB}"
)
