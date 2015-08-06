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
set(INC_DIR "${PROJECT_SOURCE_DIR}/tcp/inc/com/centreon/broker/tcp")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/tcp/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/tcp/test")
include_directories("${PROJECT_SOURCE_DIR}/tcp/inc")

# Some files must be processed by Qt's moc.
unset(QT_WRAPPED_SOURCES)
if ("${QT_VERSION}" VERSION_GREATER "4.3.0")
  qt4_wrap_cpp(QT_WRAPPED_SOURCES
    "${INC_DIR}/acceptor.hh")
else ()
  qt4_wrap_cpp(QT_WRAPPED_SOURCES
    "${INC_DIR}/acceptor.hh")
endif ()

# Sources.
set(SOURCES
  "${SRC_DIR}/acceptor.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/server_socket.cc"
  "${SRC_DIR}/stream.cc"
)
# Headers.
set(HEADERS
  "${INC_DIR}/acceptor.hh"
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/server_socket.hh"
  "${INC_DIR}/socket_parent.hh"
  "${INC_DIR}/stream.hh"
)

# TCP module.
set(TCP "50-tcp")
set(TCP "${TCP}" PARENT_SCOPE)
add_library("${TCP}" SHARED
  ${SOURCES}
  ${HEADERS}
  ${QT_WRAPPED_SOURCES}
)
target_link_libraries("${TCP}" ${QT_LIBRARIES})
set_target_properties("${TCP}" PROPERTIES PREFIX "")
get_property(TCP_MODULE_PATH
  TARGET "${TCP}"
  PROPERTY LOCATION)
set(TCP_MODULE_PATH "${TCP_MODULE_PATH}" PARENT_SCOPE)

# Testing.
if (WITH_TESTING)
  include_directories("${PROJECT_SOURCE_DIR}/tcp")
  add_library("ccb_tcp" STATIC "${TEST_DIR}/common.cc")
  set(TEST_LIBRARIES "ccb_tcp" "${TCP}" "roker")

  # Acceptor tests.
  #   Concurrent close.
  set(TEST_NAME "tcp_acceptor_close_concurrent")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/acceptor/close_concurrent.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Close with open stream.
  set(TEST_NAME "tcp_acceptor_close_with_open_stream")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/acceptor/close_with_open_stream.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Connector tests.
  #   Concurrent close.
  set(TEST_NAME "tcp_connector_close_concurrent")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/connector/close_concurrent.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
endif ()

# Install rule.
install(TARGETS "${TCP}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
