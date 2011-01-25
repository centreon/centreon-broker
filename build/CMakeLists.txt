##
## Copyright 2009-2011 MERETHIS
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
## For more information: contact@centreon.com
##

# Global options.
cmake_minimum_required(VERSION 2.8)
project("Centreon Broker" C CXX)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
include_directories("../inc" "../inc/daemon" "../inc/module")

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

# Find pthread library.
include("FindThreads")
if (NOT CMAKE_USE_PTHREADS_INIT)
  message(FATAL_ERROR "Could not find pthread library.")
endif ()

# Find QtSql module.
find_package(Qt4 COMPONENTS QtCore QtSql QtXml REQUIRED)
include(${QT_USE_FILE})

# Activate proper DBMS.
option(WITH_MYSQL "Activate MySQL support." ON)
if (WITH_MYSQL)
  add_definitions(-DUSE_MYSQL)
endif ()
option(WITH_ORACLE "Activate Oracle support." OFF)
if (WITH_ORACLE)
  add_definitions(-DUSE_ORACLE)
endif ()
option(WITH_POSTGRESQL "Activate PostgreSQL." OFF)
if (WITH_POSTGRESQL)
  add_definitions(-DUSE_POSTGRESQL)
endif ()

# Check for GNU TLS.
option(TLS "TLS support." ON)
if (TLS)
  set(GNUTLS_DIR .)
  find_package(GNUTLS REQUIRED)
  include_directories(${GNUTLS_INCLUDE_DIR})
  add_definitions(-DUSE_TLS)
endif ()

# libroker library.
set(LIBROKER_SOURCES
	../src/concurrency/condition_variable.cc
	../src/concurrency/lock.cc
	../src/concurrency/mutex.cc
	../src/concurrency/thread.cc
	../src/concurrency/thread_listener.cc
	../src/events/acknowledgement.cc
	../src/events/check.cc
	../src/events/comment.cc
	../src/events/custom_variable.cc
	../src/events/custom_variable_status.cc
	../src/events/dependency.cc
	../src/events/downtime.cc
	../src/events/event.cc
	../src/events/event_handler.cc
	../src/events/flapping_status.cc
	../src/events/group.cc
	../src/events/group_member.cc
	../src/events/host.cc
	../src/events/host_check.cc
	../src/events/host_dependency.cc
	../src/events/host_group.cc
	../src/events/host_group_member.cc
	../src/events/host_parent.cc
	../src/events/host_service.cc
	../src/events/host_service_status.cc
	../src/events/host_status.cc
	../src/events/instance.cc
	../src/events/instance_status.cc
	../src/events/issue.cc
	../src/events/issue_parent.cc
	../src/events/log_entry.cc
	../src/events/module.cc
	../src/events/notification.cc
	../src/events/service.cc
	../src/events/service_check.cc
	../src/events/service_dependency.cc
	../src/events/service_group.cc
	../src/events/service_group_member.cc
	../src/events/service_status.cc
	../src/events/state.cc
	../src/events/status.cc
	../src/exceptions/basic.cc
	../src/exceptions/concurrency.cc
	../src/exceptions/retval.cc
	../src/interface/db/destination.cc
	../src/interface/db/internal.cc
	../src/interface/destination.cc
	../src/interface/ndo/base.cc
	../src/interface/ndo/destination.cc
	../src/interface/ndo/internal.cc
	../src/interface/ndo/source.cc
	../src/interface/ndo/source_destination.cc
	../src/interface/source.cc
	../src/interface/source_destination.cc
	../src/interface/xml/destination.cc
	../src/interface/xml/internal.cc
	../src/interface/xml/source.cc
	../src/io/acceptor.cc
	../src/io/fd.cc
	../src/io/file.cc
	../src/io/net/ipv4.cc
	../src/io/net/ipv6.cc
	../src/io/net/socket.cc
	../src/io/net/unix.cc
	../src/io/split.cc
	../src/io/stream.cc
	../src/io/text.cc
	../src/logging/backend.cc
	../src/logging/file.cc
	../src/logging/logger.cc
	../src/logging/logging.cc
	../src/logging/ostream.cc
	../src/logging/syslogger.cc
	../src/logging/temp_logger.cc
	../src/logging/void_logger.cc
	../src/mapping.cc
	../src/misc/stringifier.cc
)
if (TLS)
  set(LIBROKER_SOURCES ${LIBROKER_SOURCES}
	../src/io/tls/acceptor.cc
	../src/io/tls/connector.cc
	../src/io/tls/internal.cc
	../src/io/tls/params.cc
	../src/io/tls/stream.cc
  )
endif ()
add_library(roker SHARED ${LIBROKER_SOURCES})
target_link_libraries(roker ${GNUTLS_LIBRARIES})
target_link_libraries(roker ${QT_LIBRARIES})
target_link_libraries(roker ${CMAKE_THREAD_LIBS_INIT})

# Common sources files.
set(COMMON_SOURCES
	../src/config/factory/interface.cc
	../src/config/factory/logger.cc
	../src/config/globals.cc
	../src/config/handle.cc
	../src/config/interface.cc
	../src/config/logger.cc
	../src/config/parser.cc
	../src/correlation/correlator.cc
	../src/correlation/node.cc
	../src/correlation/parser.cc
	../src/init.cc
	../src/multiplexing/internal.cc
	../src/multiplexing/publisher.cc
	../src/multiplexing/subscriber.cc
	../src/processing/failover_in.cc
	../src/processing/failover_out.cc
	../src/processing/feeder.cc
	../src/processing/listener.cc
)

# Standalone binary.
add_executable(cbd ${COMMON_SOURCES} ../src/daemon/main.cc)
target_link_libraries(cbd roker ${QT_LIBRARIES})

# Module.
add_library(cbmod SHARED
	${COMMON_SOURCES}
	../src/module/callbacks.cc
	../src/module/initial.cc
	../src/module/set_log_data.cc
	../src/module/main.cc
)
target_link_libraries(cbmod roker ${QT_LIBRARIES})

# Install rules.
install(TARGETS roker cbd
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
