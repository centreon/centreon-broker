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
set(INC_DIR "${PROJECT_SOURCE_DIR}/file/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/file/src")
include_directories("${INC_DIR}")

# File module.
set(FILE "50-file")
add_library("${FILE}" MODULE
  # Sources.
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/opener.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/file/factory.hh"
  "${INC_DIR}/com/centreon/broker/file/opener.hh"
  "${INC_DIR}/com/centreon/broker/file/stream.hh"
)
set_target_properties("${FILE}" PROPERTIES PREFIX "")

# Install rule.
install(TARGETS "${FILE}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
