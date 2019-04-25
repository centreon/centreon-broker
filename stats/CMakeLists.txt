##
## Copyright 2012-2013 Centreon
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/stats/inc/com/centreon/broker/stats")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/stats/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/stats/test")
include_directories("${PROJECT_SOURCE_DIR}/stats/inc")
include_directories("${PROJECT_SOURCE_DIR}/dumper/inc")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")

# Some files must be processed by Qt's moc.
unset(QT_WRAPPED_SOURCES)
qt4_wrap_cpp(QT_WRAPPED_SOURCES
  "${INC_DIR}/generator.hh"
  "${INC_DIR}/worker.hh")

# Stats module.
set(STATS "15-stats")
set(STATS "${STATS}" PARENT_SCOPE)
add_library("${STATS}" SHARED
  ${QT_WRAPPED_SOURCES}
  # Sources.
  "${SRC_DIR}/builder.cc"
  "${SRC_DIR}/config.cc"
  "${SRC_DIR}/generator.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/metric.cc"
  "${SRC_DIR}/parser.cc"
  "${SRC_DIR}/json_serializer.cc"
  "${SRC_DIR}/worker.cc"
  # Headers.
  "${INC_DIR}/builder.hh"
  "${INC_DIR}/config.hh"
  "${INC_DIR}/generator.hh"
  "${INC_DIR}/metric.hh"
  "${INC_DIR}/parser.hh"
  "${INC_DIR}/serializer.hh"
  "${INC_DIR}/json_serializer.hh"
  "${INC_DIR}/worker.hh"
)
set_target_properties("${STATS}" PROPERTIES
    PREFIX ""
    COMPILE_FLAGS "${MYSQL_CFLAGS}")
target_link_libraries("${STATS}" ${QT_LIBRARIES})

# Install rule.
install(TARGETS "${STATS}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
