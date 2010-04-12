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

# Find SOCI includes and library for MySQL.
#
#  SOCIMYSQL_INCLUDE_DIR - where to find soci-mysql.h, etc.
#  SOCIMYSQL_LIBRARIES   - list of libraries when using SOCI MySQL.

# Check for MySQL main header.
set(CHECK_HEADERS mysql.h)
set(CHECK_SUFFIXES "" mysql)
if (MYSQL_INCLUDE_DIR)
  find_path(MYSQL_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATHS ${MYSQL_INCLUDE_DIR}
    PATH_SUFFIXES ${CHECK_SUFFIXES}
    NO_DEFAULT_PATH)
else ()
  find_path(MYSQL_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATH_SUFFIXES ${CHECK_SUFFIXES})
endif ()
if (MYSQL_INCLUDE_DIR)
  message(STATUS "Found MySQL headers in ${MYSQL_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find MySQL headers")
endif ()

# Check for SOCI MySQL main header.
set(CHECK_HEADERS soci-mysql.h)
set(CHECK_SUFFIXES "" mysql)
if (SOCIMYSQL_INCLUDE_DIR)
  find_path(SOCIMYSQL_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATHS ${SOCI_MYSQL_INCLUDE_DIR}
    PATH_SUFFIXES ${CHECK_SUFFIXES}
    NO_DEFAULT_PATH)
else ()
  find_path(SOCIMYSQL_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATHS ${SOCI_INCLUDE_DIR}
    PATH_SUFFIXES ${CHECK_SUFFIXES})
endif ()
if (SOCIMYSQL_INCLUDE_DIR)
  message(STATUS "Found SOCI MySQL headers in ${SOCIMYSQL_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find SOCI MySQL headers")
endif ()
set(SOCIMYSQL_INCLUDE_DIR ${MYSQL_INCLUDE_DIR} ${SOCIMYSQL_INCLUDE_DIR})

# Check for SOCI MySQL library.
set(CHECK_LIBRARIES soci_mysql soci_mysql-gcc-3_0)
if (SOCIMYSQL_LIBRARY_DIR)
  find_library(SOCIMYSQL_LIBRARIES
    NAMES ${CHECK_LIBRARIES}
    PATHS ${SOCIMYSQL_LIBRARY_DIR}
    NO_DEFAULT_PATH)
else ()
  find_library(SOCIMYSQL_LIBRARIES
    NAMES ${CHECK_LIBRARIES})
endif ()
if (SOCIMYSQL_LIBRARIES)
  message(STATUS "Found SOCI MySQL library ${SOCIMYSQL_LIBRARIES}")
else ()
  message(FATAL_ERROR "Could not find SOCI MySQL library")
endif ()
