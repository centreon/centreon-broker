##
## Copyright 2011-2014 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/influxdb/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/influxdb/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")
set(INC_DIR "${INC_DIR}/com/centreon/broker/influxdb")

# INFLUXDB module.
set(INFLUXDB "80-influxdb")
set(INFLUXDB "${INFLUXDB}" PARENT_SCOPE)
add_library("${INFLUXDB}" SHARED
  # Sources.
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/column.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/influxdb9.cc"
  "${SRC_DIR}/stream.cc"
  "${SRC_DIR}/macro_cache.cc"
  "${SRC_DIR}/json_printer.cc"
  "${SRC_DIR}/query.cc"
  # Headers.
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/column.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/influxdb.hh"
  "${INC_DIR}/influxdb9.hh"
  "${INC_DIR}/stream.hh"
  "${INC_DIR}/macro_cache.hh"
  "${INC_DIR}/json_printer.hh"
  "${INC_DIR}/query.hh"
)
target_link_libraries("${INFLUXDB}" ${QT_LIBRARIES})
set_target_properties("${INFLUXDB}" PROPERTIES PREFIX "")
get_property(INFLUXDB_MODULE_PATH
  TARGET "${INFLUXDB}"
  PROPERTY LOCATION)
set(INFLUXDB_MODULE_PATH "${INFLUXDB_MODULE_PATH}" PARENT_SCOPE)

# Testing.
if (WITH_TESTING)
  set(TEST_LIBRARIES "${NEB}" "${INFLUXDB}" "roker")

endif ()

# Install rule.
install(TARGETS "${INFLUXDB}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
