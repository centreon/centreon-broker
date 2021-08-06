##
## Copyright 2009-2021 Centreon
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## For more information : contact@centreon.com
##

#
# Global settings.
#

# Set necessary settings.
cmake_minimum_required(VERSION 2.8)
project("Centreon Collect" C CXX)
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR "You can build broker with g++ or clang++. CMake will exit.")
endif ()
set(ALLOW_DUPLICATE_EXECUTABLE TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# set -latomic if OS is Raspbian.
if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
  set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -latomic")
endif ()

# Version.
set(COLLECT_MAJOR 21)
set(COLLECT_MINOR 10)
set(COLLECT_PATCH 0)
set(COLLECT_VERSION "${COLLECT_MAJOR}.${COLLECT_MINOR}.${COLLECT_PATCH}")
add_definitions(-DCENTREON_CONNECTOR_VERSION=\"${COLLECT_VERSION}\")
# add_definitions(-DCENTREON_BROKER_VERSION=\"${COLLECT_VERSION}\")


############ CONSTANTS ###########
set(USER_BROKER centreon-broker)
set(USER_ENGINE centreon-engine)
##################################


add_subdirectory(centreon-broker)
add_subdirectory(centreon-clib)
add_subdirectory(centreon-engine)
add_subdirectory(centreon-connector)