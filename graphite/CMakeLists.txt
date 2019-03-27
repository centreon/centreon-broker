##
## Copyright 2011-2015 Centreon
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

# Global options.
set(INC_DIR "${PROJECT_SOURCE_DIR}/graphite/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/graphite/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/graphite/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")
set(INC_DIR "${INC_DIR}/com/centreon/broker/graphite")

# GRAPHITE module.
set(GRAPHITE "70-graphite")
set(GRAPHITE "${GRAPHITE}" PARENT_SCOPE)
add_library("${GRAPHITE}" SHARED
  # Sources.
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/stream.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/query.cc"
  "${SRC_DIR}/macro_cache.cc"
  # Headers.
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/stream.hh"
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/query.hh"
  "${INC_DIR}/macro_cache.hh"
)
target_link_libraries("${GRAPHITE}" ${QT_LIBRARIES})
set_target_properties("${GRAPHITE}" PROPERTIES PREFIX "")

# Testing.
if (WITH_TESTING)
  set(TEST_LIBRARIES "${NEB}" "${GRAPHITE}" "roker")

endif ()

# Install rule.
install(TARGETS "${GRAPHITE}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
