##
## Copyright 2011 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/sql/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/sql/src")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")

# SQL module.
add_library("sql" MODULE
  # Sources.
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/internal.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/stream.cc"
  "${PROJECT_SOURCE_DIR}/core/src/mapping.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/sql/connector.hh"
  "${INC_DIR}/com/centreon/broker/sql/factory.hh"
  "${INC_DIR}/com/centreon/broker/sql/internal.hh"
  "${INC_DIR}/com/centreon/broker/sql/stream.hh"
)
set_target_properties("sql" PROPERTIES PREFIX "")

# Install rule.
install(TARGETS sql
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
