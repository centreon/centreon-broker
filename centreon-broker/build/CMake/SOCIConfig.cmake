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
if (SOCI_INCLUDE_DIR)
  set(SOCI_CHECK ${SOCI_INCLUDE_DIR})
else ()
  set(SOCI_CHECK /usr/include /usr/local/include)
endif ()
find_path(SOCI_INCLUDE_DIR soci.h
  PATHS ${SOCI_CHECK}
  PATH_SUFFIXES "" soci
)
if (SOCI_INCLUDE_DIR)
  message(STATUS "Found SOCI headers in ${SOCI_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find SOCI headers in ${SOCI_CHECK}")
endif ()

# Check for SOCI library.
if (SOCI_LIBRARY_DIR)
  set(SOCI_CHECK ${SOCI_LIBRARY_DIR})
else ()
  set(SOCI_CHECK /usr/lib /usr/local/lib)
endif ()
find_library(SOCI_LIBRARIES
  NAMES soci_core soci_core-gcc-3_0
  PATHS ${SOCI_CHECK}
)
if (SOCI_LIBRARIES)
  message(STATUS "Found SOCI library ${SOCI_LIBRARIES}")
else ()
  message(FATAL_ERROR "Could not find SOCI library in ${SOCI_CHECK}")
endif ()
