##
## Copyright 2011-2017 Centreon
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/influxdb/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/influxdb/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")
set(INC_DIR "${INC_DIR}/com/centreon/broker/influxdb")

# INFLUXDB module.
set(INFLUXDB "70-influxdb")
set(INFLUXDB "${INFLUXDB}" PARENT_SCOPE)
add_library("${INFLUXDB}" SHARED
  # Sources.
  "${SRC_DIR}/column.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/influxdb12.cc"
  "${SRC_DIR}/json_printer.cc"
  "${SRC_DIR}/line_protocol_query.cc"
  "${SRC_DIR}/macro_cache.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/column.hh"
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/influxdb.hh"
  "${INC_DIR}/influxdb12.hh"
  "${INC_DIR}/json_printer.hh"
  "${INC_DIR}/line_protocol_query.hh"
  "${INC_DIR}/macro_cache.hh"
  "${INC_DIR}/stream.hh"
)
target_link_libraries("${INFLUXDB}" ${QT_LIBRARIES})
set_target_properties("${INFLUXDB}" PROPERTIES PREFIX "")

# Testing.
if (WITH_TESTING)
  set(TEST_LIBRARIES "${NEB}" "${INFLUXDB}" "roker")

endif ()

# Install rule.
install(TARGETS "${INFLUXDB}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
