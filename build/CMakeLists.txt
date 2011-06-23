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
set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/..")
set(INC_DIR "${PROJECT_SOURCE_DIR}/core/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/core/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/core/test")
include_directories("${INC_DIR}")

# Enable or not the debug mode.
option(WITH_DEBUG "Generate debug messages." ON)
if (NOT WITH_DEBUG)
  add_definitions(-DNDEBUG)
  message(STATUS "Debug messages disabled.")
else ()
  message(STATUS "Debug messages enabled.")
endif ()

# Find Qt.
find_package(Qt4 COMPONENTS QtCore QtNetwork QtSql QtXml REQUIRED)
include(${QT_USE_FILE})

# Some files must be processed by Qt's moc.
qt4_wrap_cpp(QT_WRAPPED_SOURCES
  "${INC_DIR}/com/centreon/broker/config/applier/endpoint.hh"
  "${INC_DIR}/com/centreon/broker/processing/failover.hh"
  "${INC_DIR}/com/centreon/broker/processing/feeder.hh"
)

# Core library.
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
  "${SRC_DIR}/exceptions/msg.cc"
#	../src/interface/ndo/base.cc
#	../src/interface/ndo/source_destination.cc
#	../src/interface/xml/destination.cc
#	../src/interface/xml/internal.cc
#	../src/interface/xml/source.cc
  "${SRC_DIR}/io/data.cc"
  "${SRC_DIR}/io/endpoint.cc"
  "${SRC_DIR}/io/factory.cc"
  "${SRC_DIR}/io/protocols.cc"
  "${SRC_DIR}/io/raw.cc"
  "${SRC_DIR}/io/stream.cc"
#	../src/io/net/unix.cc
#	../src/io/split.cc
  "${SRC_DIR}/logging/backend.cc"
  "${SRC_DIR}/logging/file.cc"
  "${SRC_DIR}/logging/logger.cc"
  "${SRC_DIR}/logging/logging.cc"
  "${SRC_DIR}/logging/ostream.cc"
  "${SRC_DIR}/logging/syslogger.cc"
  "${SRC_DIR}/logging/temp_logger.cc"
  "${SRC_DIR}/logging/void_logger.cc"
  "${SRC_DIR}/mapping/entry.cc"
  "${SRC_DIR}/mapping/source.cc"
  "${SRC_DIR}/misc/stringifier.cc"
  "${SRC_DIR}/modules/handle.cc"
  "${SRC_DIR}/modules/loader.cc"
  "${SRC_DIR}/multiplexing/internal.cc"
  "${SRC_DIR}/multiplexing/publisher.cc"
  "${SRC_DIR}/multiplexing/subscriber.cc"
  "${SRC_DIR}/processing/failover.cc"
  "${SRC_DIR}/processing/feeder.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/config/applier/endpoint.hh"
  "${INC_DIR}/com/centreon/broker/config/applier/logger.hh"
  "${INC_DIR}/com/centreon/broker/config/applier/modules.hh"
  "${INC_DIR}/com/centreon/broker/config/applier/state.hh"
  "${INC_DIR}/com/centreon/broker/config/endpoint.hh"
  "${INC_DIR}/com/centreon/broker/config/logger.hh"
  "${INC_DIR}/com/centreon/broker/config/parser.hh"
  "${INC_DIR}/com/centreon/broker/config/state.hh"
  "${INC_DIR}/com/centreon/broker/exceptions/msg.hh"
  "${INC_DIR}/com/centreon/broker/io/data.hh"
  "${INC_DIR}/com/centreon/broker/io/endpoint.hh"
  "${INC_DIR}/com/centreon/broker/io/factory.hh"
  "${INC_DIR}/com/centreon/broker/io/protocols.hh"
  "${INC_DIR}/com/centreon/broker/io/raw.hh"
  "${INC_DIR}/com/centreon/broker/io/stream.hh"
  "${INC_DIR}/com/centreon/broker/logging/backend.hh"
  "${INC_DIR}/com/centreon/broker/logging/defines.hh"
  "${INC_DIR}/com/centreon/broker/logging/file.hh"
  "${INC_DIR}/com/centreon/broker/logging/internal.hh"
  "${INC_DIR}/com/centreon/broker/logging/logger.hh"
  "${INC_DIR}/com/centreon/broker/logging/logging.hh"
  "${INC_DIR}/com/centreon/broker/logging/ostream.hh"
  "${INC_DIR}/com/centreon/broker/logging/syslogger.hh"
  "${INC_DIR}/com/centreon/broker/logging/temp_logger.hh"
  "${INC_DIR}/com/centreon/broker/logging/void_logger.hh"
  "${INC_DIR}/com/centreon/broker/mapping/entry.hh"
  "${INC_DIR}/com/centreon/broker/mapping/property.hh"
  "${INC_DIR}/com/centreon/broker/mapping/source.hh"
  "${INC_DIR}/com/centreon/broker/misc/stringifier.hh"
  "${INC_DIR}/com/centreon/broker/modules/handle.hh"
  "${INC_DIR}/com/centreon/broker/modules/loader.hh"
  "${INC_DIR}/com/centreon/broker/multiplexing/internal.hh"
  "${INC_DIR}/com/centreon/broker/multiplexing/publisher.hh"
  "${INC_DIR}/com/centreon/broker/multiplexing/subscriber.hh"
  "${INC_DIR}/com/centreon/broker/processing/failover.hh"
  "${INC_DIR}/com/centreon/broker/processing/feeder.hh"
)

