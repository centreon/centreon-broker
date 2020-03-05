##
## Copyright 2011-2015 Centreon
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

set(INC_DIR "${PROJECT_SOURCE_DIR}/sql/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/sql/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/sql/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/correlation/inc")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")

# SQL module.
set(SQL "80-sql")
set(SQL "${SQL}" PARENT_SCOPE)
add_library("${SQL}" SHARED
  # Sources.
  "${SRC_DIR}/cleanup.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/sql/cleanup.hh"
  "${INC_DIR}/com/centreon/broker/sql/connector.hh"
  "${INC_DIR}/com/centreon/broker/sql/factory.hh"
  "${INC_DIR}/com/centreon/broker/sql/stream.hh"
)
add_dependencies("${SQL}" json11_lib spdlog_lib)
set_target_properties("${SQL}" PROPERTIES
    PREFIX ""
    COMPILE_FLAGS "${MYSQL_CFLAGS} -fPIC")

## Testing.
#if (WITH_TESTING)
#  if (WITH_SQL_TESTS)
#    set(
#      TESTS_SOURCES
#      ${TESTS_SOURCES}
#      "${TEST_DIR}/conflict_manager.cc"
#      PARENT_SCOPE
#    )
#    set(
#      TESTS_LIBRARIES
#      ${TESTS_LIBRARIES}
#      "${SQL}"
#      PARENT_SCOPE
#    )
#  endif()
#endif()

# Install rule.
install(TARGETS "${SQL}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
