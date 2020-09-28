##
## Copyright 2011-2013 Centreon
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/storage/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/storage/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/storage/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/rrd/inc")
set(INC_DIR "${INC_DIR}/com/centreon/broker/storage")

# conflict_manager library.
add_library("conflictmgr" STATIC
  # Sources.
  "${SRC_DIR}/conflict_manager.cc"
  "${SRC_DIR}/conflict_manager_sql.cc"
  "${SRC_DIR}/conflict_manager_storage.cc"
  "${SRC_DIR}/stored_timestamp.cc"

  # Headers.
  "${INC_DIR}/conflict_manager.hh"
  "${INC_DIR}/stored_timestamp.hh"
)
set(CONFLICTMGR "conflictmgr" PARENT_SCOPE)
set_target_properties(conflictmgr PROPERTIES
    COMPILE_FLAGS "${MYSQL_CFLAGS} -fPIC")

# Storage module.
set(STORAGE "20-storage")
set(STORAGE "${STORAGE}" PARENT_SCOPE)
add_library("${STORAGE}" SHARED
  # Sources.
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/exceptions/perfdata.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/index_mapping.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/metric.cc"
  "${SRC_DIR}/metric_mapping.cc"
  "${SRC_DIR}/parser.cc"
  "${SRC_DIR}/perfdata.cc"
  "${SRC_DIR}/rebuild.cc"
  "${SRC_DIR}/rebuilder.cc"
  "${SRC_DIR}/remove_graph.cc"
  "${SRC_DIR}/status.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/events.hh"
  "${INC_DIR}/exceptions/perfdata.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/index_mapping.hh"
  "${INC_DIR}/internal.hh"
  "${INC_DIR}/metric.hh"
  "${INC_DIR}/metric_mapping.hh"
  "${INC_DIR}/parser.hh"
  "${INC_DIR}/perfdata.hh"
  "${INC_DIR}/rebuild.hh"
  "${INC_DIR}/rebuilder.hh"
  "${INC_DIR}/remove_graph.hh"
  "${INC_DIR}/status.hh"
  "${INC_DIR}/stream.hh"
)
set_target_properties("${STORAGE}" PROPERTIES
    PREFIX ""
    COMPILE_FLAGS "${MYSQL_CFLAGS} -fPIC")
target_link_libraries(${STORAGE} conflictmgr)

# Testing.
if (WITH_TESTING)
  if (WITH_SQL_TESTS)
    set(TEST_SOURCES ${TESTS_SOURCES} ${TEST_DIR}/conflict_manager.cc)
  endif (WITH_SQL_TESTS)

  # Testing.
  set(
    TESTS_SOURCES
    ${TESTS_SOURCES}
    ${TEST_DIR}/connector.cc
    ${TEST_DIR}/metric.cc
    ${TEST_DIR}/perfdata.cc
    ${TEST_DIR}/rebuild.cc
    ${TEST_DIR}/remove_graph.cc
    ${TEST_DIR}/status.cc
    PARENT_SCOPE
  )
  set(
    TESTS_LIBRARIES
    ${TESTS_LIBRARIES}
    ${STORAGE}
    PARENT_SCOPE
  )
endif()

# Install rule.
install(TARGETS "${STORAGE}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
