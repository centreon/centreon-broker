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

# Find SOCI includes and library for Oracle.
#
#  SOCIORACLE_INCLUDE_DIR - where to find soci-orable.h, etc.
#  SOCIORACLE_LIBRARIES   - list of libraries when using SOCI Oracle.

# Check for Oracle main header.
set(CHECK_HEADERS oci.h)
set(CHECK_SUFFIXES "" oracle)
if (ORACLE_INCLUDE_DIR)
  find_path(ORACLE_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATHS ${ORACLE_INCLUDE_DIR}
    PATH_SUFFIXES ${CHECK_SUFFIXES}
    NO_DEFAULT_PATH)
else ()
  find_path(ORACLE_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATH_SUFFIXES ${CHECK_SUFFIXES})
endif ()
if (ORACLE_INCLUDE_DIR)
  message(STATUS "Found Oracle headers in ${ORACLE_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find Oracle headers")
endif ()

# Check for SOCI Oracle main header.
set(CHECK_HEADERS soci-oracle.h)
set(CHECK_SUFFIXES "" oracle)
if (SOCIORACLE_INCLUDE_DIR)
  find_path(SOCIORACLE_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATHS ${SOCI_ORACLE_INCLUDE_DIR}
    PATH_SUFFIXES ${CHECK_SUFFIXES}
    NO_DEFAULT_PATH)
else ()
  find_path(SOCIORACLE_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATHS ${SOCI_INCLUDE_DIR}
    PATH_SUFFIXES ${CHECK_SUFFIXES})
endif ()
if (SOCIORACLE_INCLUDE_DIR)
  message(STATUS "Found SOCI Oracle headers in ${SOCIORACLE_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find SOCI Oracle headers")
endif ()
set(SOCIORACLE_INCLUDE_DIR ${ORACLE_INCLUDE_DIR} ${SOCIORACLE_INCLUDE_DIR})

# Check for SOCI Oracle library.
set(CHECK_LIBRARIES soci_oracle soci_oracle-gcc-3_0)
if (SOCIORACLE_LIBRARY_DIR)
  find_library(SOCIORACLE_LIBRARIES
    NAMES ${CHECK_LIBRARIES}
    PATHS ${SOCIORACLE_LIBRARY_DIR}
    NO_DEFAULT_PATH)
else ()
  find_library(SOCIORACLE_LIBRARIES
    NAMES ${CHECK_LIBRARIES})
endif ()
if (SOCIORACLE_LIBRARIES)
  message(STATUS "Found SOCI Oracle library ${SOCIORACLE_LIBRARIES}")
else ()
  message(FATAL_ERROR "Could not find SOCI Oracle library")
endif ()
