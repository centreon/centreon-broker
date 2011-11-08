##
## Copyright 2011 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/local/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/local/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/local/test")
include_directories("${INC_DIR}")

# Local sockets module.
set(LOCAL "50-local")
set(LOCAL "${LOCAL}" PARENT_SCOPE)
add_library("${LOCAL}" SHARED
  # Sources.
  "${SRC_DIR}/acceptor.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/local/acceptor.hh"
  "${INC_DIR}/com/centreon/broker/local/connector.hh"
  "${INC_DIR}/com/centreon/broker/local/factory.hh"
  "${INC_DIR}/com/centreon/broker/local/stream.hh"
)
target_link_libraries("${LOCAL}" ${QT_LIBRARIES})
set_target_properties("${LOCAL}" PROPERTIES PREFIX "")

# Testing.
if (WITH_TESTING)
  include_directories("${PROJECT_SOURCE_DIR}/local")
  set(TEST_LIBRARIES "${LOCAL}" "roker")

  # Acceptor tests.
  #   Concurrent close.
  set(TEST_NAME "local_acceptor_close_concurrent")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/acceptor/close_concurrent.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Connector tests.
  #   Concurrent close.
  set(TEST_NAME "local_connector_close_concurrent")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/connector/close_concurrent.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
endif ()

# Install rule.
install(TARGETS "${LOCAL}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
