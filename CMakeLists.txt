##
## Copyright 2009-2019 Centreon
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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(INC_DIR "${PROJECT_SOURCE_DIR}/core/inc")
include_directories("${INC_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/core/inc/com/centreon/broker")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/core/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/core/test")

# Version.
set(CENTREON_BROKER_MAJOR 20)
set(CENTREON_BROKER_MINOR 04)
set(CENTREON_BROKER_PATCH 0)
if (CENTREON_BROKER_PRERELEASE)
  set(CENTREON_BROKER_VERSION "${CENTREON_BROKER_MAJOR}.${CENTREON_BROKER_MINOR}.${CENTREON_BROKER_PATCH}-${CENTREON_BROKER_PRERELEASE}")
else()
  set(CENTREON_BROKER_VERSION "${CENTREON_BROKER_MAJOR}.${CENTREON_BROKER_MINOR}.${CENTREON_BROKER_PATCH}")
endif ()
add_definitions(-DCENTREON_BROKER_VERSION=\"${CENTREON_BROKER_VERSION}\")
configure_file(
  "${INC_DIR}/version.hh.in"
  "${INC_DIR}/version.hh"
  "@ONLY"
)

include(ExternalProject)

option(BUILD_OFFLINE "Build offline" OFF)

if (${BUILD_OFFLINE})
ExternalProject_Add( json11_lib
                     SOURCE_DIR ${PROJECT_SOURCE_DIR}/build/json11_lib-prefix/src/json11_lib
                     TIMEOUT 10
                     INSTALL_COMMAND ""
                     LOG_DOWNLOAD ON
                     LOG_CONFIGURE ON
                     LOG_BUILD ON)
else()
ExternalProject_Add( json11_lib
                     GIT_REPOSITORY https://github.com/dropbox/json11/
                     TIMEOUT 10
                     INSTALL_COMMAND ""
                     LOG_DOWNLOAD ON
                     LOG_CONFIGURE ON
                     LOG_BUILD ON)
endif()

ExternalProject_Get_Property(json11_lib source_dir)
ExternalProject_Get_Property(json11_lib binary_dir)

link_directories(${binary_dir})
include_directories(${source_dir})

#
# Check and/or find required components.
#

# Find Qt.
list(APPEND CMAKE_MODULE_PATH  "${CMAKE_CURRENT_LIST_DIR}")
find_package(Qt4 4.7.4 COMPONENTS QtCore QtSql REQUIRED)
find_package(Asio REQUIRED)
add_definitions("-DASIO_STANDALONE")

# Find zlib.
find_package(ZLIB REQUIRED)

include(${QT_USE_FILE})
set(QT_VERSION "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
if (("${QT_VERSION}" VERSION_EQUAL "4.7.0")
    OR ("${QT_VERSION}" VERSION_EQUAL "4.7.1"))
  message(WARNING "Qt versions 4.7.0 and 4.7.1 contain a bug that might prevent Centreon Broker from working properly.")
endif ()

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
elseif ()
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
endif()

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
      "${DAEMONS_CONFIGURATION}  <cbd>\n  <name>${DAEMON_NAME}</name>\n    <configuration_file>${PREFIX_CONF}/${DAEMON_NAME}.xml</configuration_file>\n    <run>1</run>\n    <reload>1</reload>\n  </cbd>\n")
  endforeach ()

  configure_file("${PROJECT_SOURCE_DIR}/script/watchdog.xml.in"
    "${PROJECT_SOURCE_DIR}/script/watchdog.xml"
    @ONLY)
  install(FILES "${PROJECT_SOURCE_DIR}/script/watchdog.xml"
    DESTINATION "${PREFIX_CONF}")
  if (WITH_STARTUP_SCRIPT STREQUAL "sysv")
    install(PROGRAMS "${PROJECT_SOURCE_DIR}/script/cbd.init"
      DESTINATION "${STARTUP_DIR}"
      RENAME "cbd")
  endif ()
endif ()

if (WITH_CONFIG_FILES)
  configure_file("${PROJECT_SOURCE_DIR}/config/poller-module.xml.in"
    "${PROJECT_SOURCE_DIR}/config/poller-module.xml"
    @ONLY)
  configure_file("${PROJECT_SOURCE_DIR}/config/central-broker.xml.in"
    "${PROJECT_SOURCE_DIR}/config/central-broker.xml"
    @ONLY)
  configure_file("${PROJECT_SOURCE_DIR}/config/central-rrd.xml.in"
    "${PROJECT_SOURCE_DIR}/config/central-rrd.xml"
    @ONLY)
  configure_file("${PROJECT_SOURCE_DIR}/config/cbmod.cfg.in"
    "${PROJECT_SOURCE_DIR}/config/cbmod.cfg"
    @ONLY)

  install(FILES "${PROJECT_SOURCE_DIR}/config/poller-module.xml"
    DESTINATION "${PREFIX_CONF}")
  install(FILES "${PROJECT_SOURCE_DIR}/config/central-rrd.xml"
    DESTINATION "${PREFIX_CONF}")
  install(FILES "${PROJECT_SOURCE_DIR}/config/central-broker.xml"
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

# DB parameters.
if (WITH_DB_TYPE)
  set(DB_TYPE "${WITH_DB_TYPE}")
  if (WITH_DB_HOST)
    set(DB_HOST "${WITH_DB_HOST}")
  else ()
    set(DB_HOST "localhost")
  endif ()
  if (WITH_DB_PORT)
    set(DB_PORT "${WITH_DB_PORT}")
  else ()
    set(DB_PORT "3306")
  endif ()
  if (WITH_DB_USER)
    set(DB_USER "${WITH_DB_USER}")
  else ()
    message(FATAL_ERROR "WITH_DB_USER is not specified.")
  endif ()
  if (WITH_DB_PASSWORD)
    set(DB_PASSWORD "${WITH_DB_PASSWORD}")
  else ()
    set(DB_PASSWORD "")
  endif ()
endif ()

# Broker vars
configure_file(
  "${INC_DIR}/vars.hh.in"
  "${INC_DIR}/vars.hh"
  "@ONLY"
)


#
# Targets.
#

# Some files must be processed by Qt's moc.
qt4_wrap_cpp(QT_WRAPPED_SOURCES
  "${INC_DIR}/logging/backend.hh"
  "${INC_DIR}/logging/manager.hh"
  "${INC_DIR}/processing/thread.hh"
)

# Core library.
set(LIBROKER_SOURCES
  # Sources.
  "${SRC_DIR}/bbdo/acceptor.cc"
  "${SRC_DIR}/bbdo/ack.cc"
  "${SRC_DIR}/bbdo/connector.cc"
  "${SRC_DIR}/bbdo/factory.cc"
  "${SRC_DIR}/bbdo/input.cc"
  "${SRC_DIR}/bbdo/input_buffer.cc"
  "${SRC_DIR}/bbdo/internal.cc"
  "${SRC_DIR}/bbdo/output.cc"
  "${SRC_DIR}/bbdo/stream.cc"
  "${SRC_DIR}/bbdo/version_response.cc"
  "${SRC_DIR}/ceof/ceof_writer.cc"
  "${SRC_DIR}/ceof/ceof_parser.cc"
  "${SRC_DIR}/ceof/ceof_iterator.cc"
  "${SRC_DIR}/ceof/ceof_token.cc"
  "${SRC_DIR}/ceof/ceof_serializer.cc"
  "${SRC_DIR}/ceof/ceof_deserializer.cc"
  "${SRC_DIR}/compression/factory.cc"
  "${SRC_DIR}/compression/internal.cc"
  "${SRC_DIR}/compression/opener.cc"
  "${SRC_DIR}/compression/stack_array.cc"
  "${SRC_DIR}/compression/stream.cc"
  "${SRC_DIR}/compression/zlib.cc"
  "${SRC_DIR}/config/applier/endpoint.cc"
  "${SRC_DIR}/config/applier/modules.cc"
  "${SRC_DIR}/config/applier/state.cc"
  "${SRC_DIR}/config/endpoint.cc"
  "${SRC_DIR}/config/logger.cc"
  "${SRC_DIR}/config/parser.cc"
  "${SRC_DIR}/config/state.cc"
  "${SRC_DIR}/database.cc"
  "${SRC_DIR}/database_config.cc"
  "${SRC_DIR}/database_preparator.cc"
  "${SRC_DIR}/database_query.cc"
  "${SRC_DIR}/exceptions/config.cc"
  "${SRC_DIR}/exceptions/corruption.cc"
  "${SRC_DIR}/exceptions/interrupt.cc"
  "${SRC_DIR}/exceptions/msg.cc"
  "${SRC_DIR}/exceptions/shutdown.cc"
  "${SRC_DIR}/exceptions/timeout.cc"
  "${SRC_DIR}/extcmd/command_client.cc"
  "${SRC_DIR}/extcmd/command_parser.cc"
  "${SRC_DIR}/extcmd/command_listener.cc"
  "${SRC_DIR}/extcmd/command_request.cc"
  "${SRC_DIR}/extcmd/command_result.cc"
  "${SRC_DIR}/extcmd/command_server.cc"
  "${SRC_DIR}/extcmd/factory.cc"
  "${SRC_DIR}/extcmd/internal.cc"
  "${SRC_DIR}/extcmd/json_command_parser.cc"
  "${SRC_DIR}/extcmd/plaintext_command_parser.cc"
  "${SRC_DIR}/file/cfile.cc"
  "${SRC_DIR}/file/directory_event.cc"
  "${SRC_DIR}/file/directory_watcher.cc"
  "${SRC_DIR}/file/factory.cc"
  "${SRC_DIR}/file/fifo.cc"
  "${SRC_DIR}/file/fs_browser.cc"
  "${SRC_DIR}/file/fs_file.cc"
  "${SRC_DIR}/file/internal.cc"
  "${SRC_DIR}/file/opener.cc"
  "${SRC_DIR}/file/qt_fs_browser.cc"
  "${SRC_DIR}/file/splitter.cc"
  "${SRC_DIR}/file/stream.cc"
  "${SRC_DIR}/instance_broadcast.cc"
  "${SRC_DIR}/io/data.cc"
  "${SRC_DIR}/io/endpoint.cc"
  "${SRC_DIR}/io/event_info.cc"
  "${SRC_DIR}/io/events.cc"
  "${SRC_DIR}/io/factory.cc"
  "${SRC_DIR}/io/properties.cc"
  "${SRC_DIR}/io/property.cc"
  "${SRC_DIR}/io/protocols.cc"
  "${SRC_DIR}/io/raw.cc"
  "${SRC_DIR}/io/stream.cc"
  "${SRC_DIR}/logging/backend.cc"
  "${SRC_DIR}/logging/file.cc"
  "${SRC_DIR}/logging/logger.cc"
  "${SRC_DIR}/logging/logging.cc"
  "${SRC_DIR}/logging/manager.cc"
  "${SRC_DIR}/logging/syslogger.cc"
  "${SRC_DIR}/logging/temp_logger.cc"
  "${SRC_DIR}/mapping/entry.cc"
  "${SRC_DIR}/mapping/source.cc"
  "${SRC_DIR}/misc/diagnostic.cc"
  "${SRC_DIR}/misc/global_lock.cc"
  "${SRC_DIR}/misc/json_writer.cc"
  "${SRC_DIR}/misc/misc.cc"
  "${SRC_DIR}/misc/processing_speed_computer.cc"
  "${SRC_DIR}/misc/string.cc"
  "${SRC_DIR}/misc/stringifier.cc"
  "${SRC_DIR}/misc/tokenizer.cc"
  "${SRC_DIR}/misc/variant.cc"
  "${SRC_DIR}/modules/handle.cc"
  "${SRC_DIR}/modules/loader.cc"
  "${SRC_DIR}/multiplexing/engine.cc"
  "${SRC_DIR}/multiplexing/hooker.cc"
  "${SRC_DIR}/multiplexing/muxer.cc"
  "${SRC_DIR}/multiplexing/publisher.cc"
  "${SRC_DIR}/multiplexing/subscriber.cc"
  "${SRC_DIR}/persistent_cache.cc"
  "${SRC_DIR}/persistent_file.cc"
  "${SRC_DIR}/processing/acceptor.cc"
  "${SRC_DIR}/processing/failover.cc"
  "${SRC_DIR}/processing/feeder.cc"
  "${SRC_DIR}/processing/thread.cc"
  "${SRC_DIR}/processing/stat_visitable.cc"
  "${SRC_DIR}/time/timezone_manager.cc"
  "${SRC_DIR}/time/timezone_locker.cc"
  "${SRC_DIR}/time/timerange.cc"
  "${SRC_DIR}/time/timeperiod.cc"
  "${SRC_DIR}/time/daterange.cc"
  ${QT_WRAPPED_SOURCES}
  # Headers.
  "${INC_DIR}/bbdo/acceptor.hh"
  "${INC_DIR}/bbdo/ack.hh"
  "${INC_DIR}/bbdo/connector.hh"
  "${INC_DIR}/bbdo/factory.hh"
  "${INC_DIR}/bbdo/input.hh"
  "${INC_DIR}/bbdo/input_buffer.hh"
  "${INC_DIR}/bbdo/internal.hh"
  "${INC_DIR}/bbdo/output.hh"
  "${INC_DIR}/bbdo/stream.hh"
  "${INC_DIR}/bbdo/version_response.hh"
  "${INC_DIR}/ceof/ceof_writer.hh"
  "${INC_DIR}/ceof/ceof_deserializer.hh"
  "${INC_DIR}/ceof/ceof_parser.hh"
  "${INC_DIR}/ceof/ceof_serializable.hh"
  "${INC_DIR}/ceof/ceof_serializer.hh"
  "${INC_DIR}/ceof/ceof_iterator.hh"
  "${INC_DIR}/ceof/ceof_token.hh"
  "${INC_DIR}/ceof/ceof_visitor.hh"
  "${INC_DIR}/compression/factory.hh"
  "${INC_DIR}/compression/internal.hh"
  "${INC_DIR}/compression/opener.hh"
  "${INC_DIR}/compression/stack_array.hh"
  "${INC_DIR}/compression/stream.hh"
  "${INC_DIR}/config/applier/endpoint.hh"
  "${INC_DIR}/config/applier/init.hh"
  "${INC_DIR}/config/applier/logger.hh"
  "${INC_DIR}/config/applier/modules.hh"
  "${INC_DIR}/config/applier/state.hh"
  "${INC_DIR}/config/endpoint.hh"
  "${INC_DIR}/config/logger.hh"
  "${INC_DIR}/config/parser.hh"
  "${INC_DIR}/config/state.hh"
  "${INC_DIR}/database.hh"
  "${INC_DIR}/database_config.hh"
  "${INC_DIR}/database_preparator.hh"
  "${INC_DIR}/database_query.hh"
  "${INC_DIR}/exceptions/config.hh"
  "${INC_DIR}/exceptions/interrupt.hh"
  "${INC_DIR}/exceptions/msg.hh"
  "${INC_DIR}/exceptions/shutdown.hh"
  "${INC_DIR}/exceptions/timeout.hh"
  "${INC_DIR}/extcmd/command_client.hh"
  "${INC_DIR}/extcmd/command_listener.hh"
  "${INC_DIR}/extcmd/command_parser.hh"
  "${INC_DIR}/extcmd/command_request.hh"
  "${INC_DIR}/extcmd/command_result.hh"
  "${INC_DIR}/extcmd/command_server.hh"
  "${INC_DIR}/extcmd/factory.hh"
  "${INC_DIR}/extcmd/internal.hh"
  "${INC_DIR}/extcmd/json_command_parser.hh"
  "${INC_DIR}/extcmd/plaintext_command_parser.hh"
  "${INC_DIR}/file/cfile.hh"
  "${INC_DIR}/file/directory_event.hh"
  "${INC_DIR}/file/directory_watcher.hh"
  "${INC_DIR}/file/factory.hh"
  "${INC_DIR}/file/fifo.hh"
  "${INC_DIR}/file/fs_browser.hh"
  "${INC_DIR}/file/fs_file.hh"
  "${INC_DIR}/file/internal.hh"
  "${INC_DIR}/file/opener.hh"
  "${INC_DIR}/file/qt_fs_browser.hh"
  "${INC_DIR}/file/splitter.hh"
  "${INC_DIR}/file/stream.hh"
  "${INC_DIR}/instance_broadcast.hh"
  "${INC_DIR}/io/data.hh"
  "${INC_DIR}/io/endpoint.hh"
  "${INC_DIR}/io/event_info.hh"
  "${INC_DIR}/io/events.hh"
  "${INC_DIR}/io/factory.hh"
  "${INC_DIR}/io/properties.hh"
  "${INC_DIR}/io/property.hh"
  "${INC_DIR}/io/protocols.hh"
  "${INC_DIR}/io/raw.hh"
  "${INC_DIR}/io/stream.hh"
  "${INC_DIR}/logging/backend.hh"
  "${INC_DIR}/logging/defines.hh"
  "${INC_DIR}/logging/file.hh"
  "${INC_DIR}/logging/logger.hh"
  "${INC_DIR}/logging/logging.hh"
  "${INC_DIR}/logging/manager.hh"
  "${INC_DIR}/logging/syslogger.hh"
  "${INC_DIR}/logging/temp_logger.hh"
  "${INC_DIR}/mapping/entry.hh"
  "${INC_DIR}/mapping/property.hh"
  "${INC_DIR}/mapping/source.hh"
  "${INC_DIR}/misc/diagnostic.hh"
  "${INC_DIR}/misc/global_lock.hh"
  "${INC_DIR}/misc/json_writer.hh"
  "${INC_DIR}/misc/misc.hh"
  "${INC_DIR}/misc/pair.hh"
  "${INC_DIR}/misc/processing_speed_computer.hh"
  "${INC_DIR}/misc/string.hh"
  "${INC_DIR}/misc/stringifier.hh"
  "${INC_DIR}/misc/tokenizer.hh"
  "${INC_DIR}/misc/variant.hh"
  "${INC_DIR}/modules/handle.hh"
  "${INC_DIR}/modules/loader.hh"
  "${INC_DIR}/multiplexing/engine.hh"
  "${INC_DIR}/multiplexing/hooker.hh"
  "${INC_DIR}/multiplexing/muxer.hh"
  "${INC_DIR}/multiplexing/publisher.hh"
  "${INC_DIR}/multiplexing/subscriber.hh"
  "${INC_DIR}/persistent_cache.hh"
  "${INC_DIR}/persistent_file.hh"
  "${INC_DIR}/processing/acceptor.hh"
  "${INC_DIR}/processing/failover.hh"
  "${INC_DIR}/processing/feeder.hh"
  "${INC_DIR}/processing/thread.hh"
  "${INC_DIR}/processing/stat_visitable.hh"
  "${INC_DIR}/timestamp.hh"
  "${INC_DIR}/time/timezone_manager.hh"
  "${INC_DIR}/time/timezone_locker.hh"
  "${INC_DIR}/time/timerange.hh"
  "${INC_DIR}/time/timeperiod.hh"
  "${INC_DIR}/time/time_info.hh"
  "${INC_DIR}/time/ptr_typedef.hh"
  "${INC_DIR}/time/daterange.hh"
  "${INC_DIR}/version.hh"
  "${INC_DIR}/vars.hh"
)

# Static libraries.
add_library("rokerbase" STATIC ${LIBROKER_SOURCES})
add_dependencies(rokerbase json11_lib)
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fPIC" COMPILER_HAS_FPIC)
if (COMPILER_HAS_FPIC)
  get_property(ROKER_CXXFLAGS
    TARGET "rokerbase"
    PROPERTY COMPILE_FLAGS)
  if (ROKER_CXXFLAGS)
    set(ROKER_CXXFLAGS "${ROKER_CXXFLAGS} -fPIC")
  else ()
    set(ROKER_CXXFLAGS "-fPIC")
  endif ()
  set_property(TARGET "rokerbase"
    PROPERTY COMPILE_FLAGS "${ROKER_CXXFLAGS}")
endif ()
target_link_libraries("rokerbase" json11 ${ZLIB_LIBRARIES} ${QT_LIBRARIES} pthread)
add_library("roker" STATIC
  "${SRC_DIR}/config/applier/init.cc"
  "${SRC_DIR}/config/applier/logger.cc")
target_link_libraries("roker" "rokerbase")

# Standalone binary.
set(DAEMON "cbd")
add_executable("${DAEMON}"
  # Sources.
  "${SRC_DIR}/main.cc"
)
if (CMAKE_COMPILER_IS_GNUCXX)
  # Flags needed to include all symbols in binary.
  target_link_libraries("${DAEMON}"
    "-Wl,--whole-archive" "rokerbase" "roker" "-Wl,--no-whole-archive")
else ()
  target_link_libraries("${DAEMON}" "roker")
endif ()

# Centreon Broker Watchdog
option(WITH_CBWD "Build centreon broker watchdong." ON)
if (WITH_CBWD)
  ADD_SUBDIrectory(watchdog)
endif()

# Module list.
unset(MODULE_LIST)

# Generator module.
option(WITH_MODULE_GENERATOR "Build generator module." OFF)
if (WITH_MODULE_GENERATOR)
  add_subdirectory(generator)
  list(APPEND MODULE_LIST "generator")
endif ()

# Stats module.
option(WITH_MODULE_STATS "Build stats module." ON)
if (WITH_MODULE_STATS)
  add_subdirectory(stats)
  list(APPEND MODULE_LIST "stats")
endif()

# NEB module.
option(WITH_MODULE_NEB "Build NEB module." ON)
if (WITH_MODULE_NEB)
  add_subdirectory(neb)
  list(APPEND MODULE_LIST "neb")
endif()

# Notification module.
option(WITH_MODULE_NOTIFICATION "Build NOTIFICATION module." ON)
if (WITH_MODULE_NOTIFICATION)
  add_subdirectory(notification)
  list(APPEND MODULE_LIST "notification")
endif()

# Correlation module.
option(WITH_MODULE_CORRELATION "Build correlation module." ON)
if (WITH_MODULE_CORRELATION)
  add_subdirectory(correlation)
  list(APPEND MODULE_LIST "correlation")
endif()

# Dumper module.
option(WITH_MODULE_DUMPER "Build Dumper module." ON)
if (WITH_MODULE_DUMPER)
  add_subdirectory(dumper)
  list(APPEND MODULE_LIST "dumper")
endif()

# RRD module.
option(WITH_MODULE_RRD "Build RRD module." ON)
if (WITH_MODULE_RRD)
  add_subdirectory(rrd)
  list(APPEND MODULE_LIST "rrd")
endif()

# SQL module.
option(WITH_MODULE_SQL "Build SQL module." ON)
if (WITH_MODULE_SQL)
  add_subdirectory(sql)
  list(APPEND MODULE_LIST "sql")
endif()

# Storage module.
option(WITH_MODULE_STORAGE "Build storage module." ON)
if (WITH_MODULE_STORAGE)
  add_subdirectory(storage)
  list(APPEND MODULE_LIST "storage")
endif()

# Lua module.
option(WITH_MODULE_LUA "Build lua module." ON)
# Simu module.
option(WITH_MODULE_SIMU "Build simu module." OFF)
if (WITH_MODULE_LUA OR WITH_MODULE_SIMU)
  foreach (vers "Lua" "Lua52" "Lua51")
    find_package("${vers}")
    if (DEFINED LUA_VERSION_STRING)
      string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.[0-9]+" "\\1\\2" v "${LUA_VERSION_STRING}")
      add_definitions(-DLUA${v} -DCENTREON_BROKER_LUA_SCRIPT_PATH="${PROJECT_SOURCE_DIR}/lua/test")
      include_directories("${LUA_INCLUDE_DIR}")
      if (WITH_MODULE_LUA)
        add_subdirectory("lua")
        list(APPEND MODULE_LIST "lua")
      endif ()
      if (WITH_MODULE_SIMU)
        add_subdirectory("simu")
        list(APPEND MODULE_LIST "simu")
      endif ()
      break ()
    endif ()
  endforeach ()
  if (NOT DEFINED LUA_VERSION_STRING)
    message(FATAL_ERROR "No Lua development package found.")
  endif ()
endif ()

# Influxdb module.
option(WITH_MODULE_INFLUXDB "Build influxdb module." ON)
if (WITH_MODULE_INFLUXDB)
  add_subdirectory(influxdb)
  list(APPEND MODULE_LIST "influxdb")
endif()

# Graphite module.
option(WITH_MODULE_GRAPHITE "Build graphite module." ON)
if (WITH_MODULE_GRAPHITE)
  add_subdirectory(graphite)
  list(APPEND MODULE_LIST "graphite")
endif()

# BAM module.
option(WITH_MODULE_BAM "Build BAM module." ON)
if (WITH_MODULE_BAM)
  add_subdirectory(bam)
  list(APPEND MODULE_LIST "bam")
endif()

# TCP module.
option(WITH_MODULE_TCP "Build TCP module." ON)
if (WITH_MODULE_TCP)
  add_subdirectory(tcp)
  list(APPEND MODULE_LIST "tcp")
endif()

# TLS module.
option(WITH_MODULE_TLS "Build TLS module." ON)
if (WITH_MODULE_TLS)
  add_subdirectory(tls)
  list(APPEND MODULE_LIST "tls")
endif()

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
  # Testing.
  set(
    TESTS_SOURCES
    ${TESTS_SOURCES}
    "${TEST_DIR}/test_server.cc"
    "${TEST_DIR}/config/logger.cc"
    "${TEST_DIR}/config/parser.cc"
  )

  enable_testing()
  add_subdirectory(test)

endif ()

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
message(STATUS "    - Qt                         ${QT_VERSION}")
if (WITH_TESTING)
  message(STATUS "    - Unit tests                 enabled")
  if (MONITORING_ENGINE)
    message(STATUS "      - Monitoring engine        ${MONITORING_ENGINE}")
  else ()
    message(STATUS "      - Monitoring engine        none")
  endif ()
  if (DB_TYPE)
    message(STATUS "      - DB type                  ${DB_TYPE}")
  else ()
    message(STATUS "      - DB type                  none")
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
