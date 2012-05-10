##
## Copyright 2012 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/stats/inc/com/centreon/broker/stats")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/stats/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/stats/test")
include_directories("${PROJECT_SOURCE_DIR}/stats/inc")

# Some files must be processed by Qt's moc.
unset(QT_WRAPPED_SOURCES)
qt4_wrap_cpp(QT_WRAPPED_SOURCES
  "${INC_DIR}/worker.hh")

# Stats module.
set(STATS "05-stats")
set(STATS "${STATS}" PARENT_SCOPE)
add_library("${STATS}" SHARED
  ${QT_WRAPPED_SOURCES}
  # Sources.
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/worker.cc"
  # Headers.
  "${INC_DIR}/worker.hh"
)
target_link_libraries("${STATS}" ${QT_LIBRARIES})
set_target_properties("${STATS}" PROPERTIES PREFIX "")

# Install rule.
install(TARGETS "${STATS}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
