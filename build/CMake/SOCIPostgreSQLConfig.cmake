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

# Find SOCI includes and library for PostgreSQL.
#
#  SOCIPOSTGRESQL_INCLUDE_DIR - where to find soci-postgresql.h, etc.
#  SOCIPOSTGRESQL_LIBRARIES   - list of libraries when using SOCI PostgreSQL.

# Check for PostgreSQL main header.
set(CHECK_HEADERS libpq-fe.h)
set(CHECK_SUFFIXES "" postgresql)
if (POSTGRESQL_INCLUDE_DIR)
  find_path(POSTGRESQL_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATHS ${POSTGRESQL_INCLUDE_DIR}
    PATH_SUFFIXES ${CHECK_SUFFIXES}
    NO_DEFAULT_PATH)
else ()
  find_path(POSTGRESQL_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATH_SUFFIXES ${CHECK_SUFFIXES})
endif ()
if (POSTGRESQL_INCLUDE_DIR)
  message(STATUS "Found PostgreSQL headers in ${POSTGRESQL_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find PostgreSQL headers")
endif ()

# Check for SOCI PostgreSQL main header.
set(CHECK_HEADERS soci-postgresql.h)
set(CHECK_SUFFIXES "" postgresql)
if (SOCIPOSTGRESQL_INCLUDE_DIR)
  find_path(SOCIPOSTGRESQL_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATHS ${SOCIPOSTGRESQL_INCLUDE_DIR}
    PATH_SUFFIXES ${CHECK_SUFFIXES}
    NO_DEFAULT_PATH)
else ()
  find_path(SOCI_POSTGRESQL_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATHS ${SOCI_INCLUDE_DIR}
    PATH_SUFFIXES ${CHECK_SUFFIXES})
endif ()
if (SOCIPOSTGRESQL_INCLUDE_DIR)
  message(STATUS "Found SOCI PostgreSQL headers in ${SOCIPOSTGRESQL_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find SOCI PostgreSQL headers")
endif ()
set(SOCIPOSTGRESQL_INCLUDE_DIR ${POSTGRESQL_INCLUDE_DIR} ${SOCIPOSTGRESQL_INCLUDE_DIR})

# Check for SOCI PostgreSQL library.
set(CHECK_LIBRARIES soci_postgresql soci_postgresql-gcc-3_0)
if (SOCIPOSTGRESQL_LIBRARY_DIR)
  find_library(SOCIPOSTGRESQL_LIBRARIES
    NAMES ${CHECK_LIBRARIES}
    PATHS ${SOCIPOSTGRESQL_LIBRARIES}
    NO_DEFAULT_PATH)
else ()
  find_library(SOCIPOSTGRESQL_LIBRARIES
    NAMES ${CHECK_LIBRARIES})
endif ()
if (SOCIPOSTGRESQL_LIBRARIES)
  message(STATUS "Found SOCI PostgreSQL library ${SOCIPOSTGRESQL_LIBRARIES}")
else ()
  message(FATAL_ERROR "Could not find SOCI PostgreSQL library")
endif ()
