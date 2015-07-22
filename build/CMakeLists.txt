##
## Copyright 2009-2015 Merethis
##
## This file is part of Centreon Broker.
##
## Centreon Broker is free software: you can redistribute it and/or
## modify it under the terms of the GNU General Public License version 2
## as published by the Free Software Foundation.
##
## Centreon Broker is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Centreon Broker. If not, see
## <http://www.gnu.org/licenses/>.
##

#
# Global settings.
#

# Set necessary settings.
cmake_minimum_required(VERSION 2.8)
project("Centreon Broker" C CXX)
set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/..")
set(INC_DIR "${PROJECT_SOURCE_DIR}/core/inc")
include_directories("${INC_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/core/inc/com/centreon/broker")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/core/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/core/test")

# Version.
set(CENTREON_BROKER_MAJOR 3)
set(CENTREON_BROKER_MINOR 0)
set(CENTREON_BROKER_PATCH 0)
set(CENTREON_BROKER_VERSION "${CENTREON_BROKER_MAJOR}.${CENTREON_BROKER_MINOR}.${CENTREON_BROKER_PATCH}")
add_definitions(-DCENTREON_BROKER_VERSION=\"${CENTREON_BROKER_VERSION}\")

#
# Check and/or find required components.
#

# Find Qt.
find_package(Qt4 4.7.4 COMPONENTS QtCore QtNetwork QtSql QtXml REQUIRED)
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

# Unit tests.
option(WITH_TESTING "Build unit tests." OFF)
if (WITH_TESTING)
  include(CTest)
  enable_testing()
  include_directories("${PROJECT_SOURCE_DIR}/core")
  include_directories("${PROJECT_SOURCE_DIR}/storage/inc")
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
else ()
  set(WITH_STARTUP_SCRIPT "no")
endif ()

# Startup dir.
if (WITH_STARTUP_SCRIPT STREQUAL "sysv")
  # Set destination directory.
  if (WITH_STARTUP_DIR)
    set(STARTUP_DIR "${WITH_STARTUP_DIR}")
  else ()
    set(STARTUP_DIR "/etc/init.d")
  endif ()
endif ()

# Configure files.
if (WITH_DAEMONS)
  if (OS_DISTRIBUTOR STREQUAL "CentOS" OR OS_DISTRIBUTOR STREQUAL "RedHat")
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

  set(DAEMONS_CONFIGURATION "")
  foreach (DAEMON_NAME IN LISTS WITH_DAEMONS)
    set(DAEMONS_CONFIGURATION
      "${DAEMONS_CONFIGURATION}${DAEMON_NAME}\t${DAEMON_NAME}.xml\ty\ty\n")
  endforeach ()

  configure_file("${PROJECT_SOURCE_DIR}/script/master.run.in"
    "${PROJECT_SOURCE_DIR}/script/master.run"
    @ONLY)
  install(FILES "${PROJECT_SOURCE_DIR}/script/master.run"
    DESTINATION "${PREFIX_CONF}")
  install(PROGRAMS "${PROJECT_SOURCE_DIR}/script/cbd.init"
    DESTINATION "${STARTUP_DIR}"
    RENAME "cbd")
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
  "${SRC_DIR}/bbdo/connector.cc"
  "${SRC_DIR}/bbdo/factory.cc"
  "${SRC_DIR}/bbdo/input.cc"
  "${SRC_DIR}/bbdo/internal.cc"
  "${SRC_DIR}/bbdo/output.cc"
  "${SRC_DIR}/bbdo/stream.cc"
  "${SRC_DIR}/bbdo/version_response.cc"
  "${SRC_DIR}/compression/factory.cc"
  "${SRC_DIR}/compression/internal.cc"
  "${SRC_DIR}/compression/opener.cc"
  "${SRC_DIR}/compression/stream.cc"
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
  "${SRC_DIR}/exceptions/msg.cc"
  "${SRC_DIR}/exceptions/timeout.cc"
  "${SRC_DIR}/extcmd/command_request.cc"
  "${SRC_DIR}/extcmd/command_result.cc"
  "${SRC_DIR}/extcmd/endpoint.cc"
  "${SRC_DIR}/extcmd/factory.cc"
  "${SRC_DIR}/extcmd/stream.cc"
  "${SRC_DIR}/extcmd/internal.cc"
  "${SRC_DIR}/file/cfile.cc"
  "${SRC_DIR}/file/directory_event.cc"
  "${SRC_DIR}/file/directory_watcher.cc"
  "${SRC_DIR}/file/factory.cc"
  "${SRC_DIR}/file/fifo.cc"
  "${SRC_DIR}/file/internal.cc"
  "${SRC_DIR}/file/opener.cc"
  "${SRC_DIR}/file/stream.cc"
  "${SRC_DIR}/instance_broadcast.cc"
  "${SRC_DIR}/io/data.cc"
  "${SRC_DIR}/io/endpoint.cc"
  "${SRC_DIR}/io/event_info.cc"
  "${SRC_DIR}/io/events.cc"
  "${SRC_DIR}/io/exceptions/shutdown.cc"
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
  "${SRC_DIR}/misc/string.cc"
  "${SRC_DIR}/misc/stringifier.cc"
  "${SRC_DIR}/misc/tokenizer.cc"
  "${SRC_DIR}/misc/global_lock.cc"
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
  "${SRC_DIR}/time/timezone_manager.cc"
  "${SRC_DIR}/time/timezone_locker.cc"
  "${SRC_DIR}/time/timerange.cc"
  "${SRC_DIR}/time/timeperiod.cc"
  "${SRC_DIR}/time/daterange.cc"
  ${QT_WRAPPED_SOURCES}
  # Headers.
  "${INC_DIR}/bbdo/acceptor.hh"
  "${INC_DIR}/bbdo/connector.hh"
  "${INC_DIR}/bbdo/factory.hh"
  "${INC_DIR}/bbdo/input.hh"
  "${INC_DIR}/bbdo/internal.hh"
  "${INC_DIR}/bbdo/output.hh"
  "${INC_DIR}/bbdo/stream.hh"
  "${INC_DIR}/bbdo/version_response.hh"
  "${INC_DIR}/compression/factory.hh"
  "${INC_DIR}/compression/internal.hh"
  "${INC_DIR}/compression/opener.hh"
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
  "${INC_DIR}/exceptions/msg.hh"
  "${INC_DIR}/exceptions/timeout.hh"
  "${INC_DIR}/extcmd/command_request.hh"
  "${INC_DIR}/extcmd/command_result.hh"
  "${INC_DIR}/extcmd/endpoint.hh"
  "${INC_DIR}/extcmd/factory.hh"
  "${INC_DIR}/extcmd/stream.hh"
  "${INC_DIR}/extcmd/internal.hh"
  "${INC_DIR}/file/cfile.hh"
  "${INC_DIR}/file/directory_event.hh"
  "${INC_DIR}/file/directory_watcher.hh"
  "${INC_DIR}/file/factory.hh"
  "${INC_DIR}/file/fifo.hh"
  "${INC_DIR}/file/internal.hh"
  "${INC_DIR}/file/opener.hh"
  "${INC_DIR}/file/stream.hh"
  "${INC_DIR}/instance_broadcast.hh"
  "${INC_DIR}/io/data.hh"
  "${INC_DIR}/io/endpoint.hh"
  "${INC_DIR}/io/event_info.hh"
  "${INC_DIR}/io/events.hh"
  "${INC_DIR}/io/exceptions/shutdown.hh"
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
  "${INC_DIR}/misc/string.hh"
  "${INC_DIR}/misc/stringifier.hh"
  "${INC_DIR}/misc/tokenizer.hh"
  "${INC_DIR}/misc/global_lock.hh"
  "${INC_DIR}/misc/shared_ptr.hh"
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
  "${INC_DIR}/timestamp.hh"
  "${INC_DIR}/time/timezone_manager.hh"
  "${INC_DIR}/time/timezone_locker.hh"
  "${INC_DIR}/time/timerange.hh"
  "${INC_DIR}/time/timeperiod.hh"
  "${INC_DIR}/time/time_info.hh"
  "${INC_DIR}/time/ptr_typedef.hh"
  "${INC_DIR}/time/daterange.hh"
)

