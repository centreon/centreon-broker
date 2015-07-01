##
## Copyright 2013,2015 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/dumper/inc/com/centreon/broker/dumper")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/dumper/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/dumper/test")
include_directories("${PROJECT_SOURCE_DIR}/dumper/inc")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")

# Dumper module.
set(DUMPER "05-dumper")
set(DUMPER "${DUMPER}" PARENT_SCOPE)
add_library("${DUMPER}" SHARED
  # Sources.
  "${SRC_DIR}/dump.cc"
  "${SRC_DIR}/directory_dumper.cc"
  "${SRC_DIR}/entries/ba.cc"
  "${SRC_DIR}/entries/kpi.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/fifo_dumper.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/opener.cc"
  "${SRC_DIR}/reload.cc"
  "${SRC_DIR}/remove.cc"
  "${SRC_DIR}/stream.cc"
  "${SRC_DIR}/timestamp_cache.cc"
  # Headers.
  "${INC_DIR}/dump.hh"
  "${INC_DIR}/directory_dumper.hh"
  "${INC_DIR}/entries/ba.hh"
  "${INC_DIR}/entries/kpi.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/fifo_dumper.hh"
  "${INC_DIR}/internal.hh"
  "${INC_DIR}/opener.hh"
  "${INC_DIR}/reload.hh"
  "${INC_DIR}/remove.hh"
  "${INC_DIR}/stream.hh"
  "${INC_DIR}/timestamp_cache.hh"
)
target_link_libraries("${DUMPER}" ${QT_LIBRARIES})
set_target_properties("${DUMPER}" PROPERTIES PREFIX "")
get_property(DUMPER_MODULE_PATH
  TARGET "${DUMPER}"
  PROPERTY LOCATION)
set(DUMPER_MODULE_PATH "${DUMPER_MODULE_PATH}" PARENT_SCOPE)

if (WITH_TESTING)
  set(TEST_LIBRARIES "${DUMPER}" "roker")

endif ()

# Install rule.
install(TARGETS "${DUMPER}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
