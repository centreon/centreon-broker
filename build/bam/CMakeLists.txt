##
## Copyright 2011-2015 Merethis
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
  "${SRC_DIR}/ba_event.cc"
  "${SRC_DIR}/ba_duration_event.cc"
  "${SRC_DIR}/bool_and.cc"
  "${SRC_DIR}/bool_binary_operator.cc"
  "${SRC_DIR}/bool_expression.cc"
  "${SRC_DIR}/bool_not.cc"
  "${SRC_DIR}/bool_or.cc"
  "${SRC_DIR}/bool_parser.cc"
  "${SRC_DIR}/bool_service.cc"
  "${SRC_DIR}/bool_tokenizer.cc"
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
  "${SRC_DIR}/factory.cc"
  "${SRC_DIR}/event_cache_visitor.cc"
  "${SRC_DIR}/hst_svc_mapping.cc"
  "${SRC_DIR}/impact_values.cc"
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
  "${INC_DIR}/ba_event.hh"
  "${INC_DIR}/ba_duration_event.hh"
  "${INC_DIR}/bool_and.hh"
  "${INC_DIR}/bool_binary_operator.hh"
  "${INC_DIR}/bool_expression.hh"
  "${INC_DIR}/bool_not.hh"
  "${INC_DIR}/bool_or.hh"
  "${INC_DIR}/bool_parser.hh"
  "${INC_DIR}/bool_service.hh"
  "${INC_DIR}/bool_tokenizer.hh"
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
  "${INC_DIR}/factory.hh"
  "${INC_DIR}/hst_svc_mapping.hh"
  "${INC_DIR}/impact_values.hh"
  "${INC_DIR}/internal.hh"
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
target_link_libraries("${BAM}" ${QT_LIBRARIES})
set_target_properties("${BAM}" PROPERTIES PREFIX "")
get_property(BAM_MODULE_PATH
  TARGET "${BAM}"
  PROPERTY LOCATION)
set(BAM_MODULE_PATH "${BAM_MODULE_PATH}" PARENT_SCOPE)

