##
## Copyright 2009-2020 Centreon
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

#
# Global settings.
#

# Set necessary settings.
cmake_minimum_required(VERSION 2.8)
project("Centreon Broker" C CXX)
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR "You can build broker with g++ or clang++. CMake will exit.")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions("-D_GLIBCXX_USE_CXX11_ABI=0")

# With ASIO DEBUGGING ENABLED
option(WITH_DEBUG_ASIO "Add the Asio debugging flags." OFF)
if (WITH_DEBUG_ASIO)
  set(CMAKE_BUILD_TYPE Debug)
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DASIO_ENABLE_BUFFER_DEBUGGING -DASIO_ENABLE_HANDLER_TRACKING")
endif ()

# With libasan
option(WITH_ASAN "Add the libasan to check memory leaks and other memory issues." OFF)
if (WITH_ASAN)
  set(CMAKE_BUILD_TYPE Debug)
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif ()

include_directories("${PROJECT_SOURCE_DIR}/core/inc")
set(INC_DIR "${PROJECT_SOURCE_DIR}/core/inc/com/centreon/broker")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/core/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/core/test")
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

add_custom_command(
  DEPENDS ${SRC_DIR}/database/table_max_size.py "${PROJECT_SOURCE_DIR}/simu/docker/installBroker.sql" "${PROJECT_SOURCE_DIR}/simu/docker/partitioning-logs.xml" "${PROJECT_SOURCE_DIR}/simu/docker/bam.sql" "${PROJECT_SOURCE_DIR}/simu/docker/createTablesCentstorage.sql"
  COMMENT "Generating table_max_size.hh"
  OUTPUT ${INC_DIR}/database/table_max_size.hh
  COMMAND python3
  ARGS ${SRC_DIR}/database/table_max_size.py ${INC_DIR}/database/table_max_size.hh "${PROJECT_SOURCE_DIR}/simu/docker/installBroker.sql" "${PROJECT_SOURCE_DIR}/simu/docker/partitioning-logs.xml" "${PROJECT_SOURCE_DIR}/simu/docker/bam.sql" "${PROJECT_SOURCE_DIR}/simu/docker/createTablesCentstorage.sql"
)

add_custom_target(table_max_size DEPENDS ${INC_DIR}/database/table_max_size.hh)

set_source_files_properties(${INC_DIR}/database/table_max_size.hh PROPERTIES GENERATED TRUE)

