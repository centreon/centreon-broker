##
## Copyright 2013 Centreon
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/tls/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/tls/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/tls/test")
include_directories(${INC_DIR} ${PROJECT_SOURCE_DIR}/neb/inc/)
set(INC_DIR "${INC_DIR}/com/centreon/broker/tls")

# TLS module.
set(TLS "60-tls")
set(TLS "${TLS}" PARENT_SCOPE)
add_library("${TLS}" SHARED
  # Sources
  "${SRC_DIR}/acceptor.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/internal.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/acceptor.hh"
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/internal.hh"
  "${INC_DIR}/stream.hh"
)

target_link_libraries("${TLS}" ${OpenSSL_LIBS})

set_target_properties("${TLS}" PROPERTIES PREFIX "")

# Testing.
if (WITH_TESTING)
  set(
    TESTS_SOURCES
    ${TESTS_SOURCES}
    "${TEST_DIR}/acceptor.cc"
    PARENT_SCOPE
  )
  set(
    TESTS_LIBRARIES
    ${TESTS_LIBRARIES}
    "${TLS}"
    PARENT_SCOPE
  )

  add_executable(anon-test
      "${TEST_DIR}/anon-test.cc")
  target_link_libraries(anon-test ${OpenSSL_LIBS} -pthread -ldl)
  add_executable(crypt-test
      "${TEST_DIR}/crypt-test.cc")
  target_link_libraries(crypt-test ${OpenSSL_LIBS} -pthread -ldl ${fmt_LIBS})
endif()
# Install rule.
install(TARGETS "${TLS}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
