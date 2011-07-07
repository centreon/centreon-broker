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
set(INC_DIR "${PROJECT_SOURCE_DIR}/storage/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/storage/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/storage/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/rrd/inc")

# Storage module.
set(STORAGE "20-storage")
add_library("${STORAGE}" MODULE
  # Sources.
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/exceptions/perfdata.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/metric.cc"
  "${SRC_DIR}/parser.cc"
  "${SRC_DIR}/perfdata.cc"
  "${SRC_DIR}/status.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/storage/connector.hh"
  "${INC_DIR}/com/centreon/broker/storage/exceptions/perfdata.hh"
  "${INC_DIR}/com/centreon/broker/storage/factory.hh"
  "${INC_DIR}/com/centreon/broker/storage/metric.hh"
  "${INC_DIR}/com/centreon/broker/storage/parser.hh"
  "${INC_DIR}/com/centreon/broker/storage/perfdata.hh"
  "${INC_DIR}/com/centreon/broker/storage/status.hh"
  "${INC_DIR}/com/centreon/broker/storage/stream.hh"
)
set_target_properties("${STORAGE}" PROPERTIES PREFIX "")

if (WITH_TESTING)
  # Perfdata exception tests.
  #   Throw.
  set(TEST_NAME "storage_exceptions_perfdata_throw")
  add_executable("${TEST_NAME}"
    "${SRC_DIR}/exceptions/perfdata.cc"
    "${TEST_DIR}/exceptions/perfdata/throw.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Rethrow.
  set(TEST_NAME "storage_exceptions_perfdata_rethrow")
  add_executable("${TEST_NAME}"
    "${SRC_DIR}/exceptions/perfdata.cc"
    "${TEST_DIR}/exceptions/perfdata/rethrow.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
endif ()

# Install rule.
install(TARGETS "${STORAGE}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
