##
## Copyright 2011-2015 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/graphite/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/graphite/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/graphite/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")
set(INC_DIR "${INC_DIR}/com/centreon/broker/graphite")

# GRAPHITE module.
set(GRAPHITE "70-graphite")
set(GRAPHITE "${GRAPHITE}" PARENT_SCOPE)
add_library("${GRAPHITE}" SHARED
  # Sources.
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/stream.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/query.cc"
  "${SRC_DIR}/macro_cache.cc"
  # Headers.
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/stream.hh"
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/query.hh"
  "${INC_DIR}/macro_cache.hh"
)
target_link_libraries("${GRAPHITE}" ${QT_LIBRARIES})
set_target_properties("${GRAPHITE}" PROPERTIES PREFIX "")
get_property(GRAPHITE_MODULE_PATH
  TARGET "${GRAPHITE}"
  PROPERTY LOCATION)
set(GRAPHITE_MODULE_PATH "${GRAPHITE_MODULE_PATH}" PARENT_SCOPE)

# Testing.
if (WITH_TESTING)
  set(TEST_LIBRARIES "${NEB}" "${GRAPHITE}" "roker")

endif ()

# Install rule.
install(TARGETS "${GRAPHITE}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
