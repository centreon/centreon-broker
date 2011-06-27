##
## Copyright 2009-2011 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/neb/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/neb/src")
include_directories("${INC_DIR}")

# Event sources.
set(EVENT_SOURCES
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
  "${SRC_DIR}/instance_status.cc"
  "${SRC_DIR}/log_entry.cc"
  "${SRC_DIR}/mapping.cc"
  "${SRC_DIR}/module.cc"
  "${SRC_DIR}/notification.cc"
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
  "${INC_DIR}/com/centreon/broker/neb/instance_status.hh"
  "${INC_DIR}/com/centreon/broker/neb/log_entry.hh"
  "${INC_DIR}/com/centreon/broker/neb/module.hh"
  "${INC_DIR}/com/centreon/broker/neb/notification.hh"
  "${INC_DIR}/com/centreon/broker/neb/service.hh"
  "${INC_DIR}/com/centreon/broker/neb/service_check.hh"
  "${INC_DIR}/com/centreon/broker/neb/service_dependency.hh"
  "${INC_DIR}/com/centreon/broker/neb/service_group.hh"
  "${INC_DIR}/com/centreon/broker/neb/service_group_member.hh"
  "${INC_DIR}/com/centreon/broker/neb/service_status.hh"
  "${INC_DIR}/com/centreon/broker/neb/status.hh"
)

# Centreon Broker module.
set(NEB "10-neb")
add_library("${NEB}" MODULE
  # Event sources.
  ${EVENT_SOURCES}
  # Main source.
  "${SRC_DIR}/broker.cc"
)
set_target_properties("${NEB}" PROPERTIES PREFIX "")
install(TARGETS "${NEB}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)

# Centreon Engine/Nagios module.
set(CBMOD "cbmod")
add_library("${CBMOD}" SHARED
  # Event sources.
  ${EVENT_SOURCES}
  # Sources.
  "${SRC_DIR}/callbacks.cc"
  "${SRC_DIR}/initial.cc"
  "${SRC_DIR}/neb.cc"
  "${SRC_DIR}/set_log_data.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/neb/callbacks.hh"
  "${INC_DIR}/com/centreon/broker/neb/initial.hh"
  "${INC_DIR}/com/centreon/broker/neb/internal.hh"
  "${INC_DIR}/com/centreon/broker/neb/set_log_data.hh"
)
if (CMAKE_COMPILER_IS_GNUCXX)
  # Flags needed to include all symbols in shared library.
  target_link_libraries("${CBMOD}"
    "-Wl,--whole-archive" "roker" "-Wl,--no-whole-archive")
else ()
  target_link_libraries("${CBMOD}" "roker")
endif ()
set_target_properties("${CBMOD}" PROPERTIES PREFIX "")
install(TARGETS "${CBMOD}"
  LIBRARY DESTINATION "${PREFIX_LIBRARY}"
)
