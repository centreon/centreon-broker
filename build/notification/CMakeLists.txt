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
  "${PROJECT_SOURCE_DIR}/core/src/mapping.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/notification/factory.hh"
)
set_target_properties("${NOTIFICATION}" PROPERTIES PREFIX "")
get_property(NOTIFICATION_MODULE_PATH
  TARGET "${NOTIFICATION}"
  PROPERTY LOCATION)
set(NOTIFICATION_MODULE_PATH "${NOTIFICATION_MODULE_PATH}" PARENT_SCOPE)

# Install rule.
install(TARGETS "${NOTIFICATION}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
