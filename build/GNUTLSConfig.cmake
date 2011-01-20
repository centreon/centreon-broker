##
## Copyright 2009-2011 MERETHIS
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
## For more information: contact@centreon.com
##

# Find GNU TLS includes and library.
#
#  GNUTLS_INCLUDE_DIR - where to find gnutls/gnutls.h, etc.
#  GNUTLS_LIBRARIES   - list of libraries when using GNU TLS.

# Check for GNU TLS main header.
set(CHECK_HEADERS gnutls/gnutls.h)
if (GNUTLS_INCLUDE_DIR)
  find_path(GNUTLS_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATHS ${GNUTLS_INCLUDE_DIR}
    NO_DEFAULT_PATH)
else ()
  find_path(GNUTLS_INCLUDE_DIR
    NAMES ${CHECK_HEADERS})
endif ()
if (GNUTLS_INCLUDE_DIR)
  message(STATUS "Found GNU TLS headers in ${GNUTLS_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find GNU TLS headers")
endif ()

# Check for GNU TLS library.
set(CHECK_LIBRARIES gnutls)
if (GNUTLS_LIBRARY_DIR)
  find_library(GNUTLS_LIBRARIES
    NAMES ${CHECK_LIBRARIES}
    PATHS ${GNUTLS_LIBRARY_DIR}
    NO_DEFAULT_PATH)
else ()
  find_library(GNUTLS_LIBRARIES
    NAMES ${CHECK_LIBRARIES})
endif ()
if (GNUTLS_LIBRARIES)
  message(STATUS "Found GNU TLS library ${GNUTLS_LIBRARIES}")
else ()
  message(FATAL_ERROR "Could not find GNU TLS library")
endif ()
