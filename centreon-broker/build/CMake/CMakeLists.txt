##
##  Copyright 2009 MERETHIS
##  This file is part of CentreonBroker.
##
##  CentreonBroker is free software: you can redistribute it and/or modify it
##  under the terms of the GNU General Public License as published by the Free
##  Software Foundation, either version 2 of the License, or (at your option)
##  any later version.
##
##  CentreonBroker is distributed in the hope that it will be useful, but
##  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
##  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
##  for more details.
##
##  You should have received a copy of the GNU General Public License along
##  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
##
##  For more information : contact@centreon.com
##

# Global options.
cmake_minimum_required(VERSION 2.6)
project(CENTREONBROKER CXX)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
include_directories(../../inc/ ../../inc/daemon ../../inc/module/)

# Set TinyXML macro.
add_definitions(-DTIXML_USE_STL)

# Enable or not the debug mode.
option(debug "Generate debug messages." OFF)
if (NOT debug)
  add_definitions(-DNDEBUG)
  message(STATUS "Debug messages disabled.")
else ()
  message(STATUS "Debug messages enabled.")
endif ()

# Find SOCI.
set(SOCI_DIR .)
find_package(SOCI REQUIRED)
include_directories(${SOCI_INCLUDE_DIR})

# Check for MySQL.
option(MySQL "MySQL support." ON)
if (MySQL)
  set(SOCIMySQL_DIR .)
  find_package(SOCIMySQL REQUIRED)
  include_directories(${SOCIMYSQL_INCLUDE_DIR})
  add_definitions(-DUSE_MYSQL)
endif ()

# Check for Oracle
option(Oracle "Oracle support." OFF)
if (Oracle)
  set(SOCIOracle_DIR .)
#  find_package(SOCIOracle REQUIRED)
  include_directories(${SOCIORACLE_INCLUDE_DIR})
  add_definitions(-DUSE_ORACLE)
endif ()

# Check for PostgreSQL
option(PostgreSQL "PostgreSQL support." OFF)
if (PostgreSQL)
  set(SOCIPostgreSQL_DIR .)
  find_package(SOCIPostgreSQL REQUIRED)
  include_directories(${SOCIPOSTGRESQL_INCLUDE_DIR})
  add_definitions(-DUSE_POSTGRESQL)
endif ()

# Check for GNU TLS
option(TLS "TLS support." ON)
if (TLS)
  set(GNUTLS_DIR .)
  find_package(GNUTLS REQUIRED)
  include_directories(${GNUTLS_INCLUDE_DIR})
  add_definitions(-DUSE_TLS)
endif ()

# libroker library.
set(LIBROKER_SOURCES
	../../src/concurrency/condition_variable.cpp
	../../src/concurrency/lock.cpp
	../../src/concurrency/mutex.cpp
	../../src/concurrency/thread.cpp
	../../src/concurrency/thread_listener.cpp
	../../src/events/acknowledgement.cpp
	../../src/events/check.cpp
	../../src/events/comment.cpp
	../../src/events/dependency.cpp
	../../src/events/downtime.cpp
	../../src/events/event.cpp
	../../src/events/group.cpp
	../../src/events/group_member.cpp
	../../src/events/host.cpp
	../../src/events/host_check.cpp
	../../src/events/host_dependency.cpp
	../../src/events/host_group.cpp
	../../src/events/host_group_member.cpp
	../../src/events/host_parent.cpp
	../../src/events/host_service.cpp
	../../src/events/host_service_status.cpp
	../../src/events/host_status.cpp
	../../src/events/issue.cpp
	../../src/events/issue_status.cpp
	../../src/events/log.cpp
	../../src/events/program.cpp
	../../src/events/program_status.cpp
	../../src/events/service.cpp
	../../src/events/service_check.cpp
	../../src/events/service_dependency.cpp
	../../src/events/service_group.cpp
	../../src/events/service_group_member.cpp
	../../src/events/service_status.cpp
	../../src/events/status.cpp
	../../src/exception.cpp
	../../src/interface/db/destination.cpp
	../../src/interface/db/internal.cpp
	../../src/interface/destination.cpp
	../../src/interface/factory.cpp
	../../src/interface/ndo/base.cpp
	../../src/interface/ndo/destination.cpp
	../../src/interface/ndo/internal.cpp
	../../src/interface/ndo/source.cpp
	../../src/interface/ndo/sourcedestination.cpp
	../../src/interface/source.cpp
	../../src/interface/sourcedestination.cpp
	../../src/interface/xml/destination.cpp
	../../src/interface/xml/internal.cpp
	../../src/interface/xml/source.cpp
	../../src/interface/xml/tinystr.cpp
	../../src/interface/xml/tinyxml.cpp
	../../src/interface/xml/tinyxmlerror.cpp
	../../src/interface/xml/tinyxmlparser.cpp
	../../src/io/acceptor.cpp
	../../src/io/fd.cpp
	../../src/io/file.cpp
	../../src/io/net/ipv4.cpp
	../../src/io/net/ipv6.cpp
	../../src/io/net/socket.cpp
	../../src/io/net/unix.cpp
	../../src/io/split.cpp
	../../src/io/stream.cpp
	../../src/io/text.cpp
	../../src/logging.cpp
	../../src/mapping.cpp
)
if (TLS)
  set(LIBROKER_SOURCES ${LIBROKER_SOURCES}
	../../src/io/tls/acceptor.cpp
	../../src/io/tls/connector.cpp
	../../src/io/tls/internal.cpp
	../../src/io/tls/params.cpp
	../../src/io/tls/stream.cpp
  )
endif ()
add_library(roker SHARED ${LIBROKER_SOURCES})
target_link_libraries(roker ${SOCI_LIBRARIES})
target_link_libraries(roker ${SOCIMYSQL_LIBRARIES})
target_link_libraries(roker ${SOCIORACLE_LIBRARIES})
target_link_libraries(roker ${SOCIPOSTGRESQL_LIBRARIES})
target_link_libraries(roker ${GNUTLS_LIBRARIES})

# Common sources files.
set(COMMON_SOURCES
	../../src/configuration/globals.cpp
	../../src/configuration/interface.cpp
	../../src/configuration/lexer.cpp
	../../src/configuration/log.cpp
	../../src/configuration/manager.cpp
	../../src/configuration/parser.cpp
	../../src/correlation/correlator.cpp
	../../src/correlation/node.cpp
	../../src/init.cpp
	../../src/multiplexing/internal.cpp
	../../src/multiplexing/publisher.cpp
	../../src/multiplexing/subscriber.cpp
	../../src/processing/failover_in.cpp
	../../src/processing/failover_out.cpp
	../../src/processing/feeder.cpp
	../../src/processing/listener.cpp
)

# Standalone binary.
add_executable(cbd ${COMMON_SOURCES} ../../src/daemon/main.cpp)
target_link_libraries(cbd roker)

# Module.
add_library(cbmod SHARED
	${COMMON_SOURCES}
	../../src/module/callbacks.cpp
	../../src/module/initial.cpp
	../../src/module/set_log_data.cpp
	../../src/module/main.cpp
)
target_link_libraries(cbmod roker)

# Install rules.
install(TARGETS roker cbd
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
