##
## Copyright 2011 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/ndo/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/ndo/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/ndo/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/correlation/inc")
include_directories("${PROJECT_SOURCE_DIR}/dumper/inc")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")

# Some files must be processed by Qt's moc.
unset(QT_WRAPPED_SOURCES)
qt4_wrap_cpp(QT_WRAPPED_SOURCES
  "${INC_DIR}/com/centreon/broker/ndo/acceptor.hh")

# NDO module.
set(NDO "80-ndo")
set(NDO "${NDO}" PARENT_SCOPE)
add_library("${NDO}" SHARED
  # Sources.
  "${SRC_DIR}/acceptor.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/input.cc"
  "${SRC_DIR}/internal.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/output.cc"
  "${SRC_DIR}/stream.cc"
  ${QT_WRAPPED_SOURCES}
  # Headers.
  "${INC_DIR}/com/centreon/broker/ndo/acceptor.hh"
  "${INC_DIR}/com/centreon/broker/ndo/connector.hh"
  "${INC_DIR}/com/centreon/broker/ndo/factory.hh"
  "${INC_DIR}/com/centreon/broker/ndo/input.hh"
  "${INC_DIR}/com/centreon/broker/ndo/internal.hh"
  "${INC_DIR}/com/centreon/broker/ndo/output.hh"
  "${INC_DIR}/com/centreon/broker/ndo/stream.hh"
)
target_link_libraries("${NDO}" ${QT_LIBRARIES})
set_target_properties("${NDO}" PROPERTIES PREFIX "")
get_property(NDO_MODULE_PATH
  TARGET "${NDO}"
  PROPERTY LOCATION)
set(NDO_MODULE_PATH "${NDO_MODULE_PATH}" PARENT_SCOPE)

# Testing.
if (WITH_TESTING)
  include_directories("${PROJECT_SOURCE_DIR}/file/inc")
  include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
  set(TEST_LIBRARIES "${NDO}" "${STORAGE}" "${NEB}" "${CORRELATION}" "roker")

  # stream tests.
  #   Parse erased input.
  set(TEST_NAME "ndo_stream_erased_input")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/stream/erased_input.cc")
  target_link_libraries("${TEST_NAME}" "${FILE}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")
endif ()

# Install rule.
install(TARGETS "${NDO}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