set(protobuf_MODULE_COMPATIBLE True)
find_package(json11 REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(asio REQUIRED)
find_package(protobuf REQUIRED)
find_package(protoc_installer REQUIRED)
find_package(gRPC REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(c-ares REQUIRED)
find_package(ZLIB REQUIRED)

add_definitions(${spdlog_DEFINITIONS})

include_directories(${json11_INCLUDE_DIRS})
include_directories(${fmt_INCLUDE_DIRS})
include_directories(${spdlog_INCLUDE_DIRS})
include_directories(${asio_INCLUDE_DIRS})
include_directories(${protobuf_INCLUDE_DIRS})
include_directories(${gRPC_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})

link_directories(${json11_LIB_DIRS})
link_directories(${fmt_LIB_DIRS})
link_directories(${spdlog_LIB_DIRS})
link_directories(${protobuf_LIB_DIRS})
link_directories(${gRPC_LIB_DIRS})
link_directories(${c-ares_LIB_DIRS})
link_directories(${OpenSSL_LIB_DIRS})
link_directories(${ZLIB_LIB_DIRS})

message(STATUS "Using protobuf ${gRPC_VERSION}")
set(PROTOBUF_PREFIX "${protoc_installer_LIB_DIRS}/..")
message(STATUS "${PROTOBUF_PREFIX}/bin/protoc")
set(GRPC_PREFIX "${gRPC_LIB_DIRS}/..")

add_custom_command(
  DEPENDS ${SRC_DIR}/broker.proto
  COMMENT "Generating interface files of the proto file (grpc)"
  OUTPUT ${SRC_DIR}/broker.grpc.pb.cc ${SRC_DIR}/broker.grpc.pb.h
  COMMAND ${PROTOBUF_PREFIX}/bin/protoc
  ARGS --plugin=protoc-gen-grpc=${GRPC_PREFIX}/bin/grpc_cpp_plugin --proto_path=${SRC_DIR} --grpc_out="${SRC_DIR}" ${SRC_DIR}/broker.proto

  DEPENDS ${SRC_DIR}/broker.proto
  COMMENT "Generating interface files of the proto file (protobuf)"
  OUTPUT ${SRC_DIR}/broker.pb.cc ${SRC_DIR}/broker.pb.h
  COMMAND ${PROTOBUF_PREFIX}/bin/protoc
  ARGS --cpp_out="${SRC_DIR}" --proto_path=${SRC_DIR} ${SRC_DIR}/broker.proto
)

include_directories(${SRC_DIR})

add_library(
  berpc
  STATIC

  ${SRC_DIR}/broker.grpc.pb.cc
  ${SRC_DIR}/broker.pb.cc
  ${SRC_DIR}/broker.grpc.pb.h
  ${SRC_DIR}/broker.pb.h)
target_link_libraries(berpc ${protobuf_LIBS})
set_target_properties(berpc PROPERTIES COMPILE_FLAGS "-fPIC")

# Version.
set(CENTREON_BROKER_MAJOR 20)
set(CENTREON_BROKER_MINOR 04)
set(CENTREON_BROKER_PATCH 14)
if (CENTREON_BROKER_PRERELEASE)
  set(CENTREON_BROKER_VERSION "${CENTREON_BROKER_MAJOR}.${CENTREON_BROKER_MINOR}.${CENTREON_BROKER_PATCH}-${CENTREON_BROKER_PRERELEASE}")
else ()
  set(CENTREON_BROKER_VERSION "${CENTREON_BROKER_MAJOR}.${CENTREON_BROKER_MINOR}.${CENTREON_BROKER_PATCH}")
endif ()
add_definitions(-DCENTREON_BROKER_VERSION=\"${CENTREON_BROKER_VERSION}\")
configure_file(
  ${INC_DIR}/version.hh.in
  ${INC_DIR}/version.hh
  @ONLY
)

include(cmake/tool.cmake)

#
# Check and/or find required components.
#

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
add_definitions("-DASIO_STANDALONE")

# Find zlib.
find_package(ZLIB REQUIRED)
find_package(PkgConfig REQUIRED)

# Find MySQL/Mariadb devel package
pkg_check_modules(_libmariadb libmariadb)
if (_libmariadb_FOUND)
  set(MYSQL_CFLAGS ${_libmariadb_CFLAGS})
  set(MYSQL_LIBS ${_libmariadb_LDFLAGS})
else ()
  pkg_check_modules(_mariadb mariadb)
  if (_mariadb_FOUND)
    set(MYSQL_CFLAGS ${_mariadb_CFLAGS})
    set(MYSQL_LIBS ${_mariadb_LDFLAGS})
  else ()
    pkg_check_modules(_mysql mysql)
    if (_mysql_FOUND)
      set(MYSQL_CFLAGS ${_mysql_CFLAGS})
      set(MYSQL_LIBS ${_mysql_LDFLAGS})
    else ()
      message(SEND_ERROR "MariaDB devel package is not installed")
    endif ()
  endif ()
endif ()

string(REPLACE "-" " -" MYSQL_CFLAGS ${MYSQL_CFLAGS})
message(STATUS ${MYSQL_CFLAGS})
message(STATUS ${MYSQL_LIBS})

#
# Get distributions name
#
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(STATUS "Attempting to determine OS distributor.")
  execute_process(COMMAND "lsb_release" "--short" "--id"
    RESULT_VARIABLE RETCODE
    OUTPUT_VARIABLE OS_DISTRIBUTOR
    ERROR_QUIET)
  if (RETCODE EQUAL 0)
    string(REGEX REPLACE "\n$" "" OS_DISTRIBUTOR "${OS_DISTRIBUTOR}")
  else ()
    message(WARNING "lsb_release in not installed")
    set(OS_DISTRIBUTOR "${CMAKE_SYSTEM_NAME}")
  endif ()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(OS_DISTRIBUTOR "${CMAKE_SYSTEM_NAME}")
endif ()


#
# Options.
#

# Main directory.
if (WITH_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${WITH_PREFIX}")
endif ()

# Executable directory.
if (WITH_PREFIX_BIN)
  set(PREFIX_BIN "${WITH_PREFIX_BIN}")
else ()
  set(PREFIX_BIN "${CMAKE_INSTALL_PREFIX}/bin")
endif ()

# Configuration directory.
if (WITH_PREFIX_CONF)
  set(PREFIX_CONF "${WITH_PREFIX_CONF}")
else ()
  set(PREFIX_CONF "${CMAKE_INSTALL_PREFIX}/etc")
endif ()

# Library directory.
if (WITH_PREFIX_LIB)
  set(PREFIX_LIB "${WITH_PREFIX_LIB}")
else ()
  set(PREFIX_LIB "${CMAKE_INSTALL_PREFIX}/lib")
endif ()

# Modules directory.
if (WITH_PREFIX_MODULES)
  set(PREFIX_MODULES "${WITH_PREFIX_MODULES}")
else ()
  set(PREFIX_MODULES "${PREFIX_LIB}/centreon-broker")
endif ()

# var directory.
if (WITH_PREFIX_VAR)
  set(PREFIX_VAR "${WITH_PREFIX_VAR}")
else ()
  set(PREFIX_VAR "${CMAKE_INSTALL_PREFIX}/var")
endif ()

# Development headers directory.
if (WITH_PREFIX_INC)
  set(PREFIX_INC "${WITH_PREFIX_INC}")
else ()
  set(PREFIX_INC "${CMAKE_INSTALL_PREFIX}/include/centreon-broker")
endif ()

# User.
if (WITH_USER)
  set(USER "${WITH_USER}")
else ()
  set(USER "root")
endif ()

# Group.
if (WITH_GROUP)
  set(GROUP "${WITH_GROUP}")
else ()
  set(GROUP "root")
endif ()

# Code coverage on unit tests
option(WITH_COVERAGE "Add code coverage on unit tests." OFF)
if (WITH_TESTING AND WITH_COVERAGE)
  set(CMAKE_BUILD_TYPE "Debug")
  include(cmake/CodeCoverage.cmake)
  APPEND_COVERAGE_COMPILER_FLAGS()
endif ()

# Set startup script to auto if not defined.
if (NOT WITH_STARTUP_SCRIPT)
  set(WITH_STARTUP_SCRIPT "auto")
endif ()

# Check which startup script to use.
if (WITH_STARTUP_SCRIPT STREQUAL "auto")
  if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(WITH_STARTUP_SCRIPT "sysv")
  else ()
    message(STATUS "Centreon Broker does not provide startup script for ${CMAKE_SYSTEM_NAME}.")
    set(WITH_STARTUP_SCRIPT "no")
  endif ()
endif ()

# Startup dir.
if (WITH_STARTUP_SCRIPT STREQUAL "sysv"
  OR WITH_STARTUP_SCRIPT STREQUAL "systemd")
  # Set destination directory.
  if (WITH_STARTUP_DIR)
    set(STARTUP_DIR "${WITH_STARTUP_DIR}")
  else ()
    set(STARTUP_DIR "/etc/init.d")
  endif ()
endif ()

# Configure files.
if (WITH_DAEMONS)
  if (WITH_STARTUP_SCRIPT STREQUAL "systemd")
    configure_file("${PROJECT_SOURCE_DIR}/script/systemd/cbd.service.in"
      "${PROJECT_SOURCE_DIR}/script/cbd.service"
      @ONLY)
    install(FILES "${PROJECT_SOURCE_DIR}/script/cbd.service"
      DESTINATION "${STARTUP_DIR}")
  elseif (OS_DISTRIBUTOR STREQUAL "CentOS" OR OS_DISTRIBUTOR STREQUAL "RedHat")
    configure_file("${PROJECT_SOURCE_DIR}/script/redhat/cbd.init.d.in"
      "${PROJECT_SOURCE_DIR}/script/cbd.init"
      @ONLY)
  elseif (OS_DISTRIBUTOR STREQUAL "Debian" OR OS_DISTRIBUTOR STREQUAL "Ubuntu")
    configure_file("${PROJECT_SOURCE_DIR}/script/debian/cbd.init.d.in"
      "${PROJECT_SOURCE_DIR}/script/cbd.init"
      @ONLY)
    configure_file("${PROJECT_SOURCE_DIR}/script/debian/cbd.default.in"
      "${PROJECT_SOURCE_DIR}/script/cbd.default"
      @ONLY)
    install(FILES "${PROJECT_SOURCE_DIR}/script/cbd.default"
      DESTINATION "/etc/default"
      RENAME "cbd")
  else ()
    configure_file("${PROJECT_SOURCE_DIR}/script/other/cbd.init.d.in"
      "${PROJECT_SOURCE_DIR}/script/cbd.init"
      @ONLY)
  endif ()

  foreach (DAEMON_NAME IN LISTS WITH_DAEMONS)
    set(DAEMONS_CONFIGURATION
      "${DAEMONS_CONFIGURATION}  \"cbd\": [ { \"name\": \"${DAEMON_NAME}\", \"configuration_file\": \"${PREFIX_CONF}/${DAEMON_NAME}.json\", \"run\": true, \"reload\": true } ],\n")
  endforeach ()

  configure_file("${PROJECT_SOURCE_DIR}/script/watchdog.json.in"
    "${PROJECT_SOURCE_DIR}/script/watchdog.json"
    @ONLY)
  install(FILES "${PROJECT_SOURCE_DIR}/script/watchdog.json"
    DESTINATION "${PREFIX_CONF}")
  if (WITH_STARTUP_SCRIPT STREQUAL "sysv")
    install(PROGRAMS "${PROJECT_SOURCE_DIR}/script/cbd.init"
      DESTINATION "${STARTUP_DIR}"
      RENAME "cbd")
  endif ()
endif ()

if (WITH_CONFIG_FILES)
  configure_file("${PROJECT_SOURCE_DIR}/config/poller-module.json.in"
    "${PROJECT_SOURCE_DIR}/config/poller-module.json"
    @ONLY)
  configure_file("${PROJECT_SOURCE_DIR}/config/central-broker.json.in"
    "${PROJECT_SOURCE_DIR}/config/central-broker.json"
    @ONLY)
  configure_file("${PROJECT_SOURCE_DIR}/config/central-rrd.json.in"
    "${PROJECT_SOURCE_DIR}/config/central-rrd.json"
    @ONLY)

  install(FILES "${PROJECT_SOURCE_DIR}/config/poller-module.json"
    DESTINATION "${PREFIX_CONF}")
  install(FILES "${PROJECT_SOURCE_DIR}/config/central-rrd.json"
    DESTINATION "${PREFIX_CONF}")
  install(FILES "${PROJECT_SOURCE_DIR}/config/central-broker.json"
    DESTINATION "${PREFIX_CONF}")
endif ()

# Monitoring engine (for testing).
if (WITH_MONITORING_ENGINE)
  set(MONITORING_ENGINE_ADDITIONAL "")
  set(MONITORING_ENGINE "${WITH_MONITORING_ENGINE}")
  if (WITH_MONITORING_ENGINE_MODULES)
    foreach (MODULE IN LISTS WITH_MONITORING_ENGINE_MODULES)
      set(MONITORING_ENGINE_ADDITIONAL
        "${MONITORING_ENGINE_ADDITIONAL}broker_module=${MODULE}\\n")
    endforeach ()
  endif ()
  if (WITH_MONITORING_ENGINE_INTERVAL_LENGTH)
    set(MONITORING_ENGINE_INTERVAL_LENGTH "${WITH_MONITORING_ENGINE_INTERVAL_LENGTH}")
  else ()
    set(MONITORING_ENGINE_INTERVAL_LENGTH 1)
  endif ()
endif ()

# Broker vars
configure_file(
  "${INC_DIR}/vars.hh.in"
  "${INC_DIR}/vars.hh"
  "@ONLY"
)

# Core library.
set(LIBROKER_SOURCES
  # Sources.
  ${SRC_DIR}/bbdo/acceptor.cc
  ${SRC_DIR}/bbdo/ack.cc
  ${SRC_DIR}/bbdo/connector.cc
  ${SRC_DIR}/bbdo/factory.cc
#  ${SRC_DIR}/bbdo/input.cc
  ${SRC_DIR}/bbdo/internal.cc
#  ${SRC_DIR}/bbdo/output.cc
  ${SRC_DIR}/bbdo/stream.cc
  ${SRC_DIR}/bbdo/version_response.cc
  ${SRC_DIR}/broker_impl.cc
  ${SRC_DIR}/brokerrpc.cc
  ${SRC_DIR}/ceof/ceof_deserializer.cc
  ${SRC_DIR}/ceof/ceof_iterator.cc
  ${SRC_DIR}/ceof/ceof_parser.cc
  ${SRC_DIR}/ceof/ceof_serializer.cc
  ${SRC_DIR}/ceof/ceof_token.cc
  ${SRC_DIR}/ceof/ceof_writer.cc
  ${SRC_DIR}/compression/factory.cc
  ${SRC_DIR}/compression/internal.cc
  ${SRC_DIR}/compression/opener.cc
  ${SRC_DIR}/compression/stack_array.cc
  ${SRC_DIR}/compression/stream.cc
  ${SRC_DIR}/compression/zlib.cc
  ${SRC_DIR}/config/applier/endpoint.cc
  ${SRC_DIR}/config/applier/modules.cc
  ${SRC_DIR}/config/applier/state.cc
  ${SRC_DIR}/config/endpoint.cc
  ${SRC_DIR}/config/logger.cc
  ${SRC_DIR}/config/parser.cc
  ${SRC_DIR}/config/state.cc
  ${SRC_DIR}/database_config.cc
  ${SRC_DIR}/file/cfile.cc
  ${SRC_DIR}/file/directory_event.cc
  ${SRC_DIR}/file/directory_watcher.cc
  ${SRC_DIR}/file/factory.cc
  ${SRC_DIR}/file/fifo.cc
  ${SRC_DIR}/file/internal.cc
  ${SRC_DIR}/file/opener.cc
  ${SRC_DIR}/file/splitter.cc
  ${SRC_DIR}/file/stream.cc
  ${SRC_DIR}/instance_broadcast.cc
  ${SRC_DIR}/io/data.cc
  ${SRC_DIR}/io/endpoint.cc
  ${SRC_DIR}/io/events.cc
  ${SRC_DIR}/io/factory.cc
  ${SRC_DIR}/io/protocols.cc
  ${SRC_DIR}/io/raw.cc
  ${SRC_DIR}/io/stream.cc
  ${SRC_DIR}/log_v2.cc
  ${SRC_DIR}/logging/file.cc
  ${SRC_DIR}/logging/logger.cc
  ${SRC_DIR}/logging/logging.cc
  ${SRC_DIR}/logging/manager.cc
  ${SRC_DIR}/logging/syslogger.cc
  ${SRC_DIR}/logging/temp_logger.cc
  ${SRC_DIR}/mapping/entry.cc
  ${SRC_DIR}/misc/diagnostic.cc
  ${SRC_DIR}/misc/filesystem.cc
  ${SRC_DIR}/misc/global_lock.cc
  ${SRC_DIR}/misc/misc.cc
  ${SRC_DIR}/misc/processing_speed_computer.cc
  ${SRC_DIR}/misc/string.cc
  ${SRC_DIR}/misc/stringifier.cc
  ${SRC_DIR}/misc/tokenizer.cc
  ${SRC_DIR}/misc/uuid.cc
  ${SRC_DIR}/misc/variant.cc
  ${SRC_DIR}/modules/handle.cc
  ${SRC_DIR}/modules/loader.cc
  ${SRC_DIR}/multiplexing/engine.cc
  ${SRC_DIR}/multiplexing/hooker.cc
  ${SRC_DIR}/multiplexing/muxer.cc
  ${SRC_DIR}/multiplexing/publisher.cc
  ${SRC_DIR}/mysql.cc
  ${SRC_DIR}/pool.cc
  ${SRC_DIR}/database/mysql_bind.cc
  ${SRC_DIR}/database/mysql_column.cc
  ${SRC_DIR}/mysql_manager.cc
  ${SRC_DIR}/database/mysql_result.cc
  ${SRC_DIR}/database/mysql_stmt.cc
  ${SRC_DIR}/mysql_connection.cc
  ${SRC_DIR}/query_preparator.cc
  ${SRC_DIR}/persistent_cache.cc
  ${SRC_DIR}/persistent_file.cc
  ${SRC_DIR}/processing/acceptor.cc
  ${SRC_DIR}/processing/failover.cc
  ${SRC_DIR}/processing/feeder.cc
  ${SRC_DIR}/processing/stat_visitable.cc
  ${SRC_DIR}/stats/helper.cc
  ${SRC_DIR}/time/daterange.cc
  ${SRC_DIR}/time/timeperiod.cc
  ${SRC_DIR}/time/timerange.cc
  ${SRC_DIR}/time/timezone_locker.cc
  ${SRC_DIR}/time/timezone_manager.cc
  # Headers.
  ${INC_DIR}/bbdo/acceptor.hh
  ${INC_DIR}/bbdo/ack.hh
  ${INC_DIR}/bbdo/connector.hh
  ${INC_DIR}/bbdo/factory.hh
#  ${INC_DIR}/bbdo/input.hh
  ${INC_DIR}/bbdo/internal.hh
#  ${INC_DIR}/bbdo/output.hh
  ${INC_DIR}/bbdo/stream.hh
  ${INC_DIR}/bbdo/version_response.hh
  ${INC_DIR}/broker_impl.hh
  ${INC_DIR}/brokerrpc.hh
  ${INC_DIR}/ceof/ceof_deserializer.hh
  ${INC_DIR}/ceof/ceof_iterator.hh
  ${INC_DIR}/ceof/ceof_parser.hh
  ${INC_DIR}/ceof/ceof_serializable.hh
  ${INC_DIR}/ceof/ceof_serializer.hh
  ${INC_DIR}/ceof/ceof_token.hh
  ${INC_DIR}/ceof/ceof_visitor.hh
  ${INC_DIR}/ceof/ceof_writer.hh
  ${INC_DIR}/compression/factory.hh
  ${INC_DIR}/compression/internal.hh
  ${INC_DIR}/compression/opener.hh
  ${INC_DIR}/compression/stack_array.hh
  ${INC_DIR}/compression/stream.hh
  ${INC_DIR}/config/applier/endpoint.hh
  ${INC_DIR}/config/applier/init.hh
  ${INC_DIR}/config/applier/logger.hh
  ${INC_DIR}/config/applier/modules.hh
  ${INC_DIR}/config/applier/state.hh
  ${INC_DIR}/config/endpoint.hh
  ${INC_DIR}/config/logger.hh
  ${INC_DIR}/config/parser.hh
  ${INC_DIR}/config/state.hh
  ${INC_DIR}/database_config.hh
  ${INC_DIR}/exceptions/config.hh
  ${INC_DIR}/exceptions/interrupt.hh
  ${INC_DIR}/exceptions/msg.hh
  ${INC_DIR}/exceptions/shutdown.hh
  ${INC_DIR}/exceptions/timeout.hh
  ${INC_DIR}/file/cfile.hh
  ${INC_DIR}/file/directory_event.hh
  ${INC_DIR}/file/directory_watcher.hh
  ${INC_DIR}/file/factory.hh
  ${INC_DIR}/file/fifo.hh
  ${INC_DIR}/file/fs_file.hh
  ${INC_DIR}/file/internal.hh
  ${INC_DIR}/file/opener.hh
  ${INC_DIR}/file/splitter.hh
  ${INC_DIR}/file/stream.hh
  ${INC_DIR}/instance_broadcast.hh
  ${INC_DIR}/io/data.hh
  ${INC_DIR}/io/endpoint.hh
  ${INC_DIR}/io/event_info.hh
  ${INC_DIR}/io/events.hh
  ${INC_DIR}/io/factory.hh
  ${INC_DIR}/io/protocols.hh
  ${INC_DIR}/io/raw.hh
  ${INC_DIR}/io/stream.hh
  ${INC_DIR}/logging/backend.hh
  ${INC_DIR}/logging/defines.hh
  ${INC_DIR}/logging/file.hh
  ${INC_DIR}/logging/logger.hh
  ${INC_DIR}/logging/logging.hh
  ${INC_DIR}/logging/manager.hh
  ${INC_DIR}/logging/syslogger.hh
  ${INC_DIR}/logging/temp_logger.hh
  ${INC_DIR}/mapping/entry.hh
  ${INC_DIR}/mapping/property.hh
  ${INC_DIR}/mapping/source.hh
  ${INC_DIR}/misc/diagnostic.hh
  ${INC_DIR}/misc/filesystem.hh
  ${INC_DIR}/misc/global_lock.hh
  ${INC_DIR}/misc/misc.hh
  ${INC_DIR}/misc/pair.hh
  ${INC_DIR}/misc/processing_speed_computer.hh
  ${INC_DIR}/misc/shared_mutex.hh
  ${INC_DIR}/misc/string.hh
  ${INC_DIR}/misc/stringifier.hh
  ${INC_DIR}/misc/tokenizer.hh
  ${INC_DIR}/misc/variant.hh
  ${INC_DIR}/modules/handle.hh
  ${INC_DIR}/modules/loader.hh
  ${INC_DIR}/multiplexing/engine.hh
  ${INC_DIR}/multiplexing/hooker.hh
  ${INC_DIR}/multiplexing/muxer.hh
  ${INC_DIR}/multiplexing/publisher.hh
  ${INC_DIR}/multiplexing/subscriber.hh
  ${INC_DIR}/mysql.hh
  ${INC_DIR}/pool.hh
  ${INC_DIR}/database/mysql_bind.hh
  ${INC_DIR}/database/mysql_column.hh
  ${INC_DIR}/database/mysql_error.hh
  ${INC_DIR}/mysql_manager.hh
  ${INC_DIR}/database/mysql_result.hh
  ${INC_DIR}/database/mysql_stmt.hh
  ${INC_DIR}/database/mysql_task.hh
  ${INC_DIR}/database/table_max_size.hh
  ${INC_DIR}/mysql_connection.hh
  ${INC_DIR}/query_preparator.hh
  ${INC_DIR}/persistent_cache.hh
  ${INC_DIR}/persistent_file.hh
  ${INC_DIR}/processing/acceptor.hh
  ${INC_DIR}/processing/failover.hh
  ${INC_DIR}/processing/feeder.hh
  ${INC_DIR}/processing/stat_visitable.hh
  ${INC_DIR}/stats/helper.hh
  ${INC_DIR}/time/daterange.hh
  ${INC_DIR}/time/ptr_typedef.hh
  ${INC_DIR}/time/time_info.hh
  ${INC_DIR}/time/timeperiod.hh
  ${INC_DIR}/time/timerange.hh
  ${INC_DIR}/time/timezone_locker.hh
  ${INC_DIR}/time/timezone_manager.hh
  ${INC_DIR}/timestamp.hh
  ${INC_DIR}/vars.hh
  ${INC_DIR}/version.hh
  )

# Static libraries.
add_library(rokerbase STATIC ${LIBROKER_SOURCES})
set_target_properties(rokerbase PROPERTIES COMPILE_FLAGS "${MYSQL_CFLAGS} -fPIC")
target_link_libraries(rokerbase ${ZLIB_LIBRARIES} ${MYSQL_LIBS}  pthread dl berpc)

add_library(roker STATIC
  ${SRC_DIR}/config/applier/init.cc
  ${SRC_DIR}/config/applier/logger.cc)
target_link_libraries(roker rokerbase dl ${c-ares_LIBS} ${OpenSSL_LIBS} ${gRPC_LIBS} ${absl_LIBS} grpc++_reflection)

# Standalone binary.
set(DAEMON cbd)
add_executable(${DAEMON} ${SRC_DIR}/main.cc)

#Flags needed to include all symbols in binary.
target_link_libraries("${DAEMON}"
  "-Wl,--whole-archive" grpc++_reflection rokerbase roker "-Wl,--no-whole-archive" ${json11_LIBS} ${fmt_LIBS} ${spdlog_LIBS} ${OpenSSL_LIBS} ${gRPC_LIBS} ${absl_LIBS} )

# Centreon Broker Watchdog
option(WITH_CBWD "Build centreon broker watchdong." ON)
if (WITH_CBWD)
  add_subdirectory(watchdog)
endif ()

# Module list.
unset(MODULE_LIST)

# Generator module.
add_broker_module(GENERATOR OFF)
add_broker_module(STATS ON)
add_broker_module(NEB ON)
add_broker_module(NOTIFICATION OFF)
add_broker_module(CORRELATION ON)
add_broker_module(RRD ON)
add_broker_module(SQL ON)
add_broker_module(STORAGE ON)
add_broker_module(INFLUXDB ON)
add_broker_module(GRAPHITE ON)
add_broker_module(BAM ON)
add_broker_module(TCP ON)
add_broker_module(TLS ON)

# Lua module.
option(WITH_MODULE_LUA "Build lua module." ON)
# Simu module.
option(WITH_MODULE_SIMU "Build simu module." OFF)
if (WITH_MODULE_LUA OR WITH_MODULE_SIMU)
  foreach (vers "Lua" "Lua52" "Lua51")
    find_package("${vers}")
    if (DEFINED LUA_VERSION_STRING)
      string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.[0-9]+" "\\1\\2" v "${LUA_VERSION_STRING}")
      add_definitions(-DLUA${v})
      include_directories("${LUA_INCLUDE_DIR}")
      if (WITH_MODULE_LUA)
        add_subdirectory("lua")
        list(APPEND MODULE_LIST "lua")
      endif ()
      if (WITH_MODULE_SIMU)
        add_subdirectory("simu")
        list(APPEND MODULE_LIST "simu")
      endif ()
      break()
    endif ()
  endforeach ()
  if (NOT DEFINED LUA_VERSION_STRING)
    message(FATAL_ERROR "No Lua development package found.")
  endif ()
endif ()

# Format string.
string(REPLACE ";" ", " MODULE_LIST "${MODULE_LIST}")


#
# Install stuff.
#

# Create directories.
install(CODE "
  function(mkdir_chown user group path)
    if (APPLE OR (UNIX AND NOT CYGWIN))
      if (NOT EXISTS \"\$ENV{DESTDIR}\${path}\")
        file(MAKE_DIRECTORY \"\$ENV{DESTDIR}\${path}\")
        execute_process(COMMAND \"chown\" \"\${user}:\${group}\" \"\$ENV{DESTDIR}\${path}\")
      endif ()
    else()
      file(MAKE_DIRECTORY \"\$ENV{DESTDIR}\${path}\")
    endif ()
  endfunction()

  function(touch_chown user group file)
    if (APPLE OR (UNIX AND NOT CYGWIN))
      if (NOT EXISTS \"\$ENV{DESTDIR}\${file}\")
        file(WRITE \"\$ENV{DESTDIR}\${file}\" \"\")
        execute_process(COMMAND \"chown\" \"\${user}:\${group}\" \"\$ENV{DESTDIR}\${file}\")
      endif ()
    else()
      file(WRITE \"\$ENV{DESTDIR}\${file}\" \"\")
    endif ()
  endfunction()

  mkdir_chown(\"${USER}\" \"${GROUP}\" \"${CMAKE_INSTALL_PREFIX}\")
  mkdir_chown(\"${USER}\" \"${GROUP}\" \"${PREFIX_BIN}\")
  mkdir_chown(\"${USER}\" \"${GROUP}\" \"${PREFIX_LIB}\")
  mkdir_chown(\"${USER}\" \"${GROUP}\" \"${PREFIX_MODULES}\")
  mkdir_chown(\"${USER}\" \"${GROUP}\" \"${PREFIX_INC}\")
  mkdir_chown(\"${USER}\" \"${GROUP}\" \"${PREFIX_CONF}\")
  mkdir_chown(\"${USER}\" \"${GROUP}\" \"${PREFIX_VAR}\")
  if (WITH_STARTUP_SCRIPT STREQUAL \"sysv\")
    mkdir_chown(\"${USER}\" \"${GROUP}\" \"${STARTUP_DIR}\")
  endif ()
")

# Install rule.
install(TARGETS "${DAEMON}"
  RUNTIME DESTINATION "${PREFIX_BIN}"
  )


# Install header files for development.
install(DIRECTORY "${INC_DIR}/../../../"
  DESTINATION "${PREFIX_INC}/"
  COMPONENT "development"
  FILES_MATCHING PATTERN "*.hh" PATTERN "*.h"
  )

#
# Packaging.
#

include(cmake/package.cmake)

#
# Unit tests.
#
# Enable testing.
option(WITH_TESTING "Generate unit tests." OFF)
if (WITH_TESTING)
  add_subdirectory(test)
endif ()

option(WITH_SQL_TESTS "Sql unit tests are enabled." OFF)

#
# Print summary.
#

message(STATUS "")
message(STATUS "")
message(STATUS "Configuration Summary")
message(STATUS "---------------------")
message(STATUS "")
message(STATUS "  Project")
message(STATUS "    - Name                       Centreon Broker")
message(STATUS "    - Version                    ${CENTREON_BROKER_VERSION}")
message(STATUS "")
message(STATUS "  System")
message(STATUS "    - Name                       ${CMAKE_SYSTEM_NAME}")
message(STATUS "    - Version                    ${CMAKE_SYSTEM_VERSION}")
message(STATUS "    - Processor                  ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "")
message(STATUS "  Build")
message(STATUS "    - Compiler                   ${CMAKE_CXX_COMPILER} (${CMAKE_CXX_COMPILER_ID})")
message(STATUS "    - Extra compilation flags    ${CMAKE_CXX_FLAGS}")
if (WITH_TESTING)
  message(STATUS "    - Unit tests                 enabled")
  if (MONITORING_ENGINE)
    message(STATUS "      - Monitoring engine        ${MONITORING_ENGINE}")
  else ()
    message(STATUS "      - Monitoring engine        none")
  endif ()
  if (WITH_COVERAGE)
    message(STATUS "      - Code coverage            enabled")
  else ()
    message(STATUS "      - Code coverage            disabled")
  endif ()
else ()
  message(STATUS "    - Unit tests                 disabled")
endif ()
if (WITH_STARTUP_SCRIPT STREQUAL "no")
  message(STATUS "    - Startup script             disabled")
else ()
  message(STATUS "    - Startup script             ${WITH_STARTUP_SCRIPT}")
endif ()
message(STATUS "    - Module                     ${MODULE_LIST}")
message(STATUS "")
message(STATUS "  Install")
message(STATUS "    - Prefix                     ${CMAKE_INSTALL_PREFIX}")
message(STATUS "    - Binary prefix              ${PREFIX_BIN}")
message(STATUS "    - Library prefix             ${PREFIX_LIB}")
message(STATUS "    - Modules prefix             ${PREFIX_MODULES}")
message(STATUS "    - Include prefix             ${PREFIX_INC}")
message(STATUS "    - Configuration prefix       ${PREFIX_CONF}")
if (NOT WITH_STARTUP_SCRIPT STREQUAL "no")
  message(STATUS "    - Startup dir                ${STARTUP_DIR}")
endif ()
message(STATUS "    - User                       ${USER}")
message(STATUS "    - Group                      ${GROUP}")
message(STATUS "    - Package                    ${PACKAGE_LIST}")
message(STATUS "")
