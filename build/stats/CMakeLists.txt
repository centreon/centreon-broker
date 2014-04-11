##
## Copyright 2012-2013 Merethis
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
set(COMMON_INC_DIR "${PROJECT_SOURCE_DIR}/common/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/stats/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/stats/test")
include_directories("${PROJECT_SOURCE_DIR}/stats/inc")
include_directories("${PROJECT_SOURCE_DIR}/dumper/inc")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${COMMON_INC_DIR}")

# Some files must be processed by Qt's moc.
unset(QT_WRAPPED_SOURCES)
qt4_wrap_cpp(QT_WRAPPED_SOURCES
  "${INC_DIR}/generator.hh"
  "${INC_DIR}/worker.hh")

# Stats module.
set(STATS "15-stats")
set(STATS "${STATS}" PARENT_SCOPE)
add_library("${STATS}" SHARED
  ${QT_WRAPPED_SOURCES}
  # Sources.
  "${SRC_DIR}/builder.cc"
  "${SRC_DIR}/config.cc"
  "${SRC_DIR}/generator.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/metric.cc"
  "${SRC_DIR}/parser.cc"
  "${SRC_DIR}/worker.cc"
  # Headers.
  "${INC_DIR}/builder.hh"
  "${INC_DIR}/config.hh"
  "${INC_DIR}/generator.hh"
  "${INC_DIR}/metric.hh"
  "${INC_DIR}/parser.hh"
  "${INC_DIR}/worker.hh"
  "${COMMON_INC_DIR}/com/centreon/defines.hh"
)
target_link_libraries("${STATS}" ${QT_LIBRARIES})
set_target_properties("${STATS}" PROPERTIES PREFIX "")
get_property(STATS_MODULE_PATH
  TARGET "${STATS}"
  PROPERTY LOCATION)
set(STATS_MODULE_PATH "${STATS_MODULE_PATH}" PARENT_SCOPE)

# Install rule.
install(TARGETS "${STATS}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
