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
if (POSTGRESQL_INCLUDE_DIR)
  set(SOCI_CHECK ${POSTGRESQL_INCLUDE_DIR})
else ()
  set(SOCI_CHECK /usr/include /usr/local/include)
endif ()
find_path(POSTGRESQL_INCLUDE_DIR libpq-fe.h
  PATHS ${SOCI_CHECK}
  PATH_SUFFIXES "" postgresql
)
if (POSTGRESQL_INCLUDE_DIR)
  message(STATUS "Found PostgreSQL headers in ${POSTGRESQL_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find PostgreSQL headers in ${SOCI_CHECK}")
endif ()

# Check for SOCI PostgreSQL main header.
if (SOCIPOSTGRESQL_INCLUDE_DIR)
  set(SOCI_CHECK ${SOCIPOSTGRESQL_INCLUDE_DIR})
else ()
  set(SOCI_CHECK ${SOCI_INCLUDE_DIR})
endif ()
find_path(SOCIPOSTGRESQL_INCLUDE_DIR soci-postgresql.h
  PATHS ${SOCI_CHECK}
  PATH_SUFFIXES "" postgresql
)
if (SOCIPOSTGRESQL_INCLUDE_DIR)
  message(STATUS "Found SOCI PostgreSQL headers in ${SOCIPOSTGRESQL_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find SOCI PostgreSQL headers in ${SOCI_CHECK}")
endif ()
set(SOCIPOSTGRESQL_INCLUDE_DIR ${POSTGRESQL_INCLUDE_DIR} ${SOCIPOSTGRESQL_INCLUDE_DIR})

# Check for SOCI PostgreSQL library.
if (SOCIPOSTGRESQL_LIBRARY_DIR)
  set(SOCI_CHECK ${SOCIPOSTGRESQL_LIBRARY_DIR})
else ()
  set(SOCI_CHECK /usr/lib /usr/local/lib)
endif ()
find_library(SOCIPOSTGRESQL_LIBRARIES
  NAMES soci_postgresql soci_postgresql-gcc-3_0
  PATHS ${SOCI_CHECK}
)
if (SOCIPOSTGRESQL_LIBRARIES)
  message(STATUS "Found SOCI PostgreSQL library ${SOCIPOSTGRESQL_LIBRARIES}")
else ()
  message(FATAL_ERROR "Could not find SOCI library in ${SOCI_CHECK}")
endif ()