# Testing.
if (WITH_TESTING)
  set(TEST_LIBRARIES "${NEB}" "${BAM}" "roker")

  # ba tests.
  #   KPI change at recompute.
  set(TEST_NAME "bam_ba_kpi_change_at_recompute")
  add_executable("${TEST_NAME}"
    "${TEST_DIR}/ba/kpi_change_at_recompute.cc")
  target_link_libraries("${TEST_NAME}" ${TEST_LIBRARIES})
  add_test("${TEST_NAME}" "${TEST_NAME}")

  # time tests.
  set(CHECK_TIMEPERIOD "check_timeperiod")
  add_executable("${CHECK_TIMEPERIOD}"
    "${TEST_DIR}/time/check_timeperiod.cc")
  target_link_libraries("${CHECK_TIMEPERIOD}" ${TEST_LIBRARIES})
  #   Week days tests.
  add_test("bam_time_week_day_after_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/week_day/after_period.conf")
  add_test("bam_time_week_day_before_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/week_day/before_period.conf")
  add_test("bam_time_week_day_into_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/week_day/into_period.conf")
  add_test("bam_time_week_day_into_period_for_next_week"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/week_day/into_period_for_next_week.conf")
  add_test("bam_time_week_day_into_period_with_exclude_after"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/week_day/into_period_with_exclude_after.conf")
  add_test("bam_time_week_day_into_period_with_exclude_before"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/week_day/into_period_with_exclude_before.conf")
  add_test("bam_time_week_day_into_period_with_exclude_into"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/week_day/into_period_with_exclude_into.conf")
  #   Exceptions tests.
  #     Calendar date.
  add_test("bam_time_exception_calendar_date_after_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/after_period.conf")
  add_test("bam_time_exception_calendar_date_before_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/before_period.conf")
  add_test("bam_time_exception_calendar_date_into_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/into_period.conf")
  add_test("bam_time_exception_calendar_date_into_period_with_exclude_after"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/into_period_with_exclude_after.conf")
  add_test("bam_time_exception_calendar_date_into_period_with_exclude_before"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/into_period_with_exclude_before.conf")
  add_test("bam_time_exception_calendar_date_into_period_with_exclude_into"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/into_period_with_exclude_into.conf")
  add_test("bam_time_exception_calendar_date_skip_in_after_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/skip_in_after_period.conf")
  add_test("bam_time_exception_calendar_date_skip_in_before_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/skip_in_before_period.conf")
  add_test("bam_time_exception_calendar_date_skip_in_into_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/skip_in_into_period.conf")
  add_test("bam_time_exception_calendar_date_skip_in_into_period_with_exclude_after"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/skip_in_into_period_with_exclude_after.conf")
  add_test("bam_time_exception_calendar_date_skip_in_into_period_with_exclude_before"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/skip_in_into_period_with_exclude_before.conf")
  add_test("bam_time_exception_calendar_date_skip_in_into_period_with_exclude_into"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/skip_in_into_period_with_exclude_into.conf")
  add_test("bam_time_exception_calendar_date_start_end_skip_in_after_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/start_end_skip_in_after_period.conf")
  add_test("bam_time_exception_calendar_date_start_end_skip_in_before_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/start_end_skip_in_before_period.conf")
  add_test("bam_time_exception_calendar_date_start_end_skip_in_into_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/start_end_skip_in_into_period.conf")
  add_test("bam_time_exception_calendar_date_start_end_skip_in_into_period_with_exclude_after"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/start_end_skip_in_into_period_with_exclude_after.conf")
  add_test("bam_time_exception_calendar_date_start_end_skip_in_into_period_with_exclude_before"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/start_end_skip_in_into_period_with_exclude_before.conf")
  add_test("bam_time_exception_calendar_date_start_end_skip_in_into_period_with_exclude_into"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/start_end_skip_in_into_period_with_exclude_into.conf")
  add_test("bam_time_exception_calendar_date_start_end_skip_out_after_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/start_end_skip_out_after_period.conf")
  add_test("bam_time_exception_calendar_date_start_end_skip_out_after_period_with_exclude_after"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/start_end_skip_out_after_period_with_exclude_after.conf")
  add_test("bam_time_exception_calendar_date_start_end_skip_out_after_period_with_exclude_before"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/start_end_skip_out_after_period_with_exclude_before.conf")
  add_test("bam_time_exception_calendar_date_start_end_skip_out_after_period_with_exclude_into"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/start_end_skip_out_after_period_with_exclude_into.conf")
  add_test("bam_time_exception_calendar_date_start_end_skip_out_before_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/start_end_skip_out_before_period.conf")
  add_test("bam_time_exception_calendar_date_start_end_skip_out_into_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/calendar_date/start_end_skip_out_into_period.conf")
  #     Month date.
  add_test("bam_time_exception_month_date_day_after_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_date/day_after_period.conf")
  add_test("bam_time_exception_month_date_day_before_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_date/day_before_period.conf")
  add_test("bam_time_exception_month_date_day_into_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_date/day_into_period.conf")
  add_test("bam_time_exception_month_date_day_into_period_with_exclude_after"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_date/day_into_period_with_exclude_after.conf")
  add_test("bam_time_exception_month_date_day_into_period_with_exclude_before"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_date/day_into_period_with_exclude_before.conf")
  add_test("bam_time_exception_month_date_day_into_period_with_exclude_into"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_date/day_into_period_with_exclude_into.conf")
  #     Month day.
  add_test("bam_time_exception_month_day_after_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_day/after_period.conf")
  add_test("bam_time_exception_month_day_before_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_day/before_period.conf")
  add_test("bam_time_exception_month_day_into_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_day/into_period.conf")
  add_test("bam_time_exception_month_day_into_period_with_exclude_after"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_day/into_period_with_exclude_after.conf")
  add_test("bam_time_exception_month_day_into_period_with_exclude_before"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_day/into_period_with_exclude_before.conf")
  add_test("bam_time_exception_month_day_into_period_with_exclude_into"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_day/into_period_with_exclude_into.conf")
  #     Month week day.
  add_test("bam_time_exception_month_week_day_4_after_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_week_day/4_after_period.conf")
  add_test("bam_time_exception_month_week_day_4_before_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_week_day/4_before_period.conf")
  add_test("bam_time_exception_month_week_day_4_into_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_week_day/4_into_period.conf")
  add_test("bam_time_exception_month_week_day_4_into_period_with_exclude_after"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_week_day/4_into_period_with_exclude_after.conf")
  add_test("bam_time_exception_month_week_day_4_into_period_with_exclude_before"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_week_day/4_into_period_with_exclude_before.conf")
  add_test("bam_time_exception_month_week_day_4_into_period_with_exclude_into"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_week_day/4_into_period_with_exclude_into.conf")
  add_test("bam_time_exception_month_week_day_last_after_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_week_day/last_after_period.conf")
  add_test("bam_time_exception_month_week_day_last_before_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_week_day/last_before_period.conf")
  add_test("bam_time_exception_month_week_day_last_into_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_week_day/last_into_period.conf")
  add_test("bam_time_exception_month_week_day_last_into_period_with_exclude_after"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_week_day/last_into_period_with_exclude_after.conf")
  add_test("bam_time_exception_month_week_day_last_into_period_with_exclude_before"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_week_day/last_into_period_with_exclude_before.conf")
  add_test("bam_time_exception_month_week_day_last_into_period_with_exclude_into"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/month_week_day/last_into_period_with_exclude_into.conf")
  #     Week day.
  add_test("bam_time_exception_week_day_4_after_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/week_day/4_after_period.conf")
  add_test("bam_time_exception_week_day_4_before_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/week_day/4_before_period.conf")
  add_test("bam_time_exception_week_day_4_into_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/week_day/4_into_period.conf")
  add_test("bam_time_exception_week_day_4_into_period_with_exclude_after"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/week_day/4_into_period_with_exclude_after.conf")
  add_test("bam_time_exception_week_day_4_into_period_with_exclude_before"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/week_day/4_into_period_with_exclude_before.conf")
  add_test("bam_time_exception_week_day_4_into_period_with_exclude_into"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/week_day/4_into_period_with_exclude_into.conf")
  add_test("bam_time_exception_week_day_last_after_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/week_day/last_after_period.conf")
  add_test("bam_time_exception_week_day_last_before_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/week_day/last_before_period.conf")
  add_test("bam_time_exception_week_day_last_into_period"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/week_day/last_into_period.conf")
  add_test("bam_time_exception_week_day_last_into_period_with_exclude_after"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/week_day/last_into_period_with_exclude_after.conf")
  add_test("bam_time_exception_week_day_last_into_period_with_exclude_before"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/week_day/last_into_period_with_exclude_before.conf")
  add_test("bam_time_exception_week_day_last_into_period_with_exclude_into"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exception/week_day/last_into_period_with_exclude_into.conf")
  #   Exclusions tests.
  add_test("bam_time_exclusion_nested_after"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exclusion/nested_after.conf")
  add_test("bam_time_exclusion_nested_before"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exclusion/nested_before.conf")
  add_test("bam_time_exclusion_nested_into"
    "${CHECK_TIMEPERIOD}" "${TEST_DIR}/time/cfg/exclusion/nested_into.conf")

endif ()

# Install rule.
install(TARGETS "${BAM}"
  LIBRARY DESTINATION "${PREFIX_MODULES}"
)
