##
## Copyright 2011 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/rrd/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/rrd/src")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")

# Find librrd.
include(FindPkgConfig)
if (NOT PKG_CONFIG_FOUND)
  message(FATAL_ERROR "Could not find pkg-config.")
endif ()
pkg_check_modules(LIBRRD REQUIRED "librrd")

# RRD module.
set(RRD "70-rrd")
add_library("${RRD}" MODULE
  # Sources.
  "${SRC_DIR}/backend.cc"
  "${SRC_DIR}/cached.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/lib.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/output.cc"
  # Headers.
  "${INC_DIR}/com/centreon/broker/rrd/backend.hh"
  "${INC_DIR}/com/centreon/broker/rrd/cached.hh"
  "${INC_DIR}/com/centreon/broker/rrd/connector.hh"
  "${INC_DIR}/com/centreon/broker/rrd/factory.hh"
  "${INC_DIR}/com/centreon/broker/rrd/lib.hh"
  "${INC_DIR}/com/centreon/broker/rrd/output.hh"
)
set_target_properties("${RRD}" PROPERTIES PREFIX "")

# Compile with librrd flags.
if (LIBRRD_CFLAGS)
  get_source_file_property(RRD_SOURCE_CXXFLAGS "${SRC_DIR}/lib.cc" COMPILE_FLAGS)
  if (LIBRRD_SOURCE_CXXFLAGS)
    set_source_files_properties("${SRC_DIR}/lib.cc" PROPERTIES
      COMPILE_FLAGS "${RRD_SOURCE_CXXFLAGS} ${LIBRRD_CFLAGS}")
  else ()
    set_source_files_properties("${SRC_DIR}/lib.cc" PROPERTIES
      COMPILE_FLAGS "${LIBRRD_CFLAGS}")
  endif ()
endif ()
if (LIBRRD_LDFLAGS)
  get_target_property(RRD_LDFLAGS "${RRD}" LINK_FLAGS)
  if (RRD_LDFLAGS)
    set_target_properties("${RRD}" PROPERTIES
      LINK_FLAGS "${RRD_LDFLAGS} ${LIBRRD_LDFLAGS}")
  else ()
    set_target_properties("${RRD}" PROPERTIES
      LINK_FLAGS "${LIBRRD_LDFLAGS}")
  endif ()
endif ()

# Install rule.
install(TARGETS "${RRD}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
