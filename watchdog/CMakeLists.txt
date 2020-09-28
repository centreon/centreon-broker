##
## Copyright 2014-2015 Centreon
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/watchdog/inc/com/centreon/broker/watchdog")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/watchdog/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/watchdog/test")
include_directories(${PROJECT_SOURCE_DIR}/watchdog/inc/
                    ${PROJECT_SOURCE_DIR}/neb/inc/
)

# Watchdog binary.
set(WATCHDOG "cbwd")
set(WATCHDOG "${WATCHDOG}" PARENT_SCOPE)
add_executable("${WATCHDOG}"
  # Sources.
  ${SRC_DIR}/main.cc
  ${SRC_DIR}/configuration.cc
  ${SRC_DIR}/configuration_parser.cc
  ${SRC_DIR}/instance.cc
  ${SRC_DIR}/instance_configuration.cc
  # Headers.
  ${INC_DIR}/configuration.hh
  ${INC_DIR}/configuration_parser.hh
  ${INC_DIR}/instance.hh
  ${INC_DIR}/instance_configuration.hh
)
target_link_libraries(${WATCHDOG} pthread ${fmt_LIBS} ${spdlog_LIBS} ${json11_LIBS})

if (WITH_WATCHDOG_TESTS)
  # Testing.
  set(
    TESTS_SOURCES
    ${TESTS_SOURCES}
    "${TEST_DIR}/watchdog.cc"
    PARENT_SCOPE
  )
  set(
    TESTS_LIBRARIES
    ${TESTS_LIBRARIES}
    "${STORAGE}"
    PARENT_SCOPE
  )
endif (WITH_WATCHDOG_TESTS)

# Install rule.
install(TARGETS "${WATCHDOG}"
  RUNTIME DESTINATION "${PREFIX_BIN}"
)
