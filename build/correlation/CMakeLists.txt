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
set(INC_DIR "${PROJECT_SOURCE_DIR}/correlation/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/correlation/src")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")

# Correlation module.
set(CORRELATION "20-correlation")
add_library("${CORRELATION}" MODULE
  # Sources.
  "${SRC_DIR}/correlator.cc"
  "${SRC_DIR}/engine.cc"
  "${SRC_DIR}/host_state.cc"
  "${SRC_DIR}/issue.cc"
  "${SRC_DIR}/issue_parent.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/node.cc"
  "${SRC_DIR}/parser.cc"
  "${SRC_DIR}/service_state.cc"
  "${SRC_DIR}/state.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/correlation/correlator.hh"
  "${INC_DIR}/com/centreon/broker/correlation/engine.hh"
  "${INC_DIR}/com/centreon/broker/correlation/host_state.hh"
  "${INC_DIR}/com/centreon/broker/correlation/issue.hh"
  "${INC_DIR}/com/centreon/broker/correlation/issue_parent.hh"
  "${INC_DIR}/com/centreon/broker/correlation/node.hh"
  "${INC_DIR}/com/centreon/broker/correlation/parser.hh"
  "${INC_DIR}/com/centreon/broker/correlation/service_state.hh"
  "${INC_DIR}/com/centreon/broker/correlation/state.hh"
)
set_target_properties("${CORRELATION}" PROPERTIES PREFIX "")

# Install rule.
install(TARGETS "${CORRELATION}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
