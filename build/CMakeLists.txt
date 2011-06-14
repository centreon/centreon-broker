##
## Copyright 2009-2011 Merethis
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

# Global options.
cmake_minimum_required(VERSION 2.8)
project("Centreon Broker" C CXX)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
include_directories("../inc")# "../inc/daemon" "../inc/module")
set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/..")
set(INC_DIR "${PROJECT_SOURCE_DIR}/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")

# Enable or not the debug mode.
option(WITH_DEBUG "Generate debug messages." OFF)
if (NOT WITH_DEBUG)
  add_definitions(-DNDEBUG)
  message(STATUS "Debug messages disabled.")
else ()
  message(STATUS "Debug messages enabled.")
endif ()

# Find Qt.
find_package(Qt4 COMPONENTS QtCore QtNetwork QtSql QtXml REQUIRED)
include(${QT_USE_FILE})

# Check for GNU TLS.
#option(TLS "TLS support." ON)
#if (TLS)
#  set(GNUTLS_DIR .)
#  find_package(GNUTLS REQUIRED)
#  include_directories(${GNUTLS_INCLUDE_DIR})
#  add_definitions(-DUSE_TLS)
#endif ()

# Some files must be processed by Qt's moc.
qt4_wrap_cpp(QT_WRAPPED_SOURCES
  "${INC_DIR}/config/applier/endpoint.hh"
  "${INC_DIR}/processing/failover.hh"
  "${INC_DIR}/processing/feeder.hh"
)

