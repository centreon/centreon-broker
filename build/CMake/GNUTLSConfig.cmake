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

# Find GNU TLS includes and library.
#
#  GNUTLS_INCLUDE_DIR - where to find gnutls/gnutls.h, etc.
#  GNUTLS_LIBRARIES   - list of libraries when using GNU TLS.

# Check for GNU TLS main header.
if (GNUTLS_INCLUDE_DIR)
  set(GNUTLS_CHECK ${GNUTLS_INCLUDE_DIR})
else ()
  set(GNUTLS_CHECK /usr/include /usr/local/include)
endif ()
find_path(GNUTLS_INCLUDE_DIR gnutls/gnutls.h
  PATHS ${GNUTLS_CHECK}
)
if (GNUTLS_INCLUDE_DIR)
  message(STATUS "Found GNU TLS headers in ${GNUTLS_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find GNU TLS headers in ${GNUTLS_CHECK}")
endif ()

# Check for GNU TLS library.
if (GNUTLS_LIBRARY_DIR)
  set(GNUTLS_CHECK ${GNUTLS_LIBRARY_DIR})
else ()
  set(GNUTLS_CHECK /usr/lib /usr/local/lib)
endif ()
find_library(GNUTLS_LIBRARIES
  NAMES gnutls
  PATHS ${GNUTLS_CHECK}
)
if (GNUTLS_LIBRARIES)
  message(STATUS "Found GNU TLS library ${GNUTLS_LIBRARIES}")
else ()
  message(FATAL_ERROR "Could not find GNU TLS library in ${GNUTLS_CHECK}")
endif ()