# Static libraries.
add_library("rokerbase" STATIC ${LIBROKER_SOURCES})
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
target_link_libraries("rokerbase" ${QT_LIBRARIES})
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
get_property(CBD_PATH
  TARGET "cbd"
  PROPERTY LOCATION)

# Module list.
unset(MODULE_LIST)

# Stats module.
option(WITH_MODULE_STATS "Build stats module." ON)
if (WITH_MODULE_STATS)
  add_subdirectory("stats")
  list(APPEND MODULE_LIST "stats")
endif()

# NEB module.
option(WITH_MODULE_NEB "Build NEB module." ON)
if (WITH_MODULE_NEB)
  add_subdirectory("neb")
  list(APPEND MODULE_LIST "neb")
endif()

# Notification module.
option(WITH_MODULE_NOTIFICATION "Build NOTIFICATION module." ON)
if (WITH_MODULE_NOTIFICATION)
  add_subdirectory("notification")
  list(APPEND MODULE_LIST "notification")
endif()

# Correlation module.
option(WITH_MODULE_CORRELATION "Build correlation module." ON)
if (WITH_MODULE_CORRELATION)
  add_subdirectory("correlation")
  list(APPEND MODULE_LIST "correlation")
endif()

# Dumper module.
option(WITH_MODULE_DUMPER "Build Dumper module." ON)
if (WITH_MODULE_DUMPER)
  add_subdirectory("dumper")
  list(APPEND MODULE_LIST "dumper")