# Event library.
set(LIBROKER_SOURCES
  # Sources.
  "${SRC_DIR}/config/applier/endpoint.cc"
  "${SRC_DIR}/config/applier/logger.cc"
  "${SRC_DIR}/config/applier/modules.cc"
  "${SRC_DIR}/config/applier/state.cc"
  "${SRC_DIR}/config/endpoint.cc"
  "${SRC_DIR}/config/logger.cc"
  "${SRC_DIR}/config/parser.cc"
  "${SRC_DIR}/config/state.cc"
  "${SRC_DIR}/events/acknowledgement.cc"
  "${SRC_DIR}/events/check.cc"
  "${SRC_DIR}/events/comment.cc"
  "${SRC_DIR}/events/custom_variable.cc"
  "${SRC_DIR}/events/custom_variable_status.cc"
  "${SRC_DIR}/events/dependency.cc"
  "${SRC_DIR}/events/downtime.cc"
  "${SRC_DIR}/events/event.cc"
  "${SRC_DIR}/events/event_handler.cc"
  "${SRC_DIR}/events/flapping_status.cc"
  "${SRC_DIR}/events/group.cc"
  "${SRC_DIR}/events/group_member.cc"
  "${SRC_DIR}/events/host.cc"
  "${SRC_DIR}/events/host_check.cc"
  "${SRC_DIR}/events/host_dependency.cc"
  "${SRC_DIR}/events/host_group.cc"
  "${SRC_DIR}/events/host_group_member.cc"
  "${SRC_DIR}/events/host_parent.cc"
  "${SRC_DIR}/events/host_service.cc"
  "${SRC_DIR}/events/host_service_status.cc"
  "${SRC_DIR}/events/host_state.cc"
  "${SRC_DIR}/events/host_status.cc"
  "${SRC_DIR}/events/instance.cc"
  "${SRC_DIR}/events/instance_status.cc"
#	../src/events/issue.cc
#	../src/events/issue_parent.cc
  "${SRC_DIR}/events/log_entry.cc"
  "${SRC_DIR}/events/module.cc"
  "${SRC_DIR}/events/notification.cc"
  "${SRC_DIR}/events/perfdata.cc"
  "${SRC_DIR}/events/service.cc"
  "${SRC_DIR}/events/service_check.cc"
  "${SRC_DIR}/events/service_dependency.cc"
  "${SRC_DIR}/events/service_group.cc"
  "${SRC_DIR}/events/service_group_member.cc"
  "${SRC_DIR}/events/service_state.cc"
  "${SRC_DIR}/events/service_status.cc"
  "${SRC_DIR}/events/state.cc"
  "${SRC_DIR}/events/status.cc"
  "${SRC_DIR}/exceptions/basic.cc"
#	../src/interface/db/destination.cc
#	../src/interface/db/internal.cc
#	../src/interface/ndo/base.cc
#	../src/interface/ndo/destination.cc
#	../src/interface/ndo/internal.cc
#	../src/interface/ndo/source.cc
#	../src/interface/ndo/source_destination.cc
#	../src/interface/xml/destination.cc
#	../src/interface/xml/internal.cc
#	../src/interface/xml/source.cc
  "${SRC_DIR}/io/acceptor.cc"
  "${SRC_DIR}/io/connector.cc"
  "${SRC_DIR}/io/data.cc"
  "${SRC_DIR}/io/endpoint.cc"
  "${SRC_DIR}/io/protocols.cc"
  "${SRC_DIR}/io/raw.cc"
  "${SRC_DIR}/io/stream.cc"
#  ../src/io/fd.cc
#	../src/io/file.cc
#	../src/io/net/ipv4.cc
#	../src/io/net/ipv6.cc
#	../src/io/net/socket.cc
#	../src/io/net/unix.cc
#	../src/io/split.cc
#	../src/io/text.cc
  "${SRC_DIR}/logging/backend.cc"
  "${SRC_DIR}/logging/file.cc"
  "${SRC_DIR}/logging/logger.cc"
  "${SRC_DIR}/logging/logging.cc"
  "${SRC_DIR}/logging/ostream.cc"
  "${SRC_DIR}/logging/syslogger.cc"
  "${SRC_DIR}/logging/temp_logger.cc"
  "${SRC_DIR}/logging/void_logger.cc"
  "${SRC_DIR}/misc/stringifier.cc"
  "${SRC_DIR}/modules/handle.cc"
  "${SRC_DIR}/modules/loader.cc"
  "${SRC_DIR}/multiplexing/internal.cc"
  "${SRC_DIR}/multiplexing/publisher.cc"
  "${SRC_DIR}/multiplexing/subscriber.cc"
  "${SRC_DIR}/processing/failover.cc"
  "${SRC_DIR}/processing/feeder.cc"
  "${QT_WRAPPED_SOURCES}"
  # Headers.
  "${INC_DIR}/config/applier/endpoint.hh"
  "${INC_DIR}/config/applier/logger.hh"
  "${INC_DIR}/config/applier/modules.hh"
  "${INC_DIR}/config/applier/state.hh"
  "${INC_DIR}/config/endpoint.hh"
  "${INC_DIR}/config/logger.hh"
  "${INC_DIR}/config/parser.hh"
  "${INC_DIR}/config/state.hh"
  "${INC_DIR}/events/acknowledgement.hh"
  "${INC_DIR}/events/check.hh"
  "${INC_DIR}/events/comment.hh"
  "${INC_DIR}/events/custom_variable.hh"
  "${INC_DIR}/events/custom_variable_status.hh"
  "${INC_DIR}/events/dependency.hh"
  "${INC_DIR}/events/downtime.hh"
  "${INC_DIR}/events/event.hh"
  "${INC_DIR}/events/event_handler.hh"
  "${INC_DIR}/events/events.hh"
  "${INC_DIR}/events/flapping_status.hh"
  "${INC_DIR}/events/group.hh"
  "${INC_DIR}/events/group_member.hh"
  "${INC_DIR}/events/host.hh"
  "${INC_DIR}/events/host_check.hh"
  "${INC_DIR}/events/host_dependency.hh"
  "${INC_DIR}/events/host_group.hh"
  "${INC_DIR}/events/host_group_member.hh"
  "${INC_DIR}/events/host_parent.hh"
  "${INC_DIR}/events/host_service.hh"
  "${INC_DIR}/events/host_service_status.hh"
  "${INC_DIR}/events/host_state.hh"
  "${INC_DIR}/events/host_status.hh"
  "${INC_DIR}/events/instance.hh"
  "${INC_DIR}/events/instance_status.hh"
  "${INC_DIR}/events/log_entry.hh"
  "${INC_DIR}/events/module.hh"
  "${INC_DIR}/events/notification.hh"
  "${INC_DIR}/events/service.hh"
  "${INC_DIR}/events/service_check.hh"
  "${INC_DIR}/events/service_dependency.hh"
  "${INC_DIR}/events/service_group.hh"
  "${INC_DIR}/events/service_group_member.hh"
  "${INC_DIR}/events/service_state.hh"
  "${INC_DIR}/events/service_status.hh"
  "${INC_DIR}/events/state.hh"
  "${INC_DIR}/events/status.hh"
  "${INC_DIR}/exceptions/basic.hh"
  "${INC_DIR}/io/acceptor.hh"
  "${INC_DIR}/io/connector.hh"
  "${INC_DIR}/io/data.hh"
  "${INC_DIR}/io/endpoint.hh"
  "${INC_DIR}/io/factory.hh"
  "${INC_DIR}/io/protocols.hh"
  "${INC_DIR}/io/raw.hh"
  "${INC_DIR}/io/stream.hh"
  "${INC_DIR}/logging/backend.hh"
  "${INC_DIR}/logging/defines.hh"
  "${INC_DIR}/logging/file.hh"
  "${INC_DIR}/logging/internal.hh"
  "${INC_DIR}/logging/logger.hh"
  "${INC_DIR}/logging/logging.hh"
  "${INC_DIR}/logging/ostream.hh"
  "${INC_DIR}/logging/syslogger.hh"
  "${INC_DIR}/logging/temp_logger.hh"
  "${INC_DIR}/logging/void_logger.hh"
  "${INC_DIR}/misc/stringifier.hh"
  "${INC_DIR}/modules/handle.hh"
  "${INC_DIR}/modules/loader.hh"
  "${INC_DIR}/multiplexing/internal.hh"
  "${INC_DIR}/multiplexing/publisher.hh"
  "${INC_DIR}/multiplexing/subscriber.hh"
  "${INC_DIR}/processing/failover.hh"
  "${INC_DIR}/processing/feeder.hh"
)
#if (TLS)
#  set(LIBROKER_SOURCES ${LIBROKER_SOURCES}
#	../src/io/tls/acceptor.cc
#	../src/io/tls/connector.cc
#	../src/io/tls/internal.cc
#	../src/io/tls/params.cc
#	../src/io/tls/stream.cc
#  )
#endif ()
#add_library(roker STATIC ${LIBROKER_SOURCES})
#target_link_libraries(roker ${GNUTLS_LIBRARIES})
#target_link_libraries(roker ${QT_LIBRARIES})
#target_link_libraries(roker ${CMAKE_THREAD_LIBS_INIT})

