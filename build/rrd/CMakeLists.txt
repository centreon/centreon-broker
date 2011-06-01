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
set(INC_DIR "${PROJECT_SOURCE_DIR}/rrd/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/rrd/src")
include_directories("${INC_DIR}")

# RRD module.
add_library("rrd" MODULE
  # Sources.
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  # Headers.
  "${INC_DIR}/rrd/connector.hh"
  "${INC_DIR}/rrd/factory.hh"
)
set_target_properties("rrd" PROPERTIES PREFIX "")
