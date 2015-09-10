##
## Copyright 2013 Centreon
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/bbdo/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/bbdo/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/bbdp/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/bam/inc")
include_directories("${PROJECT_SOURCE_DIR}/correlation/inc")
include_directories("${PROJECT_SOURCE_DIR}/dumper/inc")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")
set(INC_DIR "${INC_DIR}/com/centreon/broker/bbdo")

# Some files must be processed by Qt's moc.
unset(QT_WRAPPED_SOURCES)
qt4_wrap_cpp(QT_WRAPPED_SOURCES
  "${INC_DIR}/acceptor.hh"
)

# BBDO module.
set(BBDO "80-bbdo")
set(BBDO "${BBDO}" PARENT_SCOPE)
add_library("${BBDO}" SHARED
  # Sources.
  "${SRC_DIR}/acceptor.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/input.cc"
  "${SRC_DIR}/internal.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/output.cc"
  "${SRC_DIR}/stream.cc"
  "${SRC_DIR}/version_response.cc"
  "${PROJECT_SOURCE_DIR}/core/src/mapping.cc"
  "${PROJECT_SOURCE_DIR}/ndo/src/temp_mapping.cc"
  ${QT_WRAPPED_SOURCES}
  # Headers.
  "${INC_DIR}/acceptor.hh"
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/input.hh"
  "${INC_DIR}/internal.hh"
  "${INC_DIR}/output.hh"
  "${INC_DIR}/stream.hh"
  "${INC_DIR}/version_response.hh"
)
target_link_libraries("${BBDO}" ${QT_LIBRARIES})
set_target_properties("${BBDO}" PROPERTIES PREFIX "")
get_property(BBDO_MODULE_PATH
  TARGET "${BBDO}"
  PROPERTY LOCATION)
set(BBDO_MODULE_PATH "${BBDO_MODULE_PATH}" PARENT_SCOPE)

# Install rule.
install(TARGETS "${BBDO}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
