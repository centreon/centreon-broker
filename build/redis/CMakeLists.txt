##
## Copyright 2018 Centreon
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

set(INC_DIR "${PROJECT_SOURCE_DIR}/redis/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/redis/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/redis/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
set(INC_DIR "${INC_DIR}/com/centreon/broker/redis")

# Storage module.
set(REDIS "70-redis")
set(REDIS "${REDIS}" PARENT_SCOPE)
add_library("${REDIS}" SHARED
  # Sources.
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/redisdb.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/redisdb.hh"
  "${INC_DIR}/stream.hh"
)
target_link_libraries("${REDIS}" ${QT_LIBRARIES} ${LUA_LIBRARIES})
set_target_properties("${REDIS}" PROPERTIES PREFIX "")

# Testing.
set(
  TESTS_SOURCES
  ${TESTS_SOURCES}
  "${TEST_DIR}/redisdb.cc"
  PARENT_SCOPE
)
set(
  TESTS_LIBRARIES
  ${TESTS_LIBRARIES}
  "${REDIS}"
  PARENT_SCOPE
)

# Install rule.
install(TARGETS "${REDIS}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
