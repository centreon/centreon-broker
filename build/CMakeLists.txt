##
## Copyright 2009-2012 Merethis
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

# Global options.
cmake_minimum_required(VERSION 2.8)
project("Centreon Broker" C CXX)
set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/..")
set(INC_DIR "${PROJECT_SOURCE_DIR}/core/inc")
include_directories("${INC_DIR}")
set(INC_DIR "${PROJECT_SOURCE_DIR}/core/inc/com/centreon/broker")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/core/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/core/test")

# Version.
set(BROKER_MAJOR "2")
set(BROKER_MINOR "1")
set(BROKER_PATCH "1")
set(BROKER_VERSION "${BROKER_MAJOR}.${BROKER_MINOR}.${BROKER_PATCH}")

# Find Qt.
find_package(Qt4 4.5.0 COMPONENTS QtCore QtNetwork QtSql QtXml REQUIRED)
include(${QT_USE_FILE})
set(QT_VERSION "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
if (("${QT_VERSION}" VERSION_EQUAL "4.7.0")
    OR ("${QT_VERSION}" VERSION_EQUAL "4.7.1"))
  message(WARNING "Qt versions 4.7.0 and 4.7.1 contain a bug that might prevent Centreon Broker from working properly.")
endif ()

# Some files must be processed by Qt's moc.
qt4_wrap_cpp(QT_WRAPPED_SOURCES
  "${INC_DIR}/config/applier/endpoint.hh"
  "${INC_DIR}/logging/backend.hh"
  "${INC_DIR}/logging/manager.hh"
  "${INC_DIR}/multiplexing/engine.hh"
  "${INC_DIR}/multiplexing/hooker.hh"
  "${INC_DIR}/processing/failover.hh"
  "${INC_DIR}/processing/feeder.hh"
)

# Core library.
set(LIBROKER_SOURCES
  # Sources.
 "${SRC_DIR}/config/applier/endpoint.cc" #
  "${SRC_DIR}/config/applier/init.cc"
  "${SRC_DIR}/config/applier/logger.cc" #
  "${SRC_DIR}/config/applier/modules.cc" #
  "${SRC_DIR}/config/applier/state.cc" #
  "${SRC_DIR}/config/endpoint.cc" #
  "${SRC_DIR}/config/logger.cc"
  "${SRC_DIR}/config/parser.cc" #
  "${SRC_DIR}/config/state.cc" #
  "${SRC_DIR}/exceptions/msg.cc"
  "${SRC_DIR}/exceptions/with_pointer.cc" #
# #	../src/interface/xml/destination.cc
# #	../src/interface/xml/internal.cc
# #	../src/interface/xml/source.cc
  "${SRC_DIR}/io/data.cc"
  "${SRC_DIR}/io/endpoint.cc"
  "${SRC_DIR}/io/exceptions/shutdown.cc"
  "${SRC_DIR}/io/factory.cc" #
  "${SRC_DIR}/io/protocols.cc" #
  "${SRC_DIR}/io/raw.cc"
  "${SRC_DIR}/io/stream.cc"
# #	../src/io/split.cc
  "${SRC_DIR}/logging/backend.cc"
  "${SRC_DIR}/logging/file.cc"
  "${SRC_DIR}/logging/logger.cc"
  "${SRC_DIR}/logging/logging.cc"
  "${SRC_DIR}/logging/manager.cc"
  "${SRC_DIR}/logging/syslogger.cc"
  "${SRC_DIR}/logging/temp_logger.cc"
  "${SRC_DIR}/mapping/entry.cc" #
  "${SRC_DIR}/mapping/source.cc" #
  "${SRC_DIR}/misc/stringifier.cc"
  "${SRC_DIR}/misc/global_lock.cc"
  "${SRC_DIR}/modules/handle.cc" #
  "${SRC_DIR}/modules/loader.cc" #
  "${SRC_DIR}/multiplexing/engine.cc"
  "${SRC_DIR}/multiplexing/hooker.cc"
  "${SRC_DIR}/multiplexing/internal.cc"
  "${SRC_DIR}/multiplexing/publisher.cc"
  "${SRC_DIR}/multiplexing/subscriber.cc"
  "${SRC_DIR}/processing/failover.cc" #
  "${SRC_DIR}/processing/feeder.cc" #
  "${QT_WRAPPED_SOURCES}"
#   # Headers.
  "${INC_DIR}/config/applier/endpoint.hh"
  "${INC_DIR}/config/applier/init.hh"
  "${INC_DIR}/config/applier/logger.hh"
  "${INC_DIR}/config/applier/modules.hh"
  "${INC_DIR}/config/applier/state.hh"
  "${INC_DIR}/config/endpoint.hh"
  "${INC_DIR}/config/logger.hh"
  "${INC_DIR}/config/parser.hh"
  "${INC_DIR}/config/state.hh"
  "${INC_DIR}/exceptions/msg.hh"
  "${INC_DIR}/exceptions/with_pointer.hh"
  "${INC_DIR}/io/data.hh"
  "${INC_DIR}/io/endpoint.hh"
  "${INC_DIR}/io/exceptions/shutdown.hh"
  "${INC_DIR}/io/factory.hh"
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
  "${INC_DIR}/misc/stringifier.hh"
  "${INC_DIR}/misc/global_lock.hh"
  "${INC_DIR}/modules/handle.hh"
  "${INC_DIR}/modules/loader.hh"
  "${INC_DIR}/multiplexing/engine.hh"
  "${INC_DIR}/multiplexing/hooker.hh"
  "${INC_DIR}/multiplexing/internal.hh"
  "${INC_DIR}/multiplexing/publisher.hh"
  "${INC_DIR}/multiplexing/subscriber.hh"
  "${INC_DIR}/processing/failover.hh"
  "${INC_DIR}/processing/feeder.hh"
)

# Static library.
add_library("roker" STATIC ${LIBROKER_SOURCES})
if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
  get_target_property(ROKER_CXXFLAGS "roker" COMPILE_FLAGS)
  if (ROKER_CXXFLAGS)
    set_target_properties("roker" PROPERTIES
      COMPILE_FLAGS "${ROKER_CXXFLAGS} -fPIC")
  else ()
    set_target_properties("roker" PROPERTIES
      COMPILE_FLAGS "-fPIC")
  endif ()
endif ()
target_link_libraries("roker" ${QT_LIBRARIES})

# Standalone binary.
set(DAEMON "cbd")
add_executable("${DAEMON}"
  # Sources.
  "${SRC_DIR}/main.cc"
)
if (CMAKE_COMPILER_IS_GNUCXX)
  # Flags needed to include all symbols in binary.
  target_link_libraries("${DAEMON}"
    "-Wl,--whole-archive" "roker" "-Wl,--no-whole-archive")
else ()
  target_link_libraries("${DAEMON}" "roker")
endif ()

# Enable testing.
option(WITH_TESTING "Generate unit tests." OFF)
if (WITH_TESTING)
  enable_testing()
  include_directories("${PROJECT_SOURCE_DIR}/core")

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
  #   Concurrent logging.
  set(TEST_NAME "core_logging_logging_concurrent")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/logging/logging/concurrent.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
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
  #   Default constructor.
  set(TEST_NAME "core_multiplexing_subscriber_ctor_default")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/multiplexing/subscriber/ctor_default.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Process.
  set(TEST_NAME "core_multiplexing_subscriber_process")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/multiplexing/subscriber/process.cc")
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
  set(TEST_NAME "core_multiplexing_engine_read")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/multiplexing/publisher/read.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Write.
  set(TEST_NAME "core_multiplexing_engine_write")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/multiplexing/publisher/write.cc")
  target_link_libraries("${TEST_NAME}" "roker")
  add_test("${TEST_NAME}" "${TEST_NAME}")


  # processing namespace tests.
  # processing::feeder tests.
  add_library("ccb_core_processing_feeder" STATIC
    "${TEST_DIR}/processing/feeder/common.cc"
    "${TEST_DIR}/processing/feeder/setable_endpoint.cc"
    "${TEST_DIR}/processing/feeder/setable_stream.cc")
  set(TEST_LIBRARIES "ccb_core_processing_feeder" "roker")
  #   Simple feeding.
  set(TEST_NAME "core_processing_feeder_simple")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/processing/feeder/simple.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Feeding with error.
  set(TEST_NAME "core_processing_feeder_error")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/processing/feeder/error.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  # processing::failover tests.
  #   Simple feeding.
  set(TEST_NAME "core_processing_failover_feed")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/processing/failover/feed.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Retry interval.
  set(TEST_NAME "core_processing_failover_retry_interval")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/processing/failover/retry_interval.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Failing intermediate.
  set(TEST_NAME "core_processing_failover_failed_intermediate")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/processing/failover/failed_intermediate.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Event loop break.
  set(TEST_NAME "core_processing_failover_loop_break")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/processing/failover/loop_break.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Exit while no data is available.
  set(TEST_NAME "core_processing_failover_exit_with_no_data")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/processing/failover/exit_with_no_data.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Reread events from failover first.
  set(TEST_NAME "core_processing_failover_reread_first")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/processing/failover/reread_first.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
  #   Recovery.
  set(TEST_NAME "core_processing_failover_recovery")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/processing/failover/recovery.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")


  # cbd tests.
  add_test("cbd_help" "cbd" "-h")
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

# Configure files.
configure_file("${PROJECT_SOURCE_DIR}/build/doc/Doxyfile.in"
  "${PROJECT_SOURCE_DIR}/build/doc/Doxyfile")
configure_file("${PROJECT_SOURCE_DIR}/build/doc/header.tex.in"
  "${PROJECT_SOURCE_DIR}/build/doc/header.tex")

# Documentation.
add_subdirectory("doc")

# NEB module.
add_subdirectory("neb")

# File module.
add_subdirectory("file")

# Compression module.
add_subdirectory("compression")

# Correlation module.
add_subdirectory("correlation")

# Local sockets module.
add_subdirectory("local")

# RRD module.
add_subdirectory("rrd")

# SQL module.
add_subdirectory("sql")

# Storage module.
add_subdirectory("storage")

# TCP module.
add_subdirectory("tcp")

# NDO module.
add_subdirectory("ndo")

# Print installation summary.
message(STATUS "")
message(STATUS "  Centreon Broker Installation Summary")
message(STATUS "")
message(STATUS "    System                  ${CMAKE_SYSTEM_NAME}")
message(STATUS "      - Version             ${CMAKE_SYSTEM_VERSION}")
message(STATUS "      - Processor           ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "    Compiler                ${CMAKE_CXX_COMPILER}")
message(STATUS "      - Flags               ${CMAKE_CXX_FLAGS}")
message(STATUS "    Centreon Broker")
message(STATUS "      - Version             ${BROKER_VERSION}")
message(STATUS "    Qt")
message(STATUS "      - Version             ${QT_VERSION}")
message(STATUS "    Installation Directory  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "      - Archive Directory   ${PREFIX_ARCHIVE}")
message(STATUS "      - Binary Directory    ${PREFIX_BINARY}")
message(STATUS "      - Library Directory   ${PREFIX_LIBRARY}")
message(STATUS "      - Modules Directory   ${PREFIX_MODULES}")
if (WITH_TESTING)
  message(STATUS "    Unit tests              enabled")
else ()
  message(STATUS "    Unit tests              disabled")
endif ()
if (WITH_DOCUMENTATION)
  message(STATUS "    Documentation           enabled")
else ()
  message(STATUS "    Documentation           disabled")
endif ()
message(STATUS "")