# Static library.
add_library("roker" STATIC ${LIBROKER_SOURCES} ${QT_WRAPPED_SOURCES})
target_link_libraries("roker" ${QT_LIBRARIES})
#target_link_libraries(roker ${CMAKE_THREAD_LIBS_INIT})

# Common sources files.
#set(COMMON_SOURCES
#	../src/correlation/correlator.cc
#	../src/correlation/node.cc
#	../src/correlation/parser.cc
#	../src/init.cc
#)

# Standalone binary.
set(DAEMON "cbd")
add_executable("${DAEMON}"
  # Core sources.
  ${LIBROKER_SOURCES}
  # Core Qt sources.
  ${QT_WRAPPED_SOURCES}
  # Sources.
  "${SRC_DIR}/main.cc"
)
target_link_libraries("${DAEMON}" ${QT_LIBRARIES})

# Enable testing.
option(WITH_TESTING "Generate unit tests." OFF)
if (WITH_TESTING)
  enable_testing()

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
endif ()

# Install directories.
if (WITH_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${WITH_PREFIX}")
endif ()
if (WITH_PREFIX_ARCHIVE)
  set(PREFIX_ARCHIVE "${WITH_PREFIX_ARCHIVE}")
else ()
  set(PREFIX_ARCHIVE "lib")
endif ()
if (WITH_PREFIX_BINARY)
  set(PREFIX_BINARY "${WITH_PREFIX_BINARY}")
else ()
  set(PREFIX_BINARY "bin")
endif ()
if (WITH_PREFIX_LIBRARY)
  set(PREFIX_LIBRARY "${WITH_PREFIX_LIBRARY}")
else ()
  set(PREFIX_LIBRARY "lib")
endif ()
if (WITH_PREFIX_MODULES)
  set(PREFIX_MODULES "${WITH_PREFIX_MODULES}")
else ()
  set(PREFIX_MODULES "lib/centreon-broker")
endif ()

# Install rule.
install(TARGETS "roker"
  ARCHIVE DESTINATION "${PREFIX_ARCHIVE}"
)

# Install rule.
install(TARGETS "${DAEMON}"
  RUNTIME DESTINATION "${PREFIX_BINARY}"
)

# File module.
add_subdirectory("file")

# NDO module.
add_subdirectory("ndo")

# NEB module.
add_subdirectory("neb")

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
message(STATUS "      - Archive Directory   ${PREFIX_ARCHIVE}")
message(STATUS "      - Binary Directory    ${PREFIX_BINARY}")
message(STATUS "      - Library Directory   ${PREFIX_LIBRARY}")
message(STATUS "      - Modules Directory   ${PREFIX_MODULES}")
if (WITH_DEBUG)
  message(STATUS "    Debug messages          enabled")
else ()
  message(STATUS "    Debug messages          disabled")
endif ()
message(STATUS "")