endif()

# RRD module.
option(WITH_MODULE_RRD "Build RRD module." ON)
if (WITH_MODULE_RRD)
  add_subdirectory("rrd")
  list(APPEND MODULE_LIST "rrd")
endif()

# SQL module.
option(WITH_MODULE_SQL "Build SQL module." ON)
if (WITH_MODULE_SQL)
  add_subdirectory("sql")
  list(APPEND MODULE_LIST "sql")
endif()

# Storage module.
option(WITH_MODULE_STORAGE "Build storage module." ON)
if (WITH_MODULE_STORAGE)
  add_subdirectory("storage")
  list(APPEND MODULE_LIST "storage")
endif()

# Influxdb module.
option(WITH_MODULE_INFLUXDB "Build influxdb module." ON)
if (WITH_MODULE_INFLUXDB)
  add_subdirectory("influxdb")
  list(APPEND MODULE_LIST "influxdb")
endif()

# Graphite module.
option(WITH_MODULE_GRAPHITE "Build graphite module." ON)
if (WITH_MODULE_GRAPHITE)
  add_subdirectory("graphite")
  list(APPEND MODULE_LIST "graphite")
endif()

# BAM module.
option(WITH_MODULE_BAM "Build BAM module." ON)
if (WITH_MODULE_BAM)
  add_subdirectory("bam")
  list(APPEND MODULE_LIST "bam")
endif()

# TCP module.
option(WITH_MODULE_TCP "Build TCP module." ON)
if (WITH_MODULE_TCP)
  add_subdirectory("tcp")
  list(APPEND MODULE_LIST "tcp")
endif()

# TLS module.
option(WITH_MODULE_TLS "Build TLS module." ON)
if (WITH_MODULE_TLS)
  add_subdirectory("tls")
  list(APPEND MODULE_LIST "tls")
endif()

# Format string.
string(REPLACE ";" ", " MODULE_LIST "${MODULE_LIST}")

