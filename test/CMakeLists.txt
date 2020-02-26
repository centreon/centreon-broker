##
## Copyright 2016-2017 Centreon
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

include(ExternalProject)

set(GOOGLE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

if (${BUILD_OFFLINE})
  ExternalProject_Add(googletest
      SOURCE_DIR ${CMAKE_BINARY_DIR}/test/googletest-prefix/src/googletest
      TIMEOUT 10
      INSTALL_COMMAND ""
      LOG_DOWNLOAD ON
      LOG_CONFIGURE ON
      LOG_BUILD ON
      CMAKE_ARGS -DCMAKE_CXX_FLAGS=${GOOGLE_CXX_FLAGS} -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS})
else ()
  ExternalProject_Add(googletest
      GIT_REPOSITORY https://github.com/google/googletest/
      GIT_TAG release-1.10.0
      TIMEOUT 10
      INSTALL_COMMAND ""
      LOG_DOWNLOAD ON
      LOG_CONFIGURE ON
      LOG_BUILD ON
      CMAKE_ARGS -DCMAKE_CXX_FLAGS=${GOOGLE_CXX_FLAGS} -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS})
endif ()

ExternalProject_Get_Property(googletest source_dir)
ExternalProject_Get_Property(googletest binary_dir)

link_directories(${binary_dir}/lib)
include_directories(${source_dir}/googletest/include)

# Tests directory.
set(TESTS_DIR ${PROJECT_SOURCE_DIR}/core/test)
set(BAM_TESTS_DIR ${PROJECT_SOURCE_DIR}/bam/test)
set(WDOG_TESTS_DIR ${PROJECT_SOURCE_DIR}/watchdog/test)

# Include directories.
include_directories(${PROJECT_SOURCE_DIR}/bam/inc)
include_directories(${PROJECT_SOURCE_DIR}/storage/inc)
include_directories(${PROJECT_SOURCE_DIR}/graphite/inc)
include_directories(${PROJECT_SOURCE_DIR}/sql/inc)
include_directories(${PROJECT_SOURCE_DIR}/influxdb/inc)
include_directories(${PROJECT_SOURCE_DIR}/lua/inc)
include_directories(${PROJECT_SOURCE_DIR}/neb/inc)
include_directories(${PROJECT_SOURCE_DIR}/rrd/inc)
include_directories(${PROJECT_SOURCE_DIR}/simu/inc)
include_directories(${PROJECT_SOURCE_DIR}/stats/inc)
include_directories(${PROJECT_SOURCE_DIR}/tcp/inc)

# needed by module.cc
add_definitions(-DCENTREON_BROKER_TEST_MODULE_PATH="${CMAKE_BINARY_DIR}/test/")
add_definitions(-DCENTREON_BROKER_BAM_TEST_PATH="${BAM_TESTS_DIR}")
add_definitions(-DCENTREON_BROKER_WD_TEST="${WDOG_TESTS_DIR}")
add_definitions(-DCENTREON_BROKER_LUA_SCRIPT_PATH="${PROJECT_SOURCE_DIR}/lua/test")
add_definitions(-DCENTREON_BROKER_NEB_TEST="${PROJECT_SOURCE_DIR}/neb/test")

add_library(null_module SHARED ${TEST_DIR}/modules/null_module.cc)
add_library(bad_version_module SHARED ${TEST_DIR}/modules/bad_version_module.cc)
# Unit test executable.
if (WITH_SQL_TESTS)
  set(TEST_SOURCES ${TESTS_SOURCES} ${TESTS_DIR}/mysql/mysql.cc)
endif (WITH_SQL_TESTS)

add_executable(ut
  # Core sources.
  ${TESTS_DIR}/bbdo/input_buffer/erase.cc
  ${TESTS_DIR}/bbdo/input_buffer/extract.cc
  ${TESTS_DIR}/bbdo/output.cc
  ${TESTS_DIR}/bbdo/read.cc
  ${TESTS_DIR}/ceof/ceof_parser/parse.cc
  ${TESTS_DIR}/compression/stream/memory_stream.hh
  ${TESTS_DIR}/compression/stream/read.cc
  ${TESTS_DIR}/compression/stream/write.cc
  ${TESTS_DIR}/compression/zlib/zlib.cc
  ${TESTS_DIR}/config/logger.cc
  ${TESTS_DIR}/config/parser.cc
  ${TESTS_DIR}/config/parser.cc
  ${TESTS_DIR}/file/splitter/concurrent.cc
  ${TESTS_DIR}/file/splitter/default.cc
  ${TESTS_DIR}/file/splitter/more_than_max_size.cc
  ${TESTS_DIR}/file/splitter/permission_denied.cc
  ${TESTS_DIR}/file/splitter/resume.cc
  ${TESTS_DIR}/file/splitter/split.cc
  ${TESTS_DIR}/misc/exec.cc
  ${TESTS_DIR}/misc/filesystem.cc
  ${TESTS_DIR}/misc/math.cc
  ${TESTS_DIR}/misc/misc.cc
  ${TESTS_DIR}/misc/string.cc
  ${TESTS_DIR}/misc/stringifier.cc
  ${TESTS_DIR}/modules/module.cc
  ${TESTS_DIR}/multiplexing/engine/hook.cc
  ${TESTS_DIR}/multiplexing/engine/hooker.cc
  ${TESTS_DIR}/multiplexing/engine/start_stop.cc
  ${TESTS_DIR}/multiplexing/engine/unhook.cc
  ${TESTS_DIR}/multiplexing/muxer/read.cc
  ${TESTS_DIR}/multiplexing/publisher/read.cc
  ${TESTS_DIR}/multiplexing/publisher/write.cc
  ${TESTS_DIR}/multiplexing/subscriber/ctor_default.cc
  ${TESTS_DIR}/processing/acceptor.cc
  ${TESTS_DIR}/processing/temporary_endpoint.cc
  ${TESTS_DIR}/processing/temporary_stream.cc
  ${TESTS_DIR}/processing/feeder.cc
  ${TESTS_DIR}/processing/thread.cc
  ${TESTS_DIR}/exceptions.cc
  ${TESTS_DIR}/io.cc
  ${TESTS_DIR}/logging.cc
  ${TESTS_DIR}/main.cc
  ${TESTS_DIR}/test_server.cc
  # Module sources.
  ${TESTS_SOURCES}
  )

set_target_properties("ut" PROPERTIES COMPILE_FLAGS "${MYSQL_CFLAGS}")

add_dependencies(ut googletest)
target_link_libraries(ut gtest rokerbase roker ${TESTS_LIBRARIES}
    conflictmgr)

add_test(NAME tests COMMAND ut)

if (WITH_COVERAGE)
  set(COVERAGE_EXCLUDES '*/main.cc' '*/test/*' '/usr/include/*' '${CMAKE_BINARY_DIR}/*')
  SETUP_TARGET_FOR_COVERAGE(
    NAME test-coverage
    EXECUTABLE ut
    DEPENDENCIES ut
  )
endif ()
