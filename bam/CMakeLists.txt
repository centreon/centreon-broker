##
## Copyright 2011-2016 Centreon
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
set(INC_DIR "${PROJECT_SOURCE_DIR}/bam/inc")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/bam/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/bam/test")
include_directories("${INC_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/neb/inc")
include_directories("${PROJECT_SOURCE_DIR}/storage/inc")
set(INC_DIR "${INC_DIR}/com/centreon/broker/bam")

# BAM module.
set(BAM "20-bam")
set(BAM "${BAM}" PARENT_SCOPE)
add_library("${BAM}" SHARED
  # Sources.
  "${SRC_DIR}/availability_builder.cc"
  "${SRC_DIR}/availability_thread.cc"
  "${SRC_DIR}/ba.cc"
  "${SRC_DIR}/ba_status.cc"
  "${SRC_DIR}/ba_svc_mapping.cc"
  "${SRC_DIR}/ba_event.cc"
  "${SRC_DIR}/ba_duration_event.cc"
  "${SRC_DIR}/bool_aggregate.cc"
  "${SRC_DIR}/bool_and.cc"
  "${SRC_DIR}/bool_binary_operator.cc"
  "${SRC_DIR}/bool_call.cc"
  "${SRC_DIR}/bool_constant.cc"
  "${SRC_DIR}/bool_equal.cc"
  "${SRC_DIR}/bool_expression.cc"
  "${SRC_DIR}/bool_less_than.cc"
  "${SRC_DIR}/bool_metric.cc"
  "${SRC_DIR}/bool_more_than.cc"
  "${SRC_DIR}/bool_not.cc"
  "${SRC_DIR}/bool_not_equal.cc"
  "${SRC_DIR}/bool_operation.cc"
  "${SRC_DIR}/bool_or.cc"
  "${SRC_DIR}/bool_service.cc"
  "${SRC_DIR}/bool_value.cc"
  "${SRC_DIR}/bool_xor.cc"
  "${SRC_DIR}/computable.cc"
  "${SRC_DIR}/configuration/applier/ba.cc"
  "${SRC_DIR}/configuration/applier/bool_expression.cc"
  "${SRC_DIR}/configuration/applier/kpi.cc"
  "${SRC_DIR}/configuration/applier/meta_service.cc"
  "${SRC_DIR}/configuration/applier/state.cc"
  "${SRC_DIR}/configuration/ba.cc"
  "${SRC_DIR}/configuration/bool_expression.cc"
  "${SRC_DIR}/configuration/kpi.cc"
  "${SRC_DIR}/configuration/meta_service.cc"
  "${SRC_DIR}/configuration/reader.cc"
  "${SRC_DIR}/configuration/reader_exception.cc"
  "${SRC_DIR}/configuration/reader_v2.cc"
  "${SRC_DIR}/configuration/state.cc"
  "${SRC_DIR}/connector.cc"
  "${SRC_DIR}/dimension_ba_event.cc"
  "${SRC_DIR}/dimension_kpi_event.cc"
  "${SRC_DIR}/dimension_ba_bv_relation_event.cc"
  "${SRC_DIR}/dimension_bv_event.cc"
  "${SRC_DIR}/dimension_timeperiod.cc"
  "${SRC_DIR}/dimension_timeperiod_exception.cc"
  "${SRC_DIR}/dimension_timeperiod_exclusion.cc"
  "${SRC_DIR}/dimension_ba_timeperiod_relation.cc"
  "${SRC_DIR}/dimension_truncate_table_signal.cc"
  "${SRC_DIR}/exp_builder.cc"
  "${SRC_DIR}/exp_parser.cc"
  "${SRC_DIR}/exp_tokenizer.cc"
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/event_cache_visitor.cc"
  "${SRC_DIR}/hst_svc_mapping.cc"
  "${SRC_DIR}/impact_values.cc"
  "${SRC_DIR}/inherited_downtime.cc"
  "${SRC_DIR}/kpi.cc"
  "${SRC_DIR}/kpi_ba.cc"
  "${SRC_DIR}/kpi_boolexp.cc"
  "${SRC_DIR}/kpi_event.cc"
  "${SRC_DIR}/kpi_meta.cc"
  "${SRC_DIR}/kpi_service.cc"
  "${SRC_DIR}/kpi_status.cc"
  "${SRC_DIR}/main.cc"
  "${SRC_DIR}/meta_service.cc"
  "${SRC_DIR}/meta_service_status.cc"
  "${SRC_DIR}/metric_book.cc"
  "${SRC_DIR}/metric_listener.cc"
  "${SRC_DIR}/monitoring_stream.cc"
  "${SRC_DIR}/rebuild.cc"
  "${SRC_DIR}/reporting_stream.cc"
  "${SRC_DIR}/service_book.cc"
  "${SRC_DIR}/service_listener.cc"
  "${SRC_DIR}/timeperiod_map.cc"
  # Headers.
  "${INC_DIR}/availability_builder.hh"
  "${INC_DIR}/availability_thread.hh"
  "${INC_DIR}/ba.hh"
  "${INC_DIR}/ba_status.hh"
  "${INC_DIR}/ba_svc_mapping.hh"
  "${INC_DIR}/ba_event.hh"
  "${INC_DIR}/ba_duration_event.hh"
  "${INC_DIR}/bool_aggregate.hh"
  "${INC_DIR}/bool_and.hh"
  "${INC_DIR}/bool_binary_operator.hh"
  "${INC_DIR}/bool_call.hh"
  "${INC_DIR}/bool_constant.hh"
  "${INC_DIR}/bool_equal.hh"
  "${INC_DIR}/bool_expression.hh"
  "${INC_DIR}/bool_less_than.hh"
  "${INC_DIR}/bool_metric.hh"
  "${INC_DIR}/bool_more_than.hh"
  "${INC_DIR}/bool_not.hh"
  "${INC_DIR}/bool_not_equal.hh"
  "${INC_DIR}/bool_operation.hh"
  "${INC_DIR}/bool_or.hh"
  "${INC_DIR}/bool_service.hh"
  "${INC_DIR}/bool_value.hh"
  "${INC_DIR}/bool_xor.hh"
  "${INC_DIR}/computable.hh"
  "${INC_DIR}/configuration/applier/ba.hh"
  "${INC_DIR}/configuration/applier/bool_expression.hh"
  "${INC_DIR}/configuration/applier/kpi.hh"
  "${INC_DIR}/configuration/applier/meta_service.hh"
  "${INC_DIR}/configuration/applier/state.hh"
  "${INC_DIR}/configuration/ba.hh"
  "${INC_DIR}/configuration/bool_expression.hh"
  "${INC_DIR}/configuration/kpi.hh"
  "${INC_DIR}/configuration/meta_service.hh"
  "${INC_DIR}/configuration/reader.hh"
  "${INC_DIR}/configuration/reader_exception.hh"
  "${INC_DIR}/configuration/reader_v2.hh"
  "${INC_DIR}/configuration/state.hh"
  "${INC_DIR}/connector.hh"
  "${INC_DIR}/dimension_ba_event.hh"
  "${INC_DIR}/dimension_kpi_event.hh"
  "${INC_DIR}/dimension_ba_bv_relation_event.hh"
  "${INC_DIR}/dimension_bv_event.hh"
  "${INC_DIR}/dimension_timeperiod.hh"
  "${INC_DIR}/dimension_timeperiod_exception.hh"
  "${INC_DIR}/dimension_timeperiod_exclusion.hh"
  "${INC_DIR}/dimension_ba_timeperiod_relation.hh"
  "${INC_DIR}/dimension_truncate_table_signal.hh"
  "${INC_DIR}/events.hh"
  "${INC_DIR}/event_cache_visitor.hh"
  "${INC_DIR}/exp_builder.hh"
  "${INC_DIR}/exp_parser.hh"
  "${INC_DIR}/exp_tokenizer.hh"
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/hst_svc_mapping.hh"
  "${INC_DIR}/impact_values.hh"
  "${INC_DIR}/internal.hh"
  "${INC_DIR}/inherited_downtime.hh"
  "${INC_DIR}/kpi.hh"
  "${INC_DIR}/kpi_ba.hh"
  "${INC_DIR}/kpi_boolexp.hh"
  "${INC_DIR}/kpi_event.hh"
  "${INC_DIR}/kpi_meta.hh"
  "${INC_DIR}/kpi_service.hh"
  "${INC_DIR}/kpi_status.hh"
  "${INC_DIR}/meta_service.hh"
  "${INC_DIR}/meta_service_status.hh"
  "${INC_DIR}/metric_book.hh"
  "${INC_DIR}/metric_listener.hh"
  "${INC_DIR}/monitoring_stream.hh"
  "${INC_DIR}/rebuild.hh"
  "${INC_DIR}/reporting_stream.hh"
  "${INC_DIR}/service_book.hh"
  "${INC_DIR}/service_listener.hh"
  "${INC_DIR}/timeperiod_map.hh"
)
set_target_properties("${BAM}" PROPERTIES
    PREFIX ""
    COMPILE_FLAGS "${MYSQL_CFLAGS}")
target_link_libraries("${BAM}" ${QT_LIBRARIES})


# Testing.
if (WITH_TESTING )
    set(
      TESTS_SOURCES
      ${TESTS_SOURCES}
      "${TEST_DIR}/exp_parser/get_postfix.cc"
      "${TEST_DIR}/exp_tokenizer/next.cc"
      "${TEST_DIR}/exp_builder/exp_builder.cc"
      "${TEST_DIR}/configuration/applier-boolexp.cc"
      "${TEST_DIR}/time/check_timeperiod.cc"
      PARENT_SCOPE
    )
    set(
      TESTS_LIBRARIES
      ${TESTS_LIBRARIES}
      "${BAM}"
      PARENT_SCOPE
    )
endif (WITH_TESTING)

# Install rule.
install(TARGETS "${BAM}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
