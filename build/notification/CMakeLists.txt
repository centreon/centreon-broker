##
## Copyright 2014 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/notification/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/notification/src")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/correlation/inc")
include_directories("${PROJECT_SOURCE_DIR}/dumper/inc")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")

# NOTIFICATION module.
set(NOTIFICATION "80-notification")
set(NOTIFICATION "${NOTIFICATION}" PARENT_SCOPE)
add_library("${NOTIFICATION}" SHARED
  # Sources.
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/stream.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/objects/command.cc"
  "${SRC_DIR}/objects/contact.cc"
  "${SRC_DIR}/objects/dependency.cc"
  "${SRC_DIR}/objects/escalation.cc"
  "${SRC_DIR}/objects/node.cc"
  "${SRC_DIR}/objects/timeperiod.cc"
  "${SRC_DIR}/objects/group.cc"
  "${SRC_DIR}/objects/string.cc"
  "${SRC_DIR}/objects/timerange.cc"
  "${SRC_DIR}/objects/daterange.cc"
  "${SRC_DIR}/objects/timezone_locker.cc"
  "${SRC_DIR}/objects/timezone_manager.cc"
  "${SRC_DIR}/objects/node_id.cc"
  "${SRC_DIR}/objects/acknowledgement.cc"
  "${SRC_DIR}/objects/downtime.cc"
  "${SRC_DIR}/objects/qhash_func.cc"
  "${SRC_DIR}/loaders/contact_loader.cc"
  "${SRC_DIR}/loaders/dependency_loader.cc"
  "${SRC_DIR}/loaders/escalation_loader.cc"
  "${SRC_DIR}/loaders/node_loader.cc"
  "${SRC_DIR}/loaders/timeperiod_loader.cc"
  "${SRC_DIR}/loaders/command_loader.cc"
  "${SRC_DIR}/loaders/downtime_loader.cc"
  "${SRC_DIR}/loaders/acknowledgement_loader.cc"
  "${SRC_DIR}/builders/node_set_builder.cc"
  "${SRC_DIR}/builders/composed_contact_builder.cc"
  "${SRC_DIR}/builders/contact_by_node_builder.cc"
  "${SRC_DIR}/builders/contact_by_name_builder.cc"
  "${SRC_DIR}/builders/composed_dependency_builder.cc"
  "${SRC_DIR}/builders/composed_downtime_builder.cc"
  "${SRC_DIR}/builders/composed_node_builder.cc"
  "${SRC_DIR}/builders/composed_timeperiod_builder.cc"
  "${SRC_DIR}/builders/composed_acknowledgement_builder.cc"
  "${SRC_DIR}/builders/composed_escalation_builder.cc"
  "${SRC_DIR}/builders/composed_command_builder.cc"
  "${SRC_DIR}/builders/dependency_by_node_id_builder.cc"
  "${SRC_DIR}/builders/acknowledgement_by_node_id_builder.cc"
  "${SRC_DIR}/builders/downtime_by_node_id_builder.cc"
  "${SRC_DIR}/builders/escalation_by_node_id_builder.cc"
  "${SRC_DIR}/builders/timeperiod_by_name_builder.cc"
  "${PROJECT_SOURCE_DIR}/core/src/mapping.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/notification/factory.hh"
  "${INC_DIR}/com/centreon/broker/notification/stream.hh"
  "${INC_DIR}/com/centreon/broker/notification/connector.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/command.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/contact.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/dependency.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/escalation.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/node.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/timeperiod.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/group.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/string.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/timerange.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/daterange.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/timezone_manager.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/timezone_locker.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/node_id.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/acknowledgement.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/qhash_func.hh"
  "${INC_DIR}/com/centreon/broker/notification/objects/downtime.hh"
  "${INC_DIR}/com/centreon/broker/notification/loaders/command_loader.hh"
  "${INC_DIR}/com/centreon/broker/notification/loaders/contact_loader.hh"
  "${INC_DIR}/com/centreon/broker/notification/loaders/dependency_loader.hh"
  "${INC_DIR}/com/centreon/broker/notification/loaders/escalation_loader.hh"
  "${INC_DIR}/com/centreon/broker/notification/loaders/node_loader.hh"
  "${INC_DIR}/com/centreon/broker/notification/loaders/timeperiod_loader.hh"
  "${INC_DIR}/com/centreon/broker/notification/loaders/downtime_loader.hh"
  "${INC_DIR}/com/centreon/broker/notification/loaders/acknowledgement_loader.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/timeperiod_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/command_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/contact_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/dependency_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/escalation_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/node_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/node_set_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/downtime_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/acknowledgement_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/composed_contact_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/contact_by_node_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/contact_by_name_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/composed_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/composed_dependency_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/composed_downtime_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/composed_node_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/composed_timeperiod_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/composed_acknowledgement_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/composed_escalation_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/composed_command_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/dependency_by_node_id_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/acknowledgement_by_node_id_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/downtime_by_node_id_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/escalation_by_node_id_builder.hh"
  "${INC_DIR}/com/centreon/broker/notification/builders/timeperiod_by_name_builder.hh"
)
target_link_libraries("${NOTIFICATION}" ${QT_LIBRARIES})
set_target_properties("${NOTIFICATION}" PROPERTIES PREFIX "")
get_property(NOTIFICATION_MODULE_PATH
  TARGET "${NOTIFICATION}"
  PROPERTY LOCATION)
set(NOTIFICATION_MODULE_PATH "${NOTIFICATION_MODULE_PATH}" PARENT_SCOPE)

# Install rule.
install(TARGETS "${NOTIFICATION}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
