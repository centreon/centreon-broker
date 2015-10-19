/*
** Copyright 2014-2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/bam/events.hh"
#include "com/centreon/broker/namespace.hh"
#include "mapping.hh"

using namespace com::centreon::broker;

// ba_status members mapping.
static mapped_data<bam::ba_status> const ba_status_mapping[] = {
  mapped_data<bam::ba_status>(
    &bam::ba_status::ba_id,
    1,
    "ba_id",
    NULL_ON_ZERO),
  mapped_data<bam::ba_status>(
    &bam::ba_status::in_downtime,
    2,
    "in_downtime"),
  mapped_data<bam::ba_status>(
    &bam::ba_status::last_state_change,
    3,
    "last_state_change"),
  mapped_data<bam::ba_status>(
    &bam::ba_status::level_acknowledgement,
    4,
    "level_acknowledgement"),
  mapped_data<bam::ba_status>(
    &bam::ba_status::level_downtime,
    5,
    "level_downtime"),
  mapped_data<bam::ba_status>(
    &bam::ba_status::level_nominal,
    6,
    "level_nominal"),
  mapped_data<bam::ba_status>(
    &bam::ba_status::state,
    7,
    "state"),
  mapped_data<bam::ba_status>(
    &bam::ba_status::state_changed,
    8,
    "state_changed"),
  mapped_data<bam::ba_status>(
    &bam::ba_status::instance_id,
    9,
    NULL),
  mapped_data<bam::ba_status>()
};

// bool_status members mapping.
static mapped_data<bam::bool_status> const bool_status_mapping[] = {
  mapped_data<bam::bool_status>(
    &bam::bool_status::bool_id,
    1,
    "bool_id",
    NULL_ON_ZERO),
  mapped_data<bam::bool_status>(
    &bam::bool_status::state,
    2,
    "state"),
  mapped_data<bam::bool_status>(
    &bam::bool_status::state,
    3,
    NULL),
  mapped_data<bam::bool_status>()
};

// kpi_status members mapping.
static mapped_data<bam::kpi_status> const kpi_status_mapping[] = {
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::kpi_id,
    1,
    "kpi_id",
    NULL_ON_ZERO),
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::level_acknowledgement_hard,
    2,
    "level_acknowledgement_hard"),
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::level_acknowledgement_soft,
    3,
    "level_acknowledgement_soft"),
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::level_downtime_hard,
    4,
    "level_downtime_hard"),
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::level_downtime_soft,
    5,
    "level_downtime_soft"),
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::level_nominal_hard,
    6,
    "level_nominal_hard"),
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::level_nominal_soft,
    7,
    "level_nominal_soft"),
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::state_hard,
    8,
    "state_hard"),
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::state_soft,
    9,
    "state_soft"),
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::last_state_change,
    10,
    "last_state_change"),
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::last_impact,
    11,
    "last_impact"),
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::instance_id,
    12,
    NULL),
  mapped_data<bam::kpi_status>(
    &bam::kpi_status::valid,
    13,
    "valid"),
  mapped_data<bam::kpi_status>()
};

// meta_service_status members mapping.
static mapped_data<bam::meta_service_status> const meta_service_status_mapping[] = {
  mapped_data<bam::meta_service_status>(
    &bam::meta_service_status::meta_service_id,
    1,
    "meta_service_id",
    NULL_ON_ZERO),
  mapped_data<bam::meta_service_status>(
    &bam::meta_service_status::value,
    2,
    "value"),
  mapped_data<bam::meta_service_status>(
    &bam::meta_service_status::state_changed,
    3,
    NULL),
  mapped_data<bam::meta_service_status>(
    &bam::meta_service_status::instance_id,
    4,
    NULL),
  mapped_data<bam::meta_service_status>(
    &bam::meta_service_status::state,
    5,
    NULL),
  mapped_data<bam::meta_service_status>()
};

// ba_event members mapping.
static mapped_data<bam::ba_event> const ba_event_mapping[] = {
  mapped_data<bam::ba_event>(
    &bam::ba_event::ba_id,
    1,
    "ba_id",
    NULL_ON_ZERO),
  mapped_data<bam::ba_event>(
    &bam::ba_event::first_level,
    2,
    "first_level"),
  mapped_data<bam::ba_event>(
    &bam::ba_event::end_time,
    3,
    "end_time"),
  mapped_data<bam::ba_event>(
    &bam::ba_event::in_downtime,
    4,
    "in_downtime"),
  mapped_data<bam::ba_event>(
    &bam::ba_event::start_time,
    5,
    "start_time"),
  mapped_data<bam::ba_event>(
    &bam::ba_event::status,
    6,
    "status"),
  mapped_data<bam::ba_event>(
    &bam::ba_event::instance_id,
    7,
    NULL),
  mapped_data<bam::ba_event>()
};

// kpi_event members mapping.
static mapped_data<bam::kpi_event> const kpi_event_mapping[] = {
  mapped_data<bam::kpi_event>(
    &bam::kpi_event::kpi_id,
    1,
    "kpi_id",
    NULL_ON_ZERO),
  mapped_data<bam::kpi_event>(
    &bam::kpi_event::end_time,
    2,
    "end_time"),
  mapped_data<bam::kpi_event>(
    &bam::kpi_event::impact_level,
    3,
    "impact_level"),
  mapped_data<bam::kpi_event>(
    &bam::kpi_event::in_downtime,
    4,
    "in_downtime"),
  mapped_data<bam::kpi_event>(
    &bam::kpi_event::output,
    5,
    "first_output"),
  mapped_data<bam::kpi_event>(
    &bam::kpi_event::perfdata,
    6,
    "first_perfdata"),
  mapped_data<bam::kpi_event>(
    &bam::kpi_event::start_time,
    7,
    "start_time"),
  mapped_data<bam::kpi_event>(
    &bam::kpi_event::status,
    8,
    "status"),
  mapped_data<bam::kpi_event>(
    &bam::kpi_event::instance_id,
    9,
    NULL),
  mapped_data<bam::kpi_event>()
};

// ba_duration_event members mapping.
static mapped_data<bam::ba_duration_event> const ba_duration_event_mapping[] = {
  mapped_data<bam::ba_duration_event>(
    &bam::ba_duration_event::ba_id,
    1,
    "ba_id",
    NULL_ON_ZERO),
  mapped_data<bam::ba_duration_event>(
    &bam::ba_duration_event::real_start_time,
    2,
    "real_start_time"),
  mapped_data<bam::ba_duration_event>(
    &bam::ba_duration_event::end_time,
    3,
    "end_time"),
  mapped_data<bam::ba_duration_event>(
    &bam::ba_duration_event::start_time,
    4,
    "start_time"),
  mapped_data<bam::ba_duration_event>(
    &bam::ba_duration_event::duration,
    5,
    "duration"),
  mapped_data<bam::ba_duration_event>(
    &bam::ba_duration_event::sla_duration,
    6,
    "sla_duration"),
  mapped_data<bam::ba_duration_event>(
    &bam::ba_duration_event::timeperiod_id,
    7,
    "timeperiod_id",
    NULL_ON_ZERO),
  mapped_data<bam::ba_duration_event>(
    &bam::ba_duration_event::timeperiod_is_default,
    8,
    "timeperiod_is_default"),
  mapped_data<bam::ba_duration_event>(
    &bam::ba_duration_event::instance_id,
    9,
    NULL),
  mapped_data<bam::ba_duration_event>()
};

// dimension_ba_event members mapping.
static mapped_data<bam::dimension_ba_event> const dimension_ba_event_mapping[] = {
  mapped_data<bam::dimension_ba_event>(
    &bam::dimension_ba_event::ba_id,
    1,
    "ba_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_ba_event>(
    &bam::dimension_ba_event::ba_name,
    2,
    "ba_name"),
  mapped_data<bam::dimension_ba_event>(
    &bam::dimension_ba_event::ba_description,
    3,
    "ba_description"),
  mapped_data<bam::dimension_ba_event>(
    &bam::dimension_ba_event::sla_month_percent_crit,
    4,
    "sla_month_percent_crit"),
  mapped_data<bam::dimension_ba_event>(
    &bam::dimension_ba_event::sla_month_percent_warn,
    5,
    "sla_month_percent_warn"),
  mapped_data<bam::dimension_ba_event>(
    &bam::dimension_ba_event::sla_duration_crit,
    6,
    "sla_month_duration_crit"),
  mapped_data<bam::dimension_ba_event>(
    &bam::dimension_ba_event::sla_duration_warn,
    7,
    "sla_month_duration_warn"),
  mapped_data<bam::dimension_ba_event>(
    &bam::dimension_ba_event::instance_id,
    8,
    NULL),
  mapped_data<bam::dimension_ba_event>()
};

// dimension_kpi_event members mapping.
static mapped_data<bam::dimension_kpi_event> const dimension_kpi_event_mapping[] = {
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::kpi_id,
    1,
    "kpi_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::ba_id,
    2,
    "ba_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::ba_name,
    3,
    "ba_name"),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::host_id,
    4,
    "host_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::host_name,
    5,
    "host_name"),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::service_id,
    6,
    "service_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::service_description,
    7,
    "service_description"),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::kpi_ba_id,
    8,
    "kpi_ba_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::kpi_ba_name,
    9,
    "kpi_ba_name"),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::meta_service_id,
    10,
    "meta_service_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::meta_service_name,
    11,
    "meta_service_name"),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::boolean_id,
    12,
    "boolean_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::boolean_name,
    13,
    "boolean_name"),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::impact_warning,
    14,
    "impact_warning"),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::impact_critical,
    14,
    "impact_critical"),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::impact_unknown,
    14,
    "impact_unknown"),
  mapped_data<bam::dimension_kpi_event>(
    &bam::dimension_kpi_event::instance_id,
    15,
    NULL),
  mapped_data<bam::dimension_kpi_event>()
};

// dimension_ba_bv_relation_event members mapping.
static mapped_data<bam::dimension_ba_bv_relation_event> const dimension_ba_bv_relation_event_mapping[] = {
  mapped_data<bam::dimension_ba_bv_relation_event>(
    &bam::dimension_ba_bv_relation_event::ba_id,
    1,
    "ba_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_ba_bv_relation_event>(
    &bam::dimension_ba_bv_relation_event::bv_id,
    2,
    "bv_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_ba_bv_relation_event>(
    &bam::dimension_ba_bv_relation_event::instance_id,
    3,
    NULL),
  mapped_data<bam::dimension_ba_bv_relation_event>()
};

// dimension_bv_event members mapping.
static mapped_data<bam::dimension_bv_event> const dimension_bv_event_mapping[] = {
  mapped_data<bam::dimension_bv_event>(
    &bam::dimension_bv_event::bv_id,
    1,
    "bv_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_bv_event>(
    &bam::dimension_bv_event::bv_name,
    2,
    "bv_name"),
  mapped_data<bam::dimension_bv_event>(
    &bam::dimension_bv_event::bv_description,
    3,
    "bv_description"),
  mapped_data<bam::dimension_bv_event>(
    &bam::dimension_bv_event::instance_id,
    4,
    NULL),
  mapped_data<bam::dimension_bv_event>()
};

// dimension_truncate_table_signal members mapping.
static mapped_data<bam::dimension_truncate_table_signal> const dimension_truncate_table_signal_mapping[] = {
  mapped_data<bam::dimension_truncate_table_signal>(
    &bam::dimension_truncate_table_signal::update_started,
    1,
    "update_started"),
  mapped_data<bam::dimension_truncate_table_signal>(
    &bam::dimension_truncate_table_signal::instance_id,
    2,
    NULL),
  mapped_data<bam::dimension_truncate_table_signal>()
};

// rebuild members mapping.
static mapped_data<bam::rebuild> const rebuild_mapping[] = {
  mapped_data<bam::rebuild>(
    &bam::rebuild::bas_to_rebuild,
    1,
    "bas_to_rebuild"),
  mapped_data<bam::rebuild>(
    &bam::rebuild::instance_id,
    2,
    NULL),
  mapped_data<bam::rebuild>()
};

// dimension_timeperiod members mapping.
static mapped_data<bam::dimension_timeperiod> const dimension_timeperiod_mapping[] = {
  mapped_data<bam::dimension_timeperiod>(
    &bam::dimension_timeperiod::id,
    1,
    "tp_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_timeperiod>(
    &bam::dimension_timeperiod::name,
    2,
    "name"),
  mapped_data<bam::dimension_timeperiod>(
    &bam::dimension_timeperiod::monday,
    3,
    "monday"),
  mapped_data<bam::dimension_timeperiod>(
    &bam::dimension_timeperiod::tuesday,
    4,
    "tuesday"),
  mapped_data<bam::dimension_timeperiod>(
    &bam::dimension_timeperiod::wednesday,
    5,
    "wednesday"),
  mapped_data<bam::dimension_timeperiod>(
    &bam::dimension_timeperiod::thursday,
    6,
    "thursday"),
  mapped_data<bam::dimension_timeperiod>(
    &bam::dimension_timeperiod::friday,
    7,
    "friday"),
  mapped_data<bam::dimension_timeperiod>(
    &bam::dimension_timeperiod::saturday,
    8,
    "saturday"),
  mapped_data<bam::dimension_timeperiod>(
    &bam::dimension_timeperiod::sunday,
    9,
    "sunday"),
  mapped_data<bam::dimension_timeperiod>(
    &bam::dimension_timeperiod::instance_id,
    10,
    NULL),
  mapped_data<bam::dimension_timeperiod>()
};

// dimension_ba_timeperiod_relation members mapping.
static mapped_data<bam::dimension_ba_timeperiod_relation> const dimension_ba_timeperiod_relation_mapping[] = {
  mapped_data<bam::dimension_ba_timeperiod_relation>(
    &bam::dimension_ba_timeperiod_relation::ba_id,
    1,
    "ba_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_ba_timeperiod_relation>(
    &bam::dimension_ba_timeperiod_relation::timeperiod_id,
    2,
    "timeperiod_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_ba_timeperiod_relation>(
    &bam::dimension_ba_timeperiod_relation::is_default,
    3,
    "is_default"),
  mapped_data<bam::dimension_ba_timeperiod_relation>(
    &bam::dimension_ba_timeperiod_relation::instance_id,
    4,
    NULL),
  mapped_data<bam::dimension_ba_timeperiod_relation>()
};

// dimension_timeperiod_exception members mapping.
static mapped_data<bam::dimension_timeperiod_exception> const dimension_timeperiod_exception_mapping[] = {
  mapped_data<bam::dimension_timeperiod_exception>(
    &bam::dimension_timeperiod_exception::timeperiod_id,
    1,
    "timeperiod_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_timeperiod_exception>(
    &bam::dimension_timeperiod_exception::daterange,
    2,
    "daterange"),
  mapped_data<bam::dimension_timeperiod_exception>(
    &bam::dimension_timeperiod_exception::timerange,
    3,
    "timerange"),
  mapped_data<bam::dimension_timeperiod_exception>(
    &bam::dimension_timeperiod_exception::instance_id,
    4,
    NULL),
  mapped_data<bam::dimension_timeperiod_exception>()
};

// dimension_timeperiod_exclusion members mapping.
static mapped_data<bam::dimension_timeperiod_exclusion> const dimension_timeperiod_exclusion_mapping[] = {
  mapped_data<bam::dimension_timeperiod_exclusion>(
    &bam::dimension_timeperiod_exclusion::timeperiod_id,
    1,
    "timeperiod_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_timeperiod_exclusion>(
    &bam::dimension_timeperiod_exclusion::excluded_timeperiod_id,
    2,
    "excluded_timeperiod_id",
    NULL_ON_ZERO),
  mapped_data<bam::dimension_timeperiod_exclusion>(
    &bam::dimension_timeperiod_exclusion::instance_id,
    3,
    NULL),
  mapped_data<bam::dimension_timeperiod_exclusion>()
};

CCB_BEGIN()
template <> const mapped_data<bam::ba_status>*
  mapped_type<bam::ba_status>::members(ba_status_mapping);
template <> char const*
  mapped_type<bam::ba_status>::table("mod_bam");
template <> const mapped_data<bam::bool_status>*
  mapped_type<bam::bool_status>::members(bool_status_mapping);
template <> char const*
  mapped_type<bam::bool_status>::table("mod_bam_boolean");
template <> const mapped_data<bam::kpi_status>*
  mapped_type<bam::kpi_status>::members(kpi_status_mapping);
template <> char const*
  mapped_type<bam::kpi_status>::table("mod_bam_kpi");
template <> const mapped_data<bam::meta_service_status>*
  mapped_type<bam::meta_service_status>::members(meta_service_status_mapping);
template <> char const*
  mapped_type<bam::meta_service_status>::table("meta_service");
template <> const mapped_data<bam::ba_event>*
  mapped_type<bam::ba_event>::members(ba_event_mapping);
template <> char const*
  mapped_type<bam::ba_event>::table("mod_bam_reporting_ba_events");
template <> const mapped_data<bam::kpi_event>*
  mapped_type<bam::kpi_event>::members(kpi_event_mapping);
template <> char const*
  mapped_type<bam::kpi_event>::table("mod_bam_reporting_kpi_events");
template <> const mapped_data<bam::ba_duration_event>*
  mapped_type<bam::ba_duration_event>::members(ba_duration_event_mapping);
template <> char const*
  mapped_type<bam::ba_duration_event>::table("mod_bam_reporting_ba_events_durations");
template <> const mapped_data<bam::dimension_ba_event>*
  mapped_type<bam::dimension_ba_event>::members(dimension_ba_event_mapping);
template <> char const*
  mapped_type<bam::dimension_ba_event>::table("mod_bam_reporting_ba");
template <> const mapped_data<bam::dimension_kpi_event>*
  mapped_type<bam::dimension_kpi_event>::members(dimension_kpi_event_mapping);
template <> char const*
  mapped_type<bam::dimension_kpi_event>::table("mod_bam_reporting_kpi");
template <> const mapped_data<bam::dimension_ba_bv_relation_event>*
  mapped_type<bam::dimension_ba_bv_relation_event>::members(dimension_ba_bv_relation_event_mapping);
template <> char const*
  mapped_type<bam::dimension_ba_bv_relation_event>::table("mod_bam_reporting_relations_ba_bv");
template <> const mapped_data<bam::dimension_bv_event>*
  mapped_type<bam::dimension_bv_event>::members(dimension_bv_event_mapping);
template <> char const*
  mapped_type<bam::dimension_bv_event>::table("mod_bam_reporting_bv");
template <> const mapped_data<bam::dimension_truncate_table_signal>*
  mapped_type<bam::dimension_truncate_table_signal>::members(dimension_truncate_table_signal_mapping);
template <> char const*
  mapped_type<bam::dimension_truncate_table_signal>::table("mod_bam_reporting_*");
template <> const mapped_data<bam::rebuild>*
  mapped_type<bam::rebuild>::members(rebuild_mapping);
template <> char const*
  mapped_type<bam::rebuild>::table("mod_bam_reporting_*");
template <> const mapped_data<bam::dimension_timeperiod>*
  mapped_type<bam::dimension_timeperiod>::members(dimension_timeperiod_mapping);
template <> char const*
  mapped_type<bam::dimension_timeperiod>::table("mod_bam_reporting_timeperiods");
template <> const mapped_data<bam::dimension_ba_timeperiod_relation>*
  mapped_type<bam::dimension_ba_timeperiod_relation>::members(dimension_ba_timeperiod_relation_mapping);
template <> char const*
  mapped_type<bam::dimension_ba_timeperiod_relation>::table("mod_bam_reporting_relations_ba_tp");
template <> const mapped_data<bam::dimension_timeperiod_exception>*
  mapped_type<bam::dimension_timeperiod_exception>::members(dimension_timeperiod_exception_mapping);
template <> char const*
  mapped_type<bam::dimension_timeperiod_exception>::table("mod_bam_reporting_timeperiods_exceptions");
template <> const mapped_data<bam::dimension_timeperiod_exclusion>*
  mapped_type<bam::dimension_timeperiod_exclusion>::members(dimension_timeperiod_exclusion_mapping);
template <> char const*
  mapped_type<bam::dimension_timeperiod_exclusion>::table("mod_bam_reporting_timeperiods_exclusions");
CCB_END()
