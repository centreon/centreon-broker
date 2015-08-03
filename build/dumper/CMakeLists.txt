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
set(INC_DIR "${PROJECT_SOURCE_DIR}/dumper/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/dumper/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/dumper/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")

# Dumper module.
set(DUMPER "05-dumper")
set(DUMPER "${DUMPER}" PARENT_SCOPE)
add_library("${DUMPER}" SHARED
  # Sources.
  "${SRC_DIR}/db_dump.cc"
  "${SRC_DIR}/db_loader.cc"
  "${SRC_DIR}/db_reader.cc"
  "${SRC_DIR}/db_writer.cc"
  "${SRC_DIR}/dump.cc"
  "${SRC_DIR}/entries/ba.cc"
  "${SRC_DIR}/entries/ba_type.cc"
  "${SRC_DIR}/entries/diff.cc"
  "${SRC_DIR}/entries/kpi.cc"
  "${SRC_DIR}/entries/state.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/opener.cc"
  "${SRC_DIR}/reload.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/dumper/db_dump.hh"
  "${INC_DIR}/com/centreon/broker/dumper/db_loader.hh"
  "${INC_DIR}/com/centreon/broker/dumper/db_reader.hh"
  "${INC_DIR}/com/centreon/broker/dumper/db_writer.hh"
  "${INC_DIR}/com/centreon/broker/dumper/dump.hh"
  "${INC_DIR}/com/centreon/broker/dumper/entries/ba.hh"
  "${INC_DIR}/com/centreon/broker/dumper/entries/ba_type.hh"
  "${INC_DIR}/com/centreon/broker/dumper/entries/diff.hh"
  "${INC_DIR}/com/centreon/broker/dumper/entries/kpi.hh"
  "${INC_DIR}/com/centreon/broker/dumper/entries/state.hh"
  "${INC_DIR}/com/centreon/broker/dumper/factory.hh"
  "${INC_DIR}/com/centreon/broker/dumper/internal.hh"
  "${INC_DIR}/com/centreon/broker/dumper/opener.hh"
  "${INC_DIR}/com/centreon/broker/dumper/reload.hh"
  "${INC_DIR}/com/centreon/broker/dumper/stream.hh"
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
