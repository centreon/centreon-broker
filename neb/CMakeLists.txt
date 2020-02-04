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
add_library(nebbase STATIC ${NEB_SOURCES})
add_dependencies(nebbase json11_lib)
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fPIC" COMPILER_HAS_FPIC)
if (COMPILER_HAS_FPIC)
  get_property(NEBBASE_CXXFLAGS
    TARGET nebbase
    PROPERTY COMPILE_FLAGS)
  if (NEBBASE_CXXFLAGS)
    set(NEBBASE_CXXFLAGS "${NEBBASE_CXXFLAGS} -fPIC")
  else ()
    set(NEBBASE_CXXFLAGS "-fPIC")
  endif ()
  set_property(TARGET nebbase
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
  "${SRC_DIR}/node_id.cc"
  "${SRC_DIR}/timeperiod_serializable.cc"
  "${SRC_DIR}/downtime_serializable.cc"
  "${SRC_DIR}/downtime_map.cc"

  # Inc
  "${INC_DIR}/com/centreon/broker/neb/downtime_scheduler.hh"
  "${INC_DIR}/com/centreon/broker/neb/node_cache.hh"
  "${INC_DIR}/com/centreon/broker/neb/node_id.hh"
  "${INC_DIR}/com/centreon/broker/neb/timeperiod_serializable.hh"
  "${INC_DIR}/com/centreon/broker/neb/downtime_serializable.hh"
  "${INC_DIR}/com/centreon/broker/neb/downtime_map.hh"
)
# Flags needed to include all symbols in binary.
target_link_libraries(${NEB} nebbase)
#  "-Wl,--whole-archive" nebbase "-Wl,--no-whole-archive")
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
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # Flags needed to include all symbols in shared library.
  target_link_libraries("${CBMOD}"
    "-Wl,--whole-archive" "nebbase" "rokerbase" "-Wl,--no-whole-archive")
else ()
  target_link_libraries("${CBMOD}" "nebbase" "rokerbase")
endif ()
set_target_properties("${CBMOD}" PROPERTIES PREFIX "")

# Testing.
if (WITH_TESTING)
  set(
      TESTS_SOURCES
      ${TESTS_SOURCES}
      ${SRC_DIR}/set_log_data.cc
      #Actual tests
      ${TEST_DIR}/custom_variable.cc
      ${TEST_DIR}/custom_variable_status.cc
      ${TEST_DIR}/event_handler.cc
      ${TEST_DIR}/flapping_status.cc
      ${TEST_DIR}/host.cc
      ${TEST_DIR}/host_check.cc
      ${TEST_DIR}/host_dependency.cc
      ${TEST_DIR}/host_parent.cc
      ${TEST_DIR}/host_status.cc
      ${TEST_DIR}/instance.cc
      ${TEST_DIR}/instance_status.cc
      ${TEST_DIR}/log_entry.cc
      ${TEST_DIR}/module.cc
      ${TEST_DIR}/randomize.cc
      ${TEST_DIR}/randomize.hh
      ${TEST_DIR}/service.cc
      ${TEST_DIR}/service_check.cc
      ${TEST_DIR}/service_dependency.cc
      ${TEST_DIR}/service_status.cc
      ${TEST_DIR}/set_log_data.cc
      PARENT_SCOPE
     )
  set(
      TESTS_LIBRARIES
      ${TESTS_LIBRARIES}
      ${NEB}
      PARENT_SCOPE
     )
endif()

# Install rules.
install(TARGETS "${CBMOD}"
  LIBRARY DESTINATION "${PREFIX_LIB}"
)
