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

# Find Xerces-C++ includes and library.
#
#  XERCESC_INCLUDE_DIR - where to find Xerces-C++ headers.
#  XERCESC_LIBRARIES   - list of libraries when using Xerces-C++.

# Check for Xerces-C++ headers.
set(CHECK_HEADERS "xercesc/util/PlatformUtils.hpp")
set(CHECK_SUFFIXES "")
if (XERCESC_INCLUDE_DIR)
  find_path(XERCESC_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATHS ${XERCESC_INCLUDE_DIR}
    PATH_SUFFIXES ${CHECK_SUFFIXES}
    NO_DEFAULT_PATH)
else ()
  find_path(XERCESC_INCLUDE_DIR
    NAMES ${CHECK_HEADERS}
    PATH_SUFFIXES ${CHECK_SUFFIXES})
endif ()
if (XERCESC_INCLUDE_DIR)
  message(STATUS "Found Xerces-C++ headers in ${XERCESC_INCLUDE_DIR}")
else ()
  message(FATAL_ERROR "Could not find Xerces-C++ headers")
endif ()

# Check for Xerces-C++ library.
set(CHECK_LIBRARIES xerces-c)
if (XERCESC_LIBRARY_DIR)
  find_library(XERCESC_LIBRARIES
    NAMES ${CHECK_LIBRARIES}
    PATHS ${XERCESC_LIBRARY_DIR}
    NO_DEFAULT_PATH)
else ()
  find_library(XERCESC_LIBRARIES
    NAMES ${CHECK_LIBRARIES})
endif ()
if (XERCESC_LIBRARIES)
  message(STATUS "Found Xerces-C++ library ${XERCESC_LIBRARIES}")
else ()
  message(FATAL_ERROR "Could not find Xerces-C++ library")
endif ()