# Functional tests.
if (WITH_TESTING AND DB_TYPE AND MONITORING_ENGINE)
  # Configure files.
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/broker_modules.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/broker_modules.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/monitoring_to_sql.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/monitoring_to_sql.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/dependencies_to_sql.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/dependencies_to_sql.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/sql_transactions.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/sql_transactions.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/custom_variables_to_sql.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/custom_variables_to_sql.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/event_handlers_to_sql_1.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/event_handlers_to_sql_1.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/event_handlers_to_sql_2.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/event_handlers_to_sql_2.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/filtering_to_sql.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/filtering_to_sql.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/compression_to_sql_1.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/compression_to_sql_1.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/compression_to_sql_2.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/compression_to_sql_2.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/tls_to_sql_1.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/tls_to_sql_1.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/tls_to_sql_2.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/tls_to_sql_2.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/bbdo_protocol_version.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/bbdo_protocol_version.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/bbdo_multiple_connections_0.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/bbdo_multiple_connections_0.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/bbdo_multiple_connections_1.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/bbdo_multiple_connections_1.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/bbdo_multiple_connections_2.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/bbdo_multiple_connections_2.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/bbdo_one_peer_retention_mode_1.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/bbdo_one_peer_retention_mode_1.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/bbdo_one_peer_retention_mode_2.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/bbdo_one_peer_retention_mode_2.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/delete_graphs.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/delete_graphs.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/rrd_as_failover.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/rrd_as_failover.xml")
  configure_file("${PROJECT_SOURCE_DIR}/test/cfg/sql_cleanup.xml.in"
    "${PROJECT_SOURCE_DIR}/test/cfg/sql_cleanup.xml")

  # Test library.
  include_directories("${PROJECT_SOURCE_DIR}")
  include_directories("${PROJECT_SOURCE_DIR}/neb/inc/")
  if (LIBRRD_INCLUDE_DIRS)
    include_directories("${LIBRRD_INCLUDE_DIRS}")
  endif ()
  set(TEST_LIB "brokertest")
  add_library("${TEST_LIB}" STATIC
    # Sources.
    "${PROJECT_SOURCE_DIR}/test/cbd.cc"
    "${PROJECT_SOURCE_DIR}/test/config.cc"
    "${PROJECT_SOURCE_DIR}/test/engine.cc"
    "${PROJECT_SOURCE_DIR}/test/external_command.cc"
    "${PROJECT_SOURCE_DIR}/test/generate.cc"
    "${PROJECT_SOURCE_DIR}/test/misc.cc"
    "${PROJECT_SOURCE_DIR}/test/rrd_file.cc"
    # Headers.
    "${PROJECT_SOURCE_DIR}/test/cbd.hh"
    "${PROJECT_SOURCE_DIR}/test/config.hh"
    "${PROJECT_SOURCE_DIR}/test/engine.hh"
    "${PROJECT_SOURCE_DIR}/test/external_command.hh"
    "${PROJECT_SOURCE_DIR}/test/generate.hh"
    "${PROJECT_SOURCE_DIR}/test/misc.hh"
    "${PROJECT_SOURCE_DIR}/test/rrd_file.hh"
    )
  target_link_libraries("${TEST_LIB}" "roker")

  # My plugin.
  set(MY_PLUGIN "my_plugin")
  add_executable("${MY_PLUGIN}"
    "${PROJECT_SOURCE_DIR}/test/my_plugin.cc")
  get_property(MY_PLUGIN_PATH
    TARGET "${MY_PLUGIN}"
    PROPERTY LOCATION)
  # My BAM plugin.
  set(MY_PLUGIN_BAM "my_plugin_bam")
  add_executable("${MY_PLUGIN_BAM}"
    "${PROJECT_SOURCE_DIR}/test/my_plugin_bam.cc")
  target_link_libraries("${MY_PLUGIN_BAM}" "${TEST_LIB}")
  get_property(MY_PLUGIN_BAM_PATH
    TARGET "${MY_PLUGIN_BAM}"
    PROPERTY LOCATION)

  # Monitoring to SQL.
  set(TEST_NAME "running_monitoring_to_sql")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/monitoring_to_sql.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Dependencies to SQL.
  set(TEST_NAME "running_dependencies_to_sql")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/dependencies_to_sql.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # SQL transactions.
  set(TEST_NAME "running_sql_transactions")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/sql_transactions.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Acknowledgements to SQL.
  set(TEST_NAME "running_acknowledgements_to_sql")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/acknowledgements_to_sql.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Correlation to SQL.
  set(TEST_NAME "running_correlation_to_sql")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/correlation_to_sql.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Custom variables to SQL.
  set(TEST_NAME "running_custom_variables_to_sql")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/custom_variables_to_sql.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Downtimes to SQL.
  set(TEST_NAME "running_downtimes_to_sql")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/downtimes_to_sql.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Event handlers to SQL.
  set(TEST_NAME "running_event_handlers_to_sql")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/event_handlers_to_sql.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Filtering to SQL.
  set(TEST_NAME "running_filtering_to_sql")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/filtering_to_sql.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Instance update when outdated.
  set(TEST_NAME "running_sql_instance_update_outdated")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/sql_instance_update_outdated.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # SQL cleanup.
  set(TEST_NAME "running_sql_cleanup")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/sql_cleanup.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}" ${LIBRRD_LDFLAGS})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # SQL id change.
  set(TEST_NAME "running_sql_id_change")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/sql_id_change.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Protocols.
  set(TEST_NAME "running_compression_to_sql")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/compression_to_sql.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Encryption.
  set(TEST_NAME "running_tls_to_sql")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/tls_to_sql.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # BBDO protocol version.
  set(TEST_NAME "running_bbdo_protocol_version")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/bbdo_protocol_version.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # BBDO supports multiple connections.
  set(TEST_NAME "running_bbdo_multiple_connections")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/bbdo_multiple_connections.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # BBDO One Peer Retention Mode.
  set(TEST_NAME "running_bbdo_one_peer_retention_mode")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/bbdo_one_peer_retention_mode.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Monitoring to RRD graphs.
  set(TEST_NAME "running_monitoring_to_rrd")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/monitoring_to_rrd.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}" ${LIBRRD_LDFLAGS})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Graph rebuild
  set(TEST_NAME "running_rebuild_graphs")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/rebuild_graphs.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}" ${LIBRRD_LDFLAGS})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Disable status graphs.
  set(TEST_NAME "running_disable_status_graphs")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/disable_status_graphs.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Disable metrics graphs.
  set(TEST_NAME "running_disable_metrics_graphs")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/disable_metrics_graphs.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Graph deletion.
  set(TEST_NAME "running_delete_graphs")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/delete_graphs.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Failover to TCP.
  set(TEST_NAME "running_failover_to_tcp")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/failover_to_tcp.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Secondary failover to file.
  set(TEST_NAME "running_secondary_failovers_to_file")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/secondary_failovers_to_file.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # RRD endpoint as failover.
  set(TEST_NAME "running_rrd_as_failover")
  add_test("${TEST_NAME}" "cbd" "-c" "${PROJECT_SOURCE_DIR}/test/cfg/rrd_as_failover.xml")

  # Automatic retention.
  set(TEST_NAME "running_automatic_retention")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/automatic_retention.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Statistics.
  set(TEST_NAME "running_stats")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/stats.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Statistics events (internal metrics).
  set(TEST_NAME "running_stats_events")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/stats_events.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}" ${LIBRRD_LDFLAGS})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # BAM engine.
  set(TEST_NAME "running_bam")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/bam.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # BAM dimension.
  set(TEST_NAME "running_bam_dimension")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/bam_dimension.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # BAM rebuild.
  set(TEST_NAME "running_bam_rebuild")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/bam_rebuild.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # BAM BI events autoclose.
  set(TEST_NAME "running_bam_events_autoclose")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/bam_events_autoclose.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # BAM bool expression retention.
  set(TEST_NAME "running_bam_boolexp_retention")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/bam_boolexp_retention.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # BA and correlation.
  set(TEST_NAME "running_bam_correlation")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/bam_correlation.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Meta-services.
  set(TEST_NAME "running_meta_service")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/meta_service.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Non-correlated notification.
  set(TEST_NAME "running_notification_non_correlated")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/notification_non_correlated.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Correlated notification.
  set(TEST_NAME "running_notification_correlated")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/notification_correlated.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Notification write into file utility
  add_executable("util_write_into_file"
    "${PROJECT_SOURCE_DIR}/test/write_into_file.cc")

  # Notification.
  set(TEST_NAME "running_notification")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/notification.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Influxdb
  set(TEST_NAME "running_influxdb")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/influxdb.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIB})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Graphite
  set(TEST_NAME "running_graphite")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/graphite.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIB})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Dumper FIFO
  set(TEST_NAME "running_fifo_dumper")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/fifo_dumper.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIB})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Dumper DIR
  set(TEST_NAME "running_directory_dumper")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/directory_dumper.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIB})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # Database synchronization.
  set(TEST_NAME "running_db_cfg")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/db_cfg.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIB})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # RRD benchmark.
  unset(QT_WRAPPED_SOURCES)
  qt4_wrap_cpp(QT_WRAPPED_SOURCES
    "${PROJECT_SOURCE_DIR}/test/bench_generate_rrd_mod.hh")
  set(TEST_NAME "running_bench_generate_rrd_mod")
  add_library("${TEST_NAME}" SHARED
    "${PROJECT_SOURCE_DIR}/test/bench_generate_rrd_mod.cc"
    ${QT_WRAPPED_SOURCES})
  get_property(BENCH_GENERATE_RRD_MOD_PATH
    TARGET "${TEST_NAME}"
    PROPERTY LOCATION)
  set(TEST_NAME "running_bench_generate_rrd")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/bench_generate_rrd.cc")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}")

  # BBDO benchmark.
  set(TEST_NAME "running_bench_bbdo")
  add_executable("${TEST_NAME}"
    "${PROJECT_SOURCE_DIR}/test/bench_bbdo.cc"
    "${PROJECT_SOURCE_DIR}/test/bench_stream.cc"
    "${PROJECT_SOURCE_DIR}/test/bench_stream.hh")
  target_link_libraries("${TEST_NAME}" "${TEST_LIB}" ${BAM} ${NOTIFICATION} ${CORRELATION} ${NEB} ${STORAGE})

  # Last file to configure, based on target paths.
  configure_file("${PROJECT_SOURCE_DIR}/test/vars.hh.in"
    "${PROJECT_SOURCE_DIR}/test/vars.hh")

