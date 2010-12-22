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

# Find SOCI includes and library.
#
#  SOCI_INCLUDE_DIR - where to find soci.h, etc.
#  SOCI_LIBRARIES   - list of libraries when using SOCI.

# Check for SOCI main header.
set(CHECK_HEADERS soci.h)
set(CHECK_SUFFIXES "" soci)
if (SOCI_INCLUDE_DIR)
  find_path(SOCI_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATHS ${SOCI_INCLUDE_DIR}
    PATH_SUFFIXES ${CHECK_SUFFIXES}
    NO_DEFAULT_PATH)
else ()
  find_path(SOCI_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATH_SUFFIXES ${CHECK_SUFFIXES})
endif ()
if (SOCI_INCLUDE_DIR)
  message(STATUS "Found SOCI headers in ${SOCI_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find SOCI headers")
endif ()

# Check for SOCI library.
set(CHECK_LIBRARIES soci_core soci_core-gcc-3_0)
if (SOCI_LIBRARY_DIR)
  find_library(SOCI_LIBRARIES
    NAMES ${CHECK_LIBRARIES}
    PATHS ${SOCI_LIBRARY_DIR}
    NO_DEFAULT_PATH)
else ()
  find_library(SOCI_LIBRARIES
    NAMES ${CHECK_LIBRARIES})
endif ()
if (SOCI_LIBRARIES)
  message(STATUS "Found SOCI library ${SOCI_LIBRARIES}")
else ()
  message(FATAL_ERROR "Could not find SOCI library")
endif ()
