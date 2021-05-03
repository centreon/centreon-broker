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

find_package(gtest REQUIRED)
include_directories(${gtest_INCLUDE_DIRS})
link_directories(${gtest_LIB_DIRS})

# Tests directory.
set(TESTS_DIR ${PROJECT_SOURCE_DIR}/core/test)
set(BAM_TESTS_DIR ${PROJECT_SOURCE_DIR}/bam/test)
set(WDOG_TESTS_DIR ${PROJECT_SOURCE_DIR}/watchdog/test)

# Include directories.
include_directories(${PROJECT_SOURCE_DIR}/bam/inc)
include_directories(${PROJECT_SOURCE_DIR}/bam/test)
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

add_definitions(-DBROKERRPC_TESTS_PATH="${TESTS_DIR}/rpc")

add_executable(rpc_client ${TESTS_DIR}/rpc/client.cc)
target_link_libraries(rpc_client berpc ${gRPC_LIBS} ${absl_LIBS} ${OpenSSL_LIBS} ${c-ares_LIBS} ${ZLIB_LIBS} dl)

add_executable(ut
  # Core sources.
  ${TESTS_DIR}/bbdo/category.cc
  ${TESTS_DIR}/bbdo/output.cc
  ${TESTS_DIR}/bbdo/read.cc
  ${TESTS_DIR}/ceof/ceof_parser/parse.cc
  ${TESTS_DIR}/compression/stream/memory_stream.hh
  ${TESTS_DIR}/compression/stream/read.cc
  ${TESTS_DIR}/compression/stream/write.cc
  ${TESTS_DIR}/compression/zlib/zlib.cc
  ${TESTS_DIR}/config/init.cc
  ${TESTS_DIR}/config/logger.cc
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
  ${TESTS_DIR}/processing/feeder.cc
  ${TESTS_DIR}/rpc/brokerrpc.cc
  ${TESTS_DIR}/exceptions.cc
  ${TESTS_DIR}/io.cc
  ${TESTS_DIR}/logging.cc
  ${TESTS_DIR}/main.cc
  ${TESTS_DIR}/test_server.cc
  # Module sources.
  ${TESTS_SOURCES}
  )

#set_target_properties("ut" PROPERTIES COMPILE_FLAGS "${MYSQL_CFLAGS}")

target_link_libraries(ut rokerbase roker ${TESTS_LIBRARIES} conflictmgr
	${json11_LIBS} ${asio_LIBS} ${fmt_LIBS} ${spdlog_LIBS} ${gtest_LIBS} ${mariadb-connector-c_LIBS} ${OpenSSL_LIBS} ${gRPC_LIBS} ${absl_LIBS} )

add_test(NAME tests COMMAND ut)

if (WITH_COVERAGE)
  set(COVERAGE_EXCLUDES '*/main.cc' '*/test/*' '/usr/include/*' '${CMAKE_BINARY_DIR}/*')
  SETUP_TARGET_FOR_COVERAGE(
    NAME test-coverage
    EXECUTABLE ut
    DEPENDENCIES ut
  )
endif ()
