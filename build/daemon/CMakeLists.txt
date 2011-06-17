##
## Copyright 2009-2011 Merethis
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
set(SRC_DIR "${PROJECT_SOURCE_DIR}/daemon/src")

# Standalone binary.
add_executable("cbd"
  # Core sources.
  ${LIBROKER_SOURCES}
  # Sources.
  "${SRC_DIR}/main.cc"
)
target_link_libraries("cbd" ${QT_LIBRARIES})

# Install rule.
install(TARGETS "cbd"
  RUNTIME DESTINATION "${PREFIX_BINARY}"
)