# Common sources files.
#set(COMMON_SOURCES
#	../src/correlation/correlator.cc
#	../src/correlation/node.cc
#	../src/correlation/parser.cc
#	../src/init.cc
#)

# Standalone binary.
add_executable(cbd
  ${LIBROKER_SOURCES}
  # Sources.
  "${SRC_DIR}/daemon/main.cc"
  "${SRC_DIR}/io/factory.cc"
  "${SRC_DIR}/io/stream.cc"
  "${SRC_DIR}/mapping.cc"
  # Headers.
  "${INC_DIR}/io/factory.hh"
  "${INC_DIR}/io/stream.hh"
  "${INC_DIR}/mapping.hh"
)
target_link_libraries(cbd ${QT_LIBRARIES})

# Centreon Engine/Nagios module.
add_library(cbmod SHARED
  # roker sources must be compiled within the module.
  ${LIBROKER_SOURCES}
  # Sources.
  "${SRC_DIR}/io/factory.cc"
  "${SRC_DIR}/io/stream.cc"
  "${SRC_DIR}/mapping.cc"
  "${SRC_DIR}/module/callbacks.cc"
  "${SRC_DIR}/module/initial.cc"
  "${SRC_DIR}/module/set_log_data.cc"
  "${SRC_DIR}/module/main.cc"
  # Headers.
  "${INC_DIR}/io/factory.hh"
  "${INC_DIR}/io/stream.hh"
  "${INC_DIR}/mapping.hh"
  "${INC_DIR}/module/callbacks.hh"
  "${INC_DIR}/module/initial.hh"
  "${INC_DIR}/module/internal.hh"
  "${INC_DIR}/module/set_log_data.hh"
)
target_link_libraries(cbmod ${QT_LIBRARIES})
set_target_properties(cbmod PROPERTIES PREFIX "")

# Install directories.
if (WITH_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${WITH_PREFIX}")
endif ()
if (WITH_PREFIX_BINARY)
  set(PREFIX_BINARY "${WITH_PREFIX_BINARY}")
else ()
  set(PREFIX_BINARY "bin")
endif ()
if (WITH_PREFIX_BROKER)
  set(PREFIX_BROKER "${WITH_PREFIX_BROKER}")
else ()
  set(PREFIX_BROKER "lib")
endif ()
if (WITH_PREFIX_MODULES)
  set(PREFIX_MODULES "${WITH_PREFIX_MODULES}")
else ()
  set(PREFIX_MODULES "lib/centreon-broker")
endif ()

# Install rules.
install(TARGETS cbd cbmod
  RUNTIME DESTINATION "${PREFIX_BINARY}"
  LIBRARY DESTINATION "${PREFIX_BROKER}"
)

# File module.
add_subdirectory("file")

# NDO module.
add_subdirectory("ndo")

# RRD module.
add_subdirectory("rrd")

# SQL module.
add_subdirectory("sql")

# Storage module.
add_subdirectory("storage")

# TCP module.
add_subdirectory("tcp")

# Print installation summary.
message(STATUS "")
message(STATUS "  Centreon Broker Installation Summary")
message(STATUS "")
message(STATUS "    System                  ${CMAKE_SYSTEM_NAME}")
message(STATUS "      - Version             ${CMAKE_SYSTEM_VERSION}")
message(STATUS "      - Processor           ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "    Installation Directory  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "      - Binary Directory    ${PREFIX_BINARY}")
message(STATUS "      - Broker Directory    ${PREFIX_BROKER}")
message(STATUS "      - Modules Directory   ${PREFIX_MODULES}")
message(STATUS "")
