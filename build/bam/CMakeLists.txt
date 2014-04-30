##
## Copyright 2011-2014 Merethis
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/bam/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/bam/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/bam/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
set(INC_DIR "${INC_DIR}/com/centreon/broker/bam")

# BAM module.
set(BAM "20-bam")
set(BAM "${BAM}" PARENT_SCOPE)
add_library("${BAM}" SHARED
  # Sources.
  "${SRC_DIR}/ba.cc"
  "${SRC_DIR}/ba_status.cc"
  "${SRC_DIR}/bool_and.cc"
  "${SRC_DIR}/bool_binary_operator.cc"
  "${SRC_DIR}/bool_expression.cc"
  "${SRC_DIR}/bool_not.cc"
  "${SRC_DIR}/bool_or.cc"
  "${SRC_DIR}/bool_parser.cc"
  "${SRC_DIR}/bool_service.cc"
  "${SRC_DIR}/bool_status.cc"
  "${SRC_DIR}/bool_tokenizer.cc"
  "${SRC_DIR}/bool_value.cc"
  "${SRC_DIR}/bool_xor.cc"
  "${SRC_DIR}/computable.cc"
  "${SRC_DIR}/configuration/applier/ba.cc"
  "${SRC_DIR}/configuration/applier/bool_expression.cc"
  "${SRC_DIR}/configuration/applier/kpi.cc"
  "${SRC_DIR}/configuration/applier/state.cc"
  "${SRC_DIR}/configuration/ba.cc"
  "${SRC_DIR}/configuration/bool_expression.cc"
  "${SRC_DIR}/configuration/db.cc"
  "${SRC_DIR}/configuration/kpi.cc"
  "${SRC_DIR}/configuration/reader.cc"
  "${SRC_DIR}/configuration/reader_exception.cc"
  "${SRC_DIR}/configuration/state.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/hst_svc_mapping.cc"
  "${SRC_DIR}/impact_values.cc"
  "${SRC_DIR}/kpi.cc"
  "${SRC_DIR}/kpi_ba.cc"
  "${SRC_DIR}/kpi_service.cc"
  "${SRC_DIR}/kpi_status.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/service_book.cc"
  "${SRC_DIR}/service_listener.cc"
  "${SRC_DIR}/sql_mapping.cc"
  "${SRC_DIR}/stream.cc"
  # Headers.
  "${INC_DIR}/ba.hh"
  "${INC_DIR}/ba_status.hh"
  "${INC_DIR}/bool_and.hh"
  "${INC_DIR}/bool_binary_operator.hh"
  "${INC_DIR}/bool_expression.hh"
  "${INC_DIR}/bool_not.hh"
  "${INC_DIR}/bool_or.hh"
  "${INC_DIR}/bool_parser.hh"
  "${INC_DIR}/bool_service.hh"
  "${INC_DIR}/bool_status.hh"
  "${INC_DIR}/bool_tokenizer.hh"
  "${INC_DIR}/bool_value.hh"
  "${INC_DIR}/bool_xor.hh"
  "${INC_DIR}/computable.hh"
  "${INC_DIR}/configuration/applier/ba.hh"
  "${INC_DIR}/configuration/applier/bool_expression.hh"
  "${INC_DIR}/configuration/applier/kpi.hh"
  "${INC_DIR}/configuration/applier/state.hh"
  "${INC_DIR}/configuration/ba.hh"
  "${INC_DIR}/configuration/bool_expression.hh"
  "${INC_DIR}/configuration/db.hh"
  "${INC_DIR}/configuration/kpi.hh"
  "${INC_DIR}/configuration/reader.hh"
  "${INC_DIR}/configuration/reader_exception.hh"
  "${INC_DIR}/configuration/state.hh"
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/hst_svc_mapping.hh"
  "${INC_DIR}/impact_values.hh"
  "${INC_DIR}/kpi.hh"
  "${INC_DIR}/kpi_ba.hh"
  "${INC_DIR}/kpi_service.hh"
  "${INC_DIR}/kpi_status.hh"
  "${INC_DIR}/service_book.hh"
  "${INC_DIR}/service_listener.hh"
  "${INC_DIR}/sql_mapping.hh"
  "${INC_DIR}/stream.hh"
)
target_link_libraries("${BAM}" ${QT_LIBRARIES})
set_target_properties("${BAM}" PROPERTIES PREFIX "")
get_property(BAM_MODULE_PATH
  TARGET "${BAM}"
  PROPERTY LOCATION)
set(BAM_MODULE_PATH "${BAM_MODULE_PATH}" PARENT_SCOPE)

# Install rule.
install(TARGETS "${BAM}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
