##
## Copyright 2013,2015 Centreon
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/dumper/inc/com/centreon/broker/dumper")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/dumper/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/dumper/test")
include_directories("${PROJECT_SOURCE_DIR}/dumper/inc")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")

# Dumper module.
set(DUMPER "05-dumper")
set(DUMPER "${DUMPER}" PARENT_SCOPE)
add_library("${DUMPER}" SHARED
  # Sources.
  "${SRC_DIR}/db_dump.cc"
  "${SRC_DIR}/db_dump_committed.cc"
  "${SRC_DIR}/db_loader.cc"
  "${SRC_DIR}/db_loader_v2.cc"
  "${SRC_DIR}/db_loader_v3.cc"
  "${SRC_DIR}/db_reader.cc"
  "${SRC_DIR}/db_writer.cc"
  "${SRC_DIR}/directory_dump.cc"
  "${SRC_DIR}/directory_dumper.cc"
  "${SRC_DIR}/directory_dump_committed.cc"
  "${SRC_DIR}/dump.cc"
  "${SRC_DIR}/entries/ba.cc"
  "${SRC_DIR}/entries/ba_type.cc"
  "${SRC_DIR}/entries/boolean.cc"
  "${SRC_DIR}/entries/diff.cc"
  "${SRC_DIR}/entries/host.cc"
  "${SRC_DIR}/entries/kpi.cc"
  "${SRC_DIR}/entries/organization.cc"
  "${SRC_DIR}/entries/service.cc"
  "${SRC_DIR}/entries/state.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/fifo_dumper.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/opener.cc"
  "${SRC_DIR}/remove.cc"
  "${SRC_DIR}/stream.cc"
  "${SRC_DIR}/timestamp_cache.cc"
  # Headers.
  "${INC_DIR}/db_dump.hh"
  "${INC_DIR}/db_dump_committed.hh"
  "${INC_DIR}/db_loader.hh"
  "${INC_DIR}/db_loader_v2.hh"
  "${INC_DIR}/db_loader_v3.hh"
  "${INC_DIR}/db_reader.hh"
  "${INC_DIR}/db_writer.hh"
  "${INC_DIR}/directory_dump.hh"
  "${INC_DIR}/directory_dumper.hh"
  "${INC_DIR}/directory_dump_committed.hh"
  "${INC_DIR}/dump.hh"
  "${INC_DIR}/entries/ba.hh"
  "${INC_DIR}/entries/ba_type.hh"
  "${INC_DIR}/entries/boolean.hh"
  "${INC_DIR}/entries/diff.hh"
  "${INC_DIR}/entries/host.hh"
  "${INC_DIR}/entries/kpi.hh"
  "${INC_DIR}/entries/organization.hh"
  "${INC_DIR}/entries/service.hh"
  "${INC_DIR}/entries/state.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/fifo_dumper.hh"
  "${INC_DIR}/internal.hh"
  "${INC_DIR}/opener.hh"
  "${INC_DIR}/remove.hh"
  "${INC_DIR}/stream.hh"
  "${INC_DIR}/timestamp_cache.hh"
)
set_target_properties("${DUMPER}" PROPERTIES
    PREFIX ""
    COMPILE_FLAGS "${MYSQL_CFLAGS}")
target_link_libraries("${DUMPER}" ${QT_LIBRARIES})

if (WITH_TESTING)
  set(TEST_LIBRARIES "${DUMPER}" "roker")

endif ()

# Install rule.
install(TARGETS "${DUMPER}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
