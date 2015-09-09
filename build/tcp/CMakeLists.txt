##
## Copyright 2011 Centreon
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## For more information : contact@centreon.com
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