endif ()

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
  if (WITH_STARTUP_SCRIPT STREQUAL \"sysv\")
    mkdir_chown(\"${USER}\" \"${GROUP}\" \"${STARTUP_DIR}\")
  endif ()
")

# Install rule.
#install(TARGETS "roker"
#  ARCHIVE DESTINATION "${PREFIX_LIB}"
#)

# Install rule.
install(TARGETS "${DAEMON}"
  RUNTIME DESTINATION "${PREFIX_BIN}"
)

# Install scripts.
#if (WITH_STARTUP_SCRIPT STREQUAL "sysv")
#  foreach (DAEMON_NAME IN LISTS WITH_DAEMONS)
#    install(FILES "${PROJECT_SOURCE_DIR}/script/cbd-${DAEMON_NAME}.sh"
#      DESTINATION "${STARTUP_DIR}"
#      RENAME "cbd-${DAEMON_NAME}")
#  endforeach()
#endif ()

#
# Packaging.
#

include("package.cmake")

#
# Unit tests.
#

# Enable testing.
option(WITH_TESTING "Generate unit tests." OFF)
if (WITH_TESTING)
  # misc namespace tests.
  # misc::stringifier tests.
  #   Default construction.
  set(TEST_NAME "core_misc_stringifier_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   C-string insertion.
  set(TEST_NAME "core_misc_stringifier_string")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/string.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Boolean insertion.
  set(TEST_NAME "core_misc_stringifier_bool")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/bool.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Double insertion.
  set(TEST_NAME "core_misc_stringifier_double")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/double.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Integer insertion.
  set(TEST_NAME "core_misc_stringifier_int")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/int.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Long insertion.
  set(TEST_NAME "core_misc_stringifier_long")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/long.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Long long insertion.
  set(TEST_NAME "core_misc_stringifier_long_long")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/long_long.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   QString insertion.
  set(TEST_NAME "core_misc_stringifier_qstring")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/qstring.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Unsigned integer insertion.
  set(TEST_NAME "core_misc_stringifier_uint")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/uint.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Unsigned long integer insertion.
  set(TEST_NAME "core_misc_stringifier_ulong")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/ulong.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Unsigned long long integer insertion.
  set(TEST_NAME "core_misc_stringifier_ulong_long")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/ulong_long.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Pointer insertion.
  set(TEST_NAME "core_misc_stringifier_pointer")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/pointer.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Mixed insertions.
  set(TEST_NAME "core_misc_stringifier_mixed")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/mixed.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Reinitialization of object.
  set(TEST_NAME "core_misc_stringifier_reset")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/reset.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "core_misc_stringifier_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "core_misc_stringifier_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/misc/stringifier/assignment.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")


  # exceptions namespace tests.
  # exceptions::msg tests.
  #   Default construction.
  set(TEST_NAME "core_exceptions_msg_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/exceptions/msg/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "core_exceptions_msg_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/exceptions/msg/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "core_exceptions_msg_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/exceptions/msg/assignment.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Throw test.
  set(TEST_NAME "core_exceptions_msg_throw")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/exceptions/msg/throw.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Rethrow test.
  set(TEST_NAME "core_exceptions_msg_rethrow")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/exceptions/msg/rethrow.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Clone test.
  set(TEST_NAME "core_exceptions_msg_clone")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/exceptions/msg/clone.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")


  # config namespace tests.
  # config::logger tests.
  #   Default constructor.
  set(TEST_NAME "core_config_logger_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/config/logger/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy constructor.
  set(TEST_NAME "core_config_logger_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/config/logger/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "core_config_logger_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/config/logger/assignment.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Equality operator.
  set(TEST_NAME "core_config_logger_equality")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/config/logger/equality.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Inequality operator.
  set(TEST_NAME "core_config_logger_inequality")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/config/logger/inequality.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Parser tests.
  #   'logger' keyword.
  set(TEST_NAME "core_config_parser_logger")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/config/parser/logger.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   'input' and 'output' keywords.
  set(TEST_NAME "core_config_parser_endpoint")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/config/parser/endpoint.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   'include' keyword.
  set(TEST_NAME "core_config_parser_include")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/config/parser/include.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")


  # logging namespace tests.
  # logging::file tests.
  add_library("ccb_core_logging_file"
    STATIC
    "${TEST_DIR}/logging/file/common.cc")
  #   Logging with no timestamp nor thread ID.
  set(TEST_NAME "core_logging_file_log_with_nothing")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/file/log_with_nothing.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Logging with timestamp.
  set(TEST_NAME "core_logging_file_log_with_timestamp")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/file/log_with_timestamp.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Logging with thread ID.
  set(TEST_NAME "core_logging_file_log_with_thread_id")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/file/log_with_thread_id.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Logging with timestamp and thread ID.
  set(TEST_NAME "core_logging_file_log_with_everything")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/file/log_with_everything.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Logging with maximum file size.
  set(TEST_NAME "core_logging_file_log_with_max_size")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/file/log_with_max_size.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # logging::manager tests.
  #   Simple logging.
  set(TEST_NAME "core_logging_manager_simple")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/manager/simple.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Logging on multiple backends.
  set(TEST_NAME "core_logging_manager_multiple")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/manager/multiple.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Logging on backends with different options.
  set(TEST_NAME "core_logging_manager_cross")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/manager/cross.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Stop logging on a backend.
  set(TEST_NAME "core_logging_manager_backend_unlog")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/manager/backend_unlog.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # logging::temp_logger tests.
  #   Default test.
  set(TEST_NAME "core_logging_temp_logger_enabled")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/temp_logger/enabled.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Object copy.
  set(TEST_NAME "core_logging_temp_logger_copied")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/temp_logger/copied.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Data insertion.
  set(TEST_NAME "core_logging_temp_logger_insertion")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/temp_logger/insertion.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Test with temp_logger disabled.
  set(TEST_NAME "core_logging_temp_logger_disabled")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/temp_logger/disabled.cc"
    "${TEST_DIR}/logging/file/common.hh")
  target_link_libraries("${TEST_NAME}" "roker" "ccb_core_logging_file")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # Global tests.
  if ("${QT_VERSION}" VERSION_GREATER "4.4.0")
    #   Concurrent logging.
    set(TEST_NAME "core_logging_logging_concurrent")
    add_executable("${TEST_NAME}"
      "${TEST_DIR}/logging/logging/concurrent.cc")
    target_link_libraries("${TEST_NAME}" "roker")
    add_test("${TEST_NAME}" "${TEST_NAME}")
  endif ()
  #   Performance testing.
  set(TEST_NAME "core_logging_logging_performance")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/logging/performance.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")


  # io namespace tests.
  # io::raw tests.
  #   Default constructor.
  set(TEST_NAME "core_io_raw_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/io/raw/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # io::exceptions namespace tests.
  # io::exceptions::shutdown tests.
  #   Default constructor.
  set(TEST_NAME "core_io_exceptions_shutdown_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/io/exceptions/shutdown/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Copy construction.
  set(TEST_NAME "core_io_exceptions_shutdown_ctor_copy")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/io/exceptions/shutdown/ctor_copy.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Assignment operator.
  set(TEST_NAME "core_io_exceptions_shutdown_assignment")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/io/exceptions/shutdown/assignment.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Throw test.
  set(TEST_NAME "core_io_exceptions_shutdown_throw")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/io/exceptions/shutdown/throw.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Rethrow test.
  set(TEST_NAME "core_io_exceptions_shutdown_rethrow")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/io/exceptions/shutdown/rethrow.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Clone test.
  set(TEST_NAME "core_io_exceptions_shutdown_clone")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/io/exceptions/shutdown/clone.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")


  # multiplexing namespace tests.
  # multiplexing::subscriber tests.
  add_library("ccb_core_multiplexing_subscriber" STATIC
    "${TEST_DIR}/multiplexing/subscriber/temporary_endpoint.cc"
    "${TEST_DIR}/multiplexing/subscriber/temporary_stream.cc")
  set(TEST_LIBRARIES "ccb_core_multiplexing_subscriber" "roker")

  #   Default constructor.
  set(TEST_NAME "core_multiplexing_subscriber_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/multiplexing/subscriber/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # multiplexing::engine tests.
  add_library("ccb_core_multiplexing_engine" STATIC
    "${TEST_DIR}/multiplexing/engine/hooker.cc")
  #   Start and stop.
  set(TEST_NAME "core_multiplexing_engine_start_stop")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/multiplexing/engine/start_stop.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Hook.
  set(TEST_NAME "core_multiplexing_engine_hook")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/multiplexing/engine/hook.cc")
  target_link_libraries("${TEST_NAME}"
    "ccb_core_multiplexing_engine"
    "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Unhook.
  set(TEST_NAME "core_multiplexing_engine_unhook")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/multiplexing/engine/unhook.cc")
  target_link_libraries("${TEST_NAME}"
    "ccb_core_multiplexing_engine"
    "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # multiplexing::publisher tests.
  #   Read.
  set(TEST_NAME "core_multiplexing_publisher_read")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/multiplexing/publisher/read.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Write.
  set(TEST_NAME "core_multiplexing_publisher_write")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/multiplexing/publisher/write.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")


  # file namespace tests.
  # file::stream tests.
  #   Writing.
  set(TEST_NAME "file_stream_write")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/file/stream/write.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Reading.
  set(TEST_NAME "file_stream_read")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/file/stream/read.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Size limitation.
  set(TEST_NAME "file_stream_max_size")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/file/stream/max_size.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Mixed reads/writes.
  set(TEST_NAME "file_stream_mixed")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/file/stream/mixed.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")


  # compression namespace tests.
  # compression::stream tests.
  #   Compress/uncompress.
  set(TEST_NAME "compression_stream_io")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/compression/stream/io.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Buffer is read back.
  set(TEST_NAME "compression_buffer_is_read_back")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/compression/stream/buffer_is_read_back.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # module namespace tests.
  # module::loader tests.
  #   module versioning.
  set(TEST_NAME "modules_versioning")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/modules/module.cc")
  add_library("null_module" SHARED
    "${TEST_DIR}/modules/null_module.cc")
  add_library("bad_version_module" SHARED
    "${TEST_DIR}/modules/bad_version_module.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")

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
