##
## Copyright 2013 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/bbdo/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/bbdo/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/bbdp/test")
include_directories("${INC_DIR}")
set(INC_DIR "${INC_DIR}/com/centreon/broker/bbdo")

# BBDO module.
set(BBDO "80-bbdo")
set(BBDO "${BBDO}" PARENT_SCOPE)
add_library("${BBDO}" SHARED
  # Sources.
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  # Headers.
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/factory.hh"
)
target_link_libraries("${BBDO}" ${QT_LIBRARIES})
set_target_properties("${BBDO}" PROPERTIES PREFIX "")
get_property(BBDO_MODULE_PATH
  TARGET "${BBDO}"
  PROPERTY LOCATION)
set(BBDO_MODULE_PATH "${BBDO_MODULE_PATH}" PARENT_SCOPE)
