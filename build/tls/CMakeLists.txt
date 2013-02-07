##
## Copyright 2013 Merethis
##
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/tls/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/tls/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/tls/test")
include_directories("${INC_DIR}")
set(INC_DIR "${INC_DIR}/com/centreon/broker/tls")

# Find GNU TLS.
include(FindGnuTLS)
if (NOT GNUTLS_FOUND)
  message(FATAL_ERROR "Could not find GNU TLS.")
endif ()
include_directories("${GNUTLS_INCLUDE_DIR}")

# TLS module.
set(TLS "60-tls")
set(TLS "${TLS}" PARENT_SCOPE)
add_library("${TLS}" SHARED
  # Sources
  "${SRC_DIR}/acceptor.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/internal.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/params.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/acceptor.hh"
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/internal.hh"
  "${INC_DIR}/params.hh"
  "${INC_DIR}/stream.hh"
)
target_link_libraries("${TLS}" ${GNUTLS_LIBRARIES})
set_target_properties("${TLS}" PROPERTIES PREFIX "")
get_property(TLS_MODULE_PATH
  TARGET "${TLS}"
  PROPERTY LOCATION)
set(TLS_MODULE_PATH "${TLS_MODULE_PATH}" PARENT_SCOPE)

# Install rule.
install(TARGETS "${TLS}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
