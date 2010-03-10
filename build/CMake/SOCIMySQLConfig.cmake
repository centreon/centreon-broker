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
if (MYSQL_INCLUDE_DIR)
  set(SOCI_CHECK ${MYSQL_INCLUDE_DIR})
else ()
  set(SOCI_CHECK /usr/include /usr/local/include)
endif ()
find_path(MYSQL_INCLUDE_DIR mysql.h
  PATHS ${SOCI_CHECK}
  PATH_SUFFIXES "" mysql
)
if (MYSQL_INCLUDE_DIR)
  message(STATUS "Found MySQL headers in ${MYSQL_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find MySQL headers in ${SOCI_CHECK}")
endif ()

# Check for SOCI MySQL main header.
if (SOCIMYSQL_INCLUDE_DIR)
  set(SOCI_CHECK ${SOCIMYSQL_INCLUDE_DIR})
else ()
  set(SOCI_CHECK ${SOCI_INCLUDE_DIR})
endif ()
find_path(SOCIMYSQL_INCLUDE_DIR soci-mysql.h
  PATHS ${SOCI_CHECK}
  PATH_SUFFIXES "" mysql
)
if (SOCIMYSQL_INCLUDE_DIR)
  message(STATUS "Found SOCI MySQL headers in ${SOCIMYSQL_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find SOCI MySQL headers in ${SOCI_CHECK}")
endif ()
set(SOCIMYSQL_INCLUDE_DIR ${MYSQL_INCLUDE_DIR} ${SOCIMYSQL_INCLUDE_DIR})

# Check for SOCI MySQL library.
if (SOCIMYSQL_LIBRARY_DIR)
  set(SOCI_CHECK ${SOCIMYSQL_LIBRARY_DIR})
else ()
  set(SOCI_CHECK /usr/lib /usr/local/lib)
endif ()
find_library(SOCIMYSQL_LIBRARIES
  NAMES soci_mysql soci_mysql-gcc-3_0
  PATHS ${SOCI_CHECK}
)
if (SOCIMYSQL_LIBRARIES)
  message(STATUS "Found SOCI MySQL library ${SOCIMYSQL_LIBRARIES}")
else ()
  message(FATAL_ERROR "Could not find SOCI library in ${SOCI_CHECK}")
endif ()
