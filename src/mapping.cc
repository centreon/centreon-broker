/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
**
** For more information: contact@centreon.com
*/

#include "events/events.hh"
#include "mapping.hh"
#include "nagios/protoapi.h"

using namespace events;

// acknowledgement members mapping.
static mapped_data<acknowledgement> const acknowledgement_mapping[] = {
  mapped_data<acknowledgement>(
    &acknowledgement::acknowledgement_type,
    NDO_DATA_ACKNOWLEDGEMENTTYPE,
    "type"),
  mapped_data<acknowledgement>(
    &acknowledgement::author,
    NDO_DATA_AUTHORNAME,
    "author"),
  mapped_data<acknowledgement>(
    &acknowledgement::comment,
    NDO_DATA_COMMENT,
    "comment"),
  mapped_data<acknowledgement>(
    &acknowledgement::entry_time,
    NDO_DATA_TIMESTAMP,
    "entry_time"),
  mapped_data<acknowledgement>(
    &acknowledgement::host_id,
    NDO_DATA_HOSTNAME,
    "host_id",
    true),
  mapped_data<acknowledgement>(
    &acknowledgement::instance_id,
    NDO_DATA_INSTANCE,
    "instance_id",
    true),
  mapped_data<acknowledgement>(
    &acknowledgement::is_sticky,
    NDO_DATA_STICKY,
    "sticky"),
  mapped_data<acknowledgement>(
    &acknowledgement::notify_contacts,
    NDO_DATA_NOTIFYCONTACTS,
    "notify_contacts"),
  mapped_data<acknowledgement>(
    &acknowledgement::persistent_comment,
    NDO_DATA_PERSISTENT,
    "persistent_comment"),
  mapped_data<acknowledgement>(
    &acknowledgement::service_id,
    NDO_DATA_SERVICEDESCRIPTION,
    "service_id",
    true),
  mapped_data<acknowledgement>(
    &acknowledgement::state,
    NDO_DATA_STATE,
    "state"),
  mapped_data<acknowledgement>()
};

// Comment members mapping.
static mapped_data<comment> const comment_mapping[] = {
  mapped_data<comment>(
    &comment::author,
    NDO_DATA_AUTHORNAME,
    "author"),
  mapped_data<comment>(
    &comment::deletion_time,
    NDO_DATA_ENDTIME,
    "deletion_time"),
  mapped_data<comment>(
    &comment::entry_time,
    NDO_DATA_ENTRYTIME,
    "entry_time"),
  mapped_data<comment>(
    &comment::entry_type,
    NDO_DATA_ENTRYTYPE,
    "entry_type"),
  mapped_data<comment>(
    &comment::expire_time,
    NDO_DATA_EXPIRATIONTIME,
    "expire_time"),
  mapped_data<comment>(
    &comment::expires,
    NDO_DATA_EXPIRES,
    "expires"),
  mapped_data<comment>(
    &comment::host_id,
    NDO_DATA_HOSTNAME,
    "host_id",
    true),
  mapped_data<comment>(
    &comment::instance_id,
    NDO_DATA_INSTANCE,
    "instance_id",
    true),
  mapped_data<comment>(
    &comment::internal_id,
    NDO_DATA_COMMENTID,
    "internal_id"),
  mapped_data<comment>(
    &comment::persistent,
    NDO_DATA_PERSISTENT,
    "persistent"),
  mapped_data<comment>(
    &comment::service_id,
    NDO_DATA_SERVICEDESCRIPTION,
    "service_id",
    true),
  mapped_data<comment>(
    &comment::source,
    NDO_DATA_SOURCE,
    "source"),
  mapped_data<comment>(
    &comment::type,
    NDO_DATA_COMMENTTYPE,
    "type"),
  mapped_data<comment>(
    &comment::data,
    NDO_DATA_COMMENT,
    "data"),
  mapped_data<comment>()
};

// Custom variables members mapping.
static mapped_data<custom_variable> const custom_variable_mapping[] = {
  mapped_data<custom_variable>(
    &custom_variable::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<custom_variable>(
    &custom_variable::modified,
    NDO_DATA_HASBEENMODIFIED,
    "modified"),
  mapped_data<custom_variable>(
    &custom_variable::name,
    NDO_DATA_CONFIGFILENAME,
    "name"),
  mapped_data<custom_variable>(
    &custom_variable::service_id,
    NDO_DATA_SERVICE,
    "service_id",
    true),
  mapped_data<custom_variable>(
    &custom_variable::type,
    NDO_DATA_TYPE,
    "type"),
  mapped_data<custom_variable>(
    &custom_variable::update_time,
    NDO_DATA_ENTRYTIME,
    "update_time"),
  mapped_data<custom_variable>(
    &custom_variable::value,
    NDO_DATA_ACTIVESERVICECHECKSENABLED,
    "value"),
  mapped_data<custom_variable>(
    &custom_variable::value,
    NDO_DATA_ACTIVEHOSTCHECKSENABLED,
    "default_value"),
  mapped_data<custom_variable>()
};

// Custom variable status members mapping.
static mapped_data<custom_variable_status> const custom_variable_status_mapping[] = {
  mapped_data<custom_variable_status>(
    &custom_variable_status::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<custom_variable_status>(
    &custom_variable_status::modified,
    NDO_DATA_HASBEENMODIFIED,
    "modified"),
  mapped_data<custom_variable_status>(
    &custom_variable_status::name,
    NDO_DATA_CONFIGFILENAME,
    "name"),
  mapped_data<custom_variable_status>(
    &custom_variable_status::service_id,
    NDO_DATA_SERVICE,
    "service_id",
    true),
  mapped_data<custom_variable_status>(
    &custom_variable_status::update_time,
    NDO_DATA_ENTRYTIME,
    "update_time"),
  mapped_data<custom_variable_status>(
    &custom_variable_status::value,
    NDO_DATA_ACTIVESERVICECHECKSENABLED,
    "value"),
  mapped_data<custom_variable_status>()
};

// downtime members mapping.
static mapped_data<downtime> const downtime_mapping[] = {
  mapped_data<downtime>(
    &downtime::author,
    NDO_DATA_AUTHORNAME,
    "author"),
  mapped_data<downtime>(
    &downtime::downtime_type,
    NDO_DATA_DOWNTIMETYPE,
    "type"),
  mapped_data<downtime>(
    &downtime::duration,
    NDO_DATA_DURATION,
    "duration"),
  mapped_data<downtime>(
    &downtime::end_time,
    NDO_DATA_ENDTIME,
    "end_time"),
  mapped_data<downtime>(
    &downtime::entry_time,
    NDO_DATA_ENTRYTIME,
    "entry_time"),
  mapped_data<downtime>(
    &downtime::fixed,
    NDO_DATA_FIXED,
    "fixed"),
  mapped_data<downtime>(
    &downtime::host_id,
    NDO_DATA_HOSTNAME,
    "host_id",
    true),
  mapped_data<downtime>(
    &downtime::instance_id,
    NDO_DATA_INSTANCE,
    "instance_id",
    true),
  mapped_data<downtime>(
    &downtime::internal_id,
    NDO_DATA_DOWNTIMEID,
    "internal_id"),
  mapped_data<downtime>(
    &downtime::service_id,
    NDO_DATA_SERVICEDESCRIPTION,
    "service_id",
    true),
  mapped_data<downtime>(
    &downtime::start_time,
    NDO_DATA_STARTTIME,
    "start_time"),
  mapped_data<downtime>(
    &downtime::triggered_by,
    NDO_DATA_TRIGGEREDBY,
    "triggered_by"),
  mapped_data<downtime>(
    &downtime::was_cancelled,
    NDO_DATA_X3D,
    "cancelled"),
  mapped_data<downtime>(
    &downtime::was_started,
    NDO_DATA_Y3D,
    "started"),
  mapped_data<downtime>(
    &downtime::comment,
    NDO_DATA_COMMENT,
    "comment"),
  mapped_data<downtime>()
};

// event_handler members mapping.
static mapped_data<event_handler> const event_handler_mapping[] = {
  mapped_data<event_handler>(
    &event_handler::early_timeout,
    NDO_DATA_EARLYTIMEOUT,
    "early_timeout"),
  mapped_data<event_handler>(
    &event_handler::end_time,
    NDO_DATA_ENDTIME,
    "end_time"),
  mapped_data<event_handler>(
    &event_handler::execution_time,
    NDO_DATA_EXECUTIONTIME,
    "execution_time"),
  mapped_data<event_handler>(
    &event_handler::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<event_handler>(
    &event_handler::return_code,
    NDO_DATA_RETURNCODE,
    "return_code"),
  mapped_data<event_handler>(
    &event_handler::service_id,
    NDO_DATA_SERVICE,
    "service_id",
    true),
  mapped_data<event_handler>(
    &event_handler::start_time,
    NDO_DATA_STARTTIME,
    "start_time"),
  mapped_data<event_handler>(
    &event_handler::state,
    NDO_DATA_STATE,
    "state"),
  mapped_data<event_handler>(
    &event_handler::state_type,
    NDO_DATA_STATETYPE,
    "state_type"),
  mapped_data<event_handler>(
    &event_handler::timeout,
    NDO_DATA_TIMEOUT,
    "timeout"),
  mapped_data<event_handler>(
    &event_handler::type,
    NDO_DATA_TYPE,
    "type"),
  mapped_data<event_handler>(
    &event_handler::command_args,
    NDO_DATA_COMMANDARGS,
    "command_args"),
  mapped_data<event_handler>(
    &event_handler::command_line,
    NDO_DATA_COMMANDLINE,
    "command_line"),
  mapped_data<event_handler>(
    &event_handler::output,
    NDO_DATA_OUTPUT,
    "output"),
  mapped_data<event_handler>()
};

// Flapping status mapping.
static mapped_data<flapping_status> const flapping_status_mapping[] = {
  mapped_data<flapping_status>(
    &flapping_status::comment_time,
    NDO_DATA_COMMENTTIME,
    "comment_time"),
  mapped_data<flapping_status>(
    &flapping_status::event_time,
    NDO_DATA_ENTRYTIME,
    "event_time"),
  mapped_data<flapping_status>(
    &flapping_status::event_type,
    NDO_DATA_ENTRYTYPE,
    "event_type"),
  mapped_data<flapping_status>(
    &flapping_status::high_threshold,
    NDO_DATA_HIGHTHRESHOLD,
    "high_threshold"),
  mapped_data<flapping_status>(
    &flapping_status::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<flapping_status>(
    &flapping_status::internal_comment_id,
    NDO_DATA_COMMENTID,
    "internal_comment_id"),
  mapped_data<flapping_status>(
    &flapping_status::low_threshold,
    NDO_DATA_LOWTHRESHOLD,
    "low_threshold"),
  mapped_data<flapping_status>(
    &flapping_status::percent_state_change,
    NDO_DATA_PERCENTSTATECHANGE,
    "percent_state_change"),
  mapped_data<flapping_status>(
    &flapping_status::reason_type,
    NDO_DATA_NOTIFICATIONREASON,
    "reason_type"),
  mapped_data<flapping_status>(
    &flapping_status::service_id,
    NDO_DATA_SERVICE,
    "service_id",
    true),
  mapped_data<flapping_status>(
    &flapping_status::type,
    NDO_DATA_TYPE,
    "type"),
  mapped_data<flapping_status>()
};

// host members mapping.
static mapped_data<host> const host_mapping[] = {
  mapped_data<host>(
    &host::acknowledgement_type,
    NDO_DATA_ACKNOWLEDGEMENTTYPE,
    "acknowledgement_type"),
  mapped_data<host>(
    &host::action_url,
    NDO_DATA_ACTIONURL,
    "action_url"),
  mapped_data<host>(
    &host::active_checks_enabled,
    NDO_DATA_ACTIVEHOSTCHECKSENABLED,
    "active_checks"),
  mapped_data<host>(
    &host::address,
    NDO_DATA_HOSTADDRESS,
    "address"),
  mapped_data<host>(
    &host::alias,
    NDO_DATA_HOSTALIAS,
    "alias"),
  mapped_data<host>(
    &host::check_freshness,
    NDO_DATA_HOSTFRESHNESSCHECKSENABLED,
    "check_freshness"),
  mapped_data<host>(
    &host::check_interval,
    NDO_DATA_NORMALCHECKINTERVAL,
    "check_interval"),
  mapped_data<host>(
    &host::check_period,
    NDO_DATA_HOSTCHECKPERIOD,
    "check_period"),
  mapped_data<host>(
    &host::check_type,
    NDO_DATA_CHECKTYPE,
    "check_type"),
  mapped_data<host>(
    &host::current_check_attempt,
    NDO_DATA_CURRENTCHECKATTEMPT,
    "check_attempt"),
  mapped_data<host>(
    &host::current_notification_number,
    NDO_DATA_CURRENTNOTIFICATIONNUMBER,
    "notification_number"),
  mapped_data<host>(
    &host::current_state,
    NDO_DATA_CURRENTSTATE,
    "state"),
  mapped_data<host>(
    &host::default_active_checks_enabled,
    0,
    "default_active_checks"),
  mapped_data<host>(
    &host::default_event_handler_enabled,
    0,
    "default_event_handler_enabled"),
  mapped_data<host>(
    &host::default_failure_prediction,
    0,
    "default_failure_prediction"),
  mapped_data<host>(
    &host::default_flap_detection_enabled,
    0,
    "default_flap_detection"),
  mapped_data<host>(
    &host::default_notifications_enabled,
    0,
    "default_notify"),
  mapped_data<host>(
    &host::default_passive_checks_enabled,
    0,
    "default_passive_checks"),
  mapped_data<host>(
    &host::default_process_perf_data,
    0,
    "default_process_perfdata"),
  mapped_data<host>(
    &host::display_name,
    NDO_DATA_DISPLAYNAME,
    "display_name"),
  mapped_data<host>(
    &host::enabled,
    NDO_DATA_X3D,
    "enabled"),
  mapped_data<host>(
    &host::event_handler,
    NDO_DATA_EVENTHANDLER,
    "event_handler"),
  mapped_data<host>(
    &host::event_handler_enabled,
    NDO_DATA_EVENTHANDLERENABLED,
    "event_handler_enabled"),
  mapped_data<host>(
    &host::execution_time,
    NDO_DATA_EXECUTIONTIME,
    "execution_time"),
  mapped_data<host>(
    &host::failure_prediction_enabled,
    NDO_DATA_FAILUREPREDICTIONENABLED,
    "failure_prediction"),
  mapped_data<host>(
    &host::first_notification_delay,
    NDO_DATA_FIRSTNOTIFICATIONDELAY,
    "first_notification_delay"),
  mapped_data<host>(
    &host::flap_detection_enabled,
    NDO_DATA_FLAPDETECTIONENABLED,
    "flap_detection"),
  mapped_data<host>(
    &host::flap_detection_on_down,
    NDO_DATA_FLAPDETECTIONONDOWN,
    "flap_detection_on_down"),
  mapped_data<host>(
    &host::flap_detection_on_unreachable,
    NDO_DATA_FLAPDETECTIONONUNREACHABLE,
    "flap_detection_on_unreachable"),
  mapped_data<host>(
    &host::flap_detection_on_up,
    NDO_DATA_FLAPDETECTIONONUP,
    "flap_detection_on_up"),
  mapped_data<host>(
    &host::freshness_threshold,
    NDO_DATA_HOSTFRESHNESSTHRESHOLD,
    "freshness_threshold"),
  mapped_data<host>(
    &host::has_been_checked,
    NDO_DATA_HASBEENCHECKED,
    "checked"),
  mapped_data<host>(
    &host::high_flap_threshold,
    NDO_DATA_HIGHHOSTFLAPTHRESHOLD,
    "high_flap_threshold"),
  mapped_data<host>(
    &host::host_name,
    NDO_DATA_HOSTNAME,
    "name"),
  mapped_data<host>(
    &host::icon_image,
    NDO_DATA_ICONIMAGE,
    "icon_image"),
  mapped_data<host>(
    &host::icon_image_alt,
    NDO_DATA_ICONIMAGEALT,
    "icon_image_alt"),
  mapped_data<host>(
    &host::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<host>(
    &host::instance_id,
    NDO_DATA_INSTANCE,
    "instance_id",
    true),
  mapped_data<host>(
    &host::is_flapping,
    NDO_DATA_ISFLAPPING,
    "flapping"),
  mapped_data<host>(
    &host::last_check,
    NDO_DATA_LASTHOSTCHECK,
    "last_check"),
  mapped_data<host>(
    &host::last_hard_state,
    NDO_DATA_LASTHARDSTATE,
    "last_hard_state"),
  mapped_data<host>(
    &host::last_hard_state_change,
    NDO_DATA_LASTHARDSTATECHANGE,
    "last_hard_state_change"),
  mapped_data<host>(
    &host::last_notification,
    NDO_DATA_LASTHOSTNOTIFICATION,
    "last_notification"),
  mapped_data<host>(
    &host::last_state_change,
    NDO_DATA_LASTSTATECHANGE,
    "last_state_change"),
  mapped_data<host>(
    &host::last_time_down,
    NDO_DATA_LASTTIMEDOWN,
    "last_time_down"),
  mapped_data<host>(
    &host::last_time_unreachable,
    NDO_DATA_LASTTIMEUNREACHABLE,
    "last_time_unreachable"),
  mapped_data<host>(
    &host::last_time_up,
    NDO_DATA_LASTTIMEUP,
    "last_time_up"),
  mapped_data<host>(
    &host::last_update,
    0, // XXX : should find macro
    "last_update"),
  mapped_data<host>(
    &host::latency,
    NDO_DATA_LATENCY,
    "latency"),
  mapped_data<host>(
    &host::low_flap_threshold,
    NDO_DATA_LOWHOSTFLAPTHRESHOLD,
    "low_flap_threshold"),
  mapped_data<host>(
    &host::max_check_attempts,
    NDO_DATA_MAXCHECKATTEMPTS,
    "max_check_attempts"),
  mapped_data<host>(
    &host::modified_attributes,
    NDO_DATA_MODIFIEDHOSTATTRIBUTES,
    "modified_attributes"),
  mapped_data<host>(
    &host::next_check,
    NDO_DATA_NEXTHOSTCHECK,
    "next_check"),
  mapped_data<host>(
    &host::next_notification,
    NDO_DATA_NEXTHOSTNOTIFICATION,
    "next_host_notification"),
  mapped_data<host>(
    &host::no_more_notifications,
    NDO_DATA_NOMORENOTIFICATIONS,
    "no_more_notifications"),
  mapped_data<host>(
    &host::notes,
    NDO_DATA_NOTES,
    "notes"),
  mapped_data<host>(
    &host::notes_url,
    NDO_DATA_NOTESURL,
    "notes_url"),
  mapped_data<host>(
    &host::notification_interval,
    NDO_DATA_HOSTNOTIFICATIONINTERVAL,
    "notification_interval"),
  mapped_data<host>(
    &host::notification_period,
    NDO_DATA_HOSTNOTIFICATIONPERIOD,
    "notification_period"),
  mapped_data<host>(
    &host::notifications_enabled,
    NDO_DATA_NOTIFICATIONSENABLED,
    "notify"),
  mapped_data<host>(
    &host::notify_on_down,
    NDO_DATA_NOTIFYHOSTDOWN,
    "notify_on_down"),
  mapped_data<host>(
    &host::notify_on_downtime,
    NDO_DATA_NOTIFYHOSTDOWNTIME,
    "notify_on_downtime"),
  mapped_data<host>(
    &host::notify_on_flapping,
    NDO_DATA_NOTIFYHOSTFLAPPING,
    "notify_on_flapping"),
  mapped_data<host>(
    &host::notify_on_recovery,
    NDO_DATA_NOTIFYHOSTRECOVERY,
    "notify_on_recovery"),
  mapped_data<host>(
    &host::notify_on_unreachable,
    NDO_DATA_NOTIFYHOSTUNREACHABLE,
    "notify_on_unreachable"),
  mapped_data<host>(
    &host::obsess_over,
    NDO_DATA_OBSESSOVERHOST,
    "obsess_over_host"),
  mapped_data<host>(
    &host::passive_checks_enabled,
    NDO_DATA_PASSIVEHOSTCHECKSENABLED,
    "passive_checks"),
  mapped_data<host>(
    &host::percent_state_change,
    NDO_DATA_PERCENTSTATECHANGE,
    "percent_state_change"),
  mapped_data<host>(
    &host::problem_has_been_acknowledged,
    NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
    "acknowledged"),
  mapped_data<host>(
    &host::process_performance_data,
    NDO_DATA_PROCESSPERFORMANCEDATA,
    "process_perfdata"),
  mapped_data<host>(
    &host::retain_nonstatus_information,
    NDO_DATA_RETAINHOSTNONSTATUSINFORMATION,
    "retain_nonstatus_information"),
  mapped_data<host>(
    &host::retain_status_information,
    NDO_DATA_RETAINHOSTSTATUSINFORMATION,
    "retain_status_information"),
  mapped_data<host>(
    &host::retry_interval,
    NDO_DATA_RETRYCHECKINTERVAL,
    "retry_interval"),
  mapped_data<host>(
    &host::scheduled_downtime_depth,
    NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
    "scheduled_downtime_depth"),
  mapped_data<host>(
    &host::should_be_scheduled,
    NDO_DATA_SHOULDBESCHEDULED,
    "should_be_scheduled"),
  mapped_data<host>(
    &host::stalk_on_down,
    NDO_DATA_STALKHOSTONDOWN,
    "stalk_on_down"),
  mapped_data<host>(
    &host::stalk_on_unreachable,
    NDO_DATA_STALKHOSTONUNREACHABLE,
    "stalk_on_unreachable"),
  mapped_data<host>(
    &host::stalk_on_up,
    NDO_DATA_STALKHOSTONUP,
    "stalk_on_up"),
  mapped_data<host>(
    &host::state_type,
    NDO_DATA_STATETYPE,
    "state_type"),
  mapped_data<host>(
    &host::statusmap_image,
    NDO_DATA_STATUSMAPIMAGE,
    "statusmap_image"),
  mapped_data<host>(
    &host::check_command,
    NDO_DATA_CHECKCOMMAND,
    "check_command"),
  mapped_data<host>(
    &host::output,
    NDO_DATA_OUTPUT,
    "output"),
  mapped_data<host>(
    &host::perf_data,
    NDO_DATA_PERFDATA,
    "perfdata"),
  mapped_data<host>()
};

// host_check members mapping.
static mapped_data<host_check> const host_check_mapping[] = {
  mapped_data<host_check>(
    &host_check::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<host_check>(
    &host_check::command_line,
    NDO_DATA_COMMANDLINE,
    "command_line"),
  mapped_data<host_check>()
};

// host_dependency members mapping.
static mapped_data<host_dependency> const host_dependency_mapping[] = {
  mapped_data<host_dependency>(
    &host_dependency::dependency_period,
    NDO_DATA_DEPENDENCYPERIOD,
    "dependency_period"),
  mapped_data<host_dependency>(
    &host_dependency::dependent_host_id,
    NDO_DATA_DEPENDENTHOSTNAME,
    "dependent_host_id",
    true),
  mapped_data<host_dependency>(
    &host_dependency::execution_failure_options,
    NDO_DATA_HOSTFAILUREPREDICTIONOPTIONS,
    "execution_failure_options"),
  mapped_data<host_dependency>(
    &host_dependency::inherits_parent,
    NDO_DATA_INHERITSPARENT,
    "inherits_parent"),
  mapped_data<host_dependency>(
    &host_dependency::notification_failure_options,
    NDO_DATA_HOSTNOTIFICATIONCOMMAND,
    "notification_failure_options"),
  mapped_data<host_dependency>(
    &host_dependency::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<host_dependency>()
};

// host_group members mapping.
static mapped_data<host_group> const host_group_mapping[] = {
  mapped_data<host_group>(
    &host_group::action_url,
    NDO_DATA_ACTIONURL,
    "action_url"),
  mapped_data<host_group>(
    &host_group::alias,
    NDO_DATA_HOSTGROUPALIAS,
    "alias"),
  mapped_data<host_group>(
    &host_group::instance_id,
    NDO_DATA_INSTANCE,
    "instance_id",
    true),
  mapped_data<host_group>(
    &host_group::name,
    NDO_DATA_HOSTGROUPNAME,
    "name"),
  mapped_data<host_group>(
    &host_group::notes,
    NDO_DATA_NOTES,
    "notes"),
  mapped_data<host_group>(
    &host_group::notes_url,
    NDO_DATA_NOTESURL,
    "notes_url"),
  mapped_data<host_group>()
};

// host_group_member members mapping.
static mapped_data<host_group_member> const host_group_member_mapping[] = {
  mapped_data<host_group_member>(
    &host_group_member::group, // XXX : should be replaced by hostgroup_id
    NDO_DATA_HOSTGROUPNAME,
    NULL),
  mapped_data<host_group_member>(
    &host_group_member::instance_id,
    NDO_DATA_INSTANCE,
    NULL),
  mapped_data<host_group_member>(
    &host_group_member::host_id,
    NDO_DATA_HOSTGROUPMEMBER,
    "host_id",
    true),
  mapped_data<host_group_member>()
};

// host_parent members mapping.
static mapped_data<host_parent> const host_parent_mapping[] = {
  mapped_data<host_parent>(
    &host_parent::host_id,
    NDO_DATA_HOST,
    "child_id",
    true),
  mapped_data<host_parent>(
    &host_parent::parent_id,
    NDO_DATA_PARENTHOST,
    "parent_id",
    true),
  mapped_data<host_parent>()
};

// host_state members mapping.
static mapped_data<host_state> const host_state_mapping[] = {
  mapped_data<host_state>(
    &host_state::current_state,
    0,
    "state"),
  mapped_data<host_state>(
    &host_state::end_time,
    1,
    "end_time"),
  mapped_data<host_state>(
    &host_state::host_id,
    2,
    "host_id"),
  mapped_data<host_state>(
    &host_state::in_downtime,
    3,
    "in_downtime"),
  mapped_data<host_state>(
    &host_state::start_time,
    4,
    "start_time"),
  mapped_data<host_state>()
};

// host_status members mapping.
static mapped_data<host_status> const host_status_mapping[] = {
  mapped_data<host_status>(
    &host_status::acknowledgement_type,
    NDO_DATA_ACKNOWLEDGEMENTTYPE,
    "acknowledgement_type"),
  mapped_data<host_status>(
    &host_status::active_checks_enabled,
    NDO_DATA_ACTIVEHOSTCHECKSENABLED,
    "active_checks"),
  mapped_data<host_status>(
    &host_status::check_interval,
    NDO_DATA_NORMALCHECKINTERVAL,
    "check_interval"),
  mapped_data<host_status>(
    &host_status::check_period,
    NDO_DATA_HOSTCHECKPERIOD,
    "check_period"),
  mapped_data<host_status>(
    &host_status::check_type,
    NDO_DATA_CHECKTYPE,
    "check_type"),
  mapped_data<host_status>(
    &host_status::current_check_attempt,
    NDO_DATA_CURRENTCHECKATTEMPT,
    "check_attempt"),
  mapped_data<host_status>(
    &host_status::current_notification_number,
    NDO_DATA_CURRENTNOTIFICATIONNUMBER,
    "notification_number"),
  mapped_data<host_status>(
    &host_status::current_state,
    NDO_DATA_CURRENTSTATE,
    "state"),
  mapped_data<host_status>(
    &host_status::enabled,
    NDO_DATA_X3D,
    "enabled"),
  mapped_data<host_status>(
    &host_status::event_handler,
    NDO_DATA_EVENTHANDLER,
    "event_handler"),
  mapped_data<host_status>(
    &host_status::event_handler_enabled,
    NDO_DATA_EVENTHANDLERENABLED,
    "event_handler_enabled"),
  mapped_data<host_status>(
    &host_status::execution_time,
    NDO_DATA_EXECUTIONTIME,
    "execution_time"),
  mapped_data<host_status>(
    &host_status::failure_prediction_enabled,
    NDO_DATA_FAILUREPREDICTIONENABLED,
    "failure_prediction"),
  mapped_data<host_status>(
    &host_status::flap_detection_enabled,
    NDO_DATA_FLAPDETECTIONENABLED,
    "flap_detection"),
  mapped_data<host_status>(
    &host_status::has_been_checked,
    NDO_DATA_HASBEENCHECKED,
    "checked"),
  mapped_data<host_status>(
    &host_status::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<host_status>(
    &host_status::is_flapping,
    NDO_DATA_ISFLAPPING,
    "flapping"),
  mapped_data<host_status>(
    &host_status::last_check,
    NDO_DATA_LASTHOSTCHECK,
    "last_check"),
  mapped_data<host_status>(
    &host_status::last_hard_state,
    NDO_DATA_LASTHARDSTATE,
    "last_hard_state"),
  mapped_data<host_status>(
    &host_status::last_hard_state_change,
    NDO_DATA_LASTHARDSTATECHANGE,
    "last_hard_state_change"),
  mapped_data<host_status>(
    &host_status::last_notification,
    NDO_DATA_LASTHOSTNOTIFICATION,
    "last_notification"),
  mapped_data<host_status>(
    &host_status::last_state_change,
    NDO_DATA_LASTSTATECHANGE,
    "last_state_change"),
  mapped_data<host_status>(
    &host_status::last_time_down,
    NDO_DATA_LASTTIMEDOWN,
    "last_time_down"),
  mapped_data<host_status>(
    &host_status::last_time_unreachable,
    NDO_DATA_LASTTIMEUNREACHABLE,
    "last_time_unreachable"),
  mapped_data<host_status>(
    &host_status::last_time_up,
    NDO_DATA_LASTTIMEUP,
    "last_time_up"),
  mapped_data<host_status>(
    &host_status::last_update,
    0, // XXX : should find macro
    "last_update"),
  mapped_data<host_status>(
    &host_status::latency,
    NDO_DATA_LATENCY,
    "latency"),
  mapped_data<host_status>(
    &host_status::max_check_attempts,
    NDO_DATA_MAXCHECKATTEMPTS,
    "max_check_attempts"),
  mapped_data<host_status>(
    &host_status::modified_attributes,
    NDO_DATA_MODIFIEDHOSTATTRIBUTES,
    "modified_attributes"),
  mapped_data<host_status>(
    &host_status::next_check,
    NDO_DATA_NEXTHOSTCHECK,
    "next_check"),
  mapped_data<host_status>(
    &host_status::next_notification,
    NDO_DATA_NEXTHOSTNOTIFICATION,
    "next_host_notification"),
  mapped_data<host_status>(
    &host_status::no_more_notifications,
    NDO_DATA_NOMORENOTIFICATIONS,
    "no_more_notifications"),
  mapped_data<host_status>(
    &host_status::notifications_enabled,
    NDO_DATA_NOTIFICATIONSENABLED,
    "notify"),
  mapped_data<host_status>(
    &host_status::obsess_over,
    NDO_DATA_OBSESSOVERHOST,
    "obsess_over_host"),
  mapped_data<host_status>(
    &host_status::passive_checks_enabled,
    NDO_DATA_PASSIVEHOSTCHECKSENABLED,
    "passive_checks"),
  mapped_data<host_status>(
    &host_status::percent_state_change,
    NDO_DATA_PERCENTSTATECHANGE,
    "percent_state_change"),
  mapped_data<host_status>(
    &host_status::problem_has_been_acknowledged,
    NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
    "acknowledged"),
  mapped_data<host_status>(
    &host_status::process_performance_data,
    NDO_DATA_PROCESSPERFORMANCEDATA,
    "process_perfdata"),
  mapped_data<host_status>(
    &host_status::retry_interval,
    NDO_DATA_RETRYCHECKINTERVAL,
    "retry_interval"),
  mapped_data<host_status>(
    &host_status::scheduled_downtime_depth,
    NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
    "scheduled_downtime_depth"),
  mapped_data<host_status>(
    &host_status::should_be_scheduled,
    NDO_DATA_SHOULDBESCHEDULED,
    "should_be_scheduled"),
  mapped_data<host_status>(
    &host_status::state_type,
    NDO_DATA_STATETYPE,
    "state_type"),
  mapped_data<host_status>(
    &host_status::check_command,
    NDO_DATA_CHECKCOMMAND,
    "check_command"),
  mapped_data<host_status>(
    &host_status::output,
    NDO_DATA_OUTPUT,
    "output"),
  mapped_data<host_status>(
    &host_status::perf_data,
    NDO_DATA_PERFDATA,
    "perfdata"),
  mapped_data<host_status>()
};

// instance members mapping.
static mapped_data<instance> const instance_mapping[] = {
  mapped_data<instance>(
    &instance::id,
    NDO_DATA_INSTANCE,
    "instance_id",
    true),
  mapped_data<instance>(
    &instance::name,
    NDO_DATA_PROGRAMNAME,
    "name"),
  mapped_data<instance>(
    &instance::is_running,
    NDO_DATA_RUNTIME,
    "running"),
  mapped_data<instance>(
    &instance::pid,
    NDO_DATA_PROCESSID,
    "pid"),
  mapped_data<instance>(
    &instance::program_end,
    NDO_DATA_ENDTIME,
    "end_time"),
  mapped_data<instance>(
    &instance::program_start,
    NDO_DATA_PROGRAMSTARTTIME,
    "start_time"),
  mapped_data<instance>()
};

// instance_status members mapping.
static mapped_data<instance_status> const instance_status_mapping[] = {
  mapped_data<instance_status>(
    &instance_status::active_host_checks_enabled,
    NDO_DATA_ACTIVEHOSTCHECKSENABLED,
    "active_host_checks"),
  mapped_data<instance_status>(
    &instance_status::active_service_checks_enabled,
    NDO_DATA_ACTIVESERVICECHECKSENABLED,
    "active_service_checks"),
  mapped_data<instance_status>(
    &instance_status::address,
    NDO_DATA_HOSTADDRESS,
    "address"),
  mapped_data<instance_status>(
    &instance_status::check_hosts_freshness,
    NDO_DATA_HOSTFRESHNESSCHECKSENABLED,
    "check_hosts_freshness"),
  mapped_data<instance_status>(
    &instance_status::check_services_freshness,
    NDO_DATA_SERVICEFRESHNESSCHECKSENABLED,
    "check_services_freshness"),
  mapped_data<instance_status>(
    &instance_status::daemon_mode,
    NDO_DATA_DAEMONMODE,
    "daemon_mode"),
  mapped_data<instance_status>(
    &instance_status::description,
    NDO_DATA_SERVICEDESCRIPTION,
    "description"),
  mapped_data<instance_status>(
    &instance_status::event_handler_enabled,
    NDO_DATA_EVENTHANDLERENABLED,
    "event_handlers"),
  mapped_data<instance_status>(
    &instance_status::failure_prediction_enabled,
    NDO_DATA_FAILUREPREDICTIONENABLED,
    "failure_prediction"),
  mapped_data<instance_status>(
    &instance_status::flap_detection_enabled,
    NDO_DATA_FLAPDETECTIONENABLED,
    "flap_detection"),
  mapped_data<instance_status>(
    &instance_status::id,
    NDO_DATA_INSTANCE,
    "instance_id",
    true),
  mapped_data<instance_status>(
    &instance_status::last_alive,
    NDO_DATA_LASTSTATE,
    "last_alive"),
  mapped_data<instance_status>(
    &instance_status::last_command_check,
    NDO_DATA_LASTCOMMANDCHECK,
    "last_command_check"),
  mapped_data<instance_status>(
    &instance_status::last_log_rotation,
    NDO_DATA_LASTLOGROTATION,
    "last_log_rotation"),
  mapped_data<instance_status>(
    &instance_status::modified_host_attributes,
    NDO_DATA_MODIFIEDHOSTATTRIBUTES,
    "modified_host_attributes"),
  mapped_data<instance_status>(
    &instance_status::modified_service_attributes,
    NDO_DATA_MODIFIEDSERVICEATTRIBUTES,
    "modified_service_attributes"),
  mapped_data<instance_status>(
    &instance_status::notifications_enabled,
    NDO_DATA_NOTIFICATIONSENABLED,
    "notifications"),
  mapped_data<instance_status>(
    &instance_status::obsess_over_hosts,
    NDO_DATA_OBSESSOVERHOST,
    "obsess_over_hosts"),
  mapped_data<instance_status>(
    &instance_status::obsess_over_services,
    NDO_DATA_OBSESSOVERSERVICE,
    "obsess_over_services"),
  mapped_data<instance_status>(
    &instance_status::passive_host_checks_enabled,
    NDO_DATA_PASSIVEHOSTCHECKSENABLED,
    "passive_host_checks"),
  mapped_data<instance_status>(
    &instance_status::passive_service_checks_enabled,
    NDO_DATA_PASSIVESERVICECHECKSENABLED,
    "passive_service_checks"),
  mapped_data<instance_status>(
    &instance_status::process_performance_data,
    NDO_DATA_PROCESSPERFORMANCEDATA,
    "process_perfdata"),
  mapped_data<instance_status>(
    &instance_status::global_host_event_handler,
    NDO_DATA_GLOBALHOSTEVENTHANDLER,
    "global_host_event_handler"),
  mapped_data<instance_status>(
    &instance_status::global_service_event_handler,
    NDO_DATA_GLOBALSERVICEEVENTHANDLER,
    "global_service_event_handler"),
  mapped_data<instance_status>()
};

// issue members mapping.
static mapped_data<issue> const issue_mapping[] = {
  mapped_data<issue>(
    &issue::ack_time,
    NDO_DATA_ACKDATA,
    "ack_time"),
  mapped_data<issue>(
    &issue::end_time,
    NDO_DATA_ENDTIME,
    "end_time"),
  mapped_data<issue>(
    &issue::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<issue>(
    &issue::service_id,
    NDO_DATA_SERVICE,
    "service_id",
    true),
  mapped_data<issue>(
    &issue::start_time,
    NDO_DATA_STARTTIME,
    "start_time"),
  mapped_data<issue>()
};

// issue_parent members mapping.
static mapped_data<issue_parent> const issue_parent_mapping[] = {
  mapped_data<issue_parent>(
    &issue_parent::child_host_id,
    1,
    "child_host_id",
    true),
  mapped_data<issue_parent>(
    &issue_parent::child_service_id,
    2,
    "child_service_id",
    true),
  mapped_data<issue_parent>(
    &issue_parent::child_start_time,
    3,
    "child_start_time"),
  mapped_data<issue_parent>(
    &issue_parent::end_time,
    4,
    "end_time"),
  mapped_data<issue_parent>(
    &issue_parent::parent_host_id,
    5,
    "parent_host_id",
    true),
  mapped_data<issue_parent>(
    &issue_parent::parent_service_id,
    6,
    "parent_service_id",
    true),
  mapped_data<issue_parent>(
    &issue_parent::parent_start_time,
    7,
    "parent_start_time"),
  mapped_data<issue_parent>(
    &issue_parent::start_time,
    8,
    "start_time"),
  mapped_data<issue_parent>()
};

// Log members mapping.
static mapped_data<log_entry> log_mapping[] = {
  mapped_data<log_entry>(
    &log_entry::c_time,
    NDO_DATA_LOGENTRYTIME,
    "ctime"),
  mapped_data<log_entry>(
    &log_entry::host_id,
    NDO_DATA_HOSTID,
    "host_id",
    true),
  mapped_data<log_entry>(
    &log_entry::host_name,
    NDO_DATA_HOST,
    "host_name"),
  mapped_data<log_entry>(
    &log_entry::instance_name,
    NDO_DATA_PROGRAMNAME,
    "instance_name"),
  mapped_data<log_entry>(
    &log_entry::issue_start_time,
    NDO_DATA_DURATION,
    NULL),
  mapped_data<log_entry>(
    &log_entry::msg_type,
    NDO_DATA_ENTRYTYPE,
    "msg_type"),
  mapped_data<log_entry>(
    &log_entry::notification_cmd,
    NDO_DATA_HOSTNOTIFICATIONCOMMAND,
    "notification_cmd"),
  mapped_data<log_entry>(
    &log_entry::notification_contact,
    NDO_DATA_CONTACT,
    "notification_contact"),
  mapped_data<log_entry>(
    &log_entry::retry,
    NDO_DATA_HOSTRETRYINTERVAL,
    "retry"),
  mapped_data<log_entry>(
    &log_entry::service_description,
    NDO_DATA_SERVICEDESCRIPTION,
    "service_description"),
  mapped_data<log_entry>(
    &log_entry::service_id,
    NDO_DATA_SERVICEID,
    "service_id",
    true),
  mapped_data<log_entry>(
    &log_entry::status,
    NDO_DATA_CURRENTSTATE,
    "status"),
  mapped_data<log_entry>(
    &log_entry::type,
    NDO_DATA_TYPE,
    "type"),
  mapped_data<log_entry>(
    &log_entry::output,
    NDO_DATA_OUTPUT,
    "output"),
  mapped_data<log_entry>()
};

// Module members mapping.
static mapped_data<module> const module_mapping[] = {
  mapped_data<module>(
    &module::args,
    1,
    "args"),
  mapped_data<module>(
    &module::filename,
    2,
    "filename"),
  mapped_data<module>(
    &module::instance_id,
    3,
    "instance_id",
    true),
  mapped_data<module>(
    &module::loaded,
    4,
    "loaded"),
  mapped_data<module>(
    &module::should_be_loaded,
    5,
    "should_be_loaded"),
  mapped_data<module>()
};

// Notification members mapping.
static mapped_data<notification> const notification_mapping[] = {
  mapped_data<notification>(
    &notification::contacts_notified,
    NDO_DATA_CONTACTSNOTIFIED,
    "contacts_notified"),
  mapped_data<notification>(
    &notification::end_time,
    NDO_DATA_ENDTIME,
    "end_time"),
  mapped_data<notification>(
    &notification::escalated,
    NDO_DATA_ESCALATED,
    "escalated"),
  mapped_data<notification>(
    &notification::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<notification>(
    &notification::reason_type,
    NDO_DATA_NOTIFICATIONREASON,
    "reason_type"),
  mapped_data<notification>(
    &notification::service_id,
    NDO_DATA_SERVICE,
    "service_id",
    true),
  mapped_data<notification>(
    &notification::start_time,
    NDO_DATA_STARTTIME,
    "start_time"),
  mapped_data<notification>(
    &notification::state,
    NDO_DATA_STATE,
    "state"),
  mapped_data<notification>(
    &notification::type,
    NDO_DATA_NOTIFICATIONTYPE,
    "type"),
  mapped_data<notification>(
    &notification::ack_author,
    NDO_DATA_ACKAUTHOR,
    "ack_author"),
  mapped_data<notification>(
    &notification::ack_data,
    NDO_DATA_ACKDATA,
    "ack_data"),
  mapped_data<notification>(
    &notification::command_name,
    NDO_DATA_COMMANDNAME,
    "command_name"),
  mapped_data<notification>(
    &notification::contact_name,
    NDO_DATA_CONTACTNAME,
    "contact_name"),
  mapped_data<notification>(
    &notification::output,
    NDO_DATA_OUTPUT,
    "output"),
  mapped_data<notification>()
};

// service members mapping.
static mapped_data<service> const service_mapping[] = {
  mapped_data<service>(
    &service::acknowledgement_type,
    NDO_DATA_ACKNOWLEDGEMENTTYPE,
    "acknowledgement_type"),
  mapped_data<service>(
    &service::action_url,
    NDO_DATA_ACTIONURL,
    "action_url"),
  mapped_data<service>(
    &service::active_checks_enabled,
    NDO_DATA_ACTIVESERVICECHECKSENABLED,
    "active_checks"),
  mapped_data<service>(
    &service::check_freshness,
    NDO_DATA_SERVICEFRESHNESSCHECKSENABLED,
    "check_freshness"),
  mapped_data<service>(
    &service::check_interval,
    NDO_DATA_NORMALCHECKINTERVAL,
    "check_interval"),
  mapped_data<service>(
    &service::check_period,
    NDO_DATA_SERVICECHECKPERIOD,
    "check_period"),
  mapped_data<service>(
    &service::check_type,
    NDO_DATA_CHECKTYPE,
    "check_type"),
  mapped_data<service>(
    &service::current_check_attempt,
    NDO_DATA_CURRENTCHECKATTEMPT,
    "check_attempt"),
  mapped_data<service>(
    &service::current_notification_number,
    NDO_DATA_CURRENTNOTIFICATIONNUMBER,
    "notification_number"),
  mapped_data<service>(
    &service::current_state,
    NDO_DATA_CURRENTSTATE,
    "state"),
  mapped_data<service>(
    &service::default_active_checks_enabled,
    0,
    "default_active_checks"),
  mapped_data<service>(
    &service::default_event_handler_enabled,
    0,
    "default_event_handler_enabled"),
  mapped_data<service>(
    &service::default_failure_prediction,
    0,
    "default_failure_prediction"),
  mapped_data<service>(
    &service::default_flap_detection_enabled,
    0,
    "default_flap_detection"),
  mapped_data<service>(
    &service::default_notifications_enabled,
    0,
    "default_notify"),
  mapped_data<service>(
    &service::default_passive_checks_enabled,
    0,
    "default_passive_checks"),
  mapped_data<service>(
    &service::default_process_perf_data,
    0,
    "default_process_perfdata"),
  mapped_data<service>(
    &service::display_name,
    NDO_DATA_DISPLAYNAME,
    "display_name"),
  mapped_data<service>(
    &service::enabled,
    NDO_DATA_X3D,
    "enabled"),
  mapped_data<service>(
    &service::event_handler,
    NDO_DATA_EVENTHANDLER,
    "event_handler"),
  mapped_data<service>(
    &service::event_handler_enabled,
    NDO_DATA_EVENTHANDLERENABLED,
    "event_handler_enabled"),
  mapped_data<service>(
    &service::execution_time,
    NDO_DATA_EXECUTIONTIME,
    "execution_time"),
  mapped_data<service>(
    &service::failure_prediction_enabled,
    NDO_DATA_FAILUREPREDICTIONENABLED,
    "failure_prediction"),
  mapped_data<service>(
    &service::failure_prediction_options,
    NDO_DATA_SERVICEFAILUREPREDICTIONOPTIONS,
    "failure_prediction_options"),
  mapped_data<service>(
    &service::first_notification_delay,
    NDO_DATA_FIRSTNOTIFICATIONDELAY,
    "first_notification_delay"),
  mapped_data<service>(
    &service::flap_detection_enabled,
    NDO_DATA_FLAPDETECTIONENABLED,
    "flap_detection"),
  mapped_data<service>(
    &service::flap_detection_on_critical,
    NDO_DATA_FLAPDETECTIONONCRITICAL,
    "flap_detection_on_critical"),
  mapped_data<service>(
    &service::flap_detection_on_ok,
    NDO_DATA_FLAPDETECTIONONOK,
    "flap_detection_on_ok"),
  mapped_data<service>(
    &service::flap_detection_on_unknown,
    NDO_DATA_FLAPDETECTIONONUNKNOWN,
    "flap_detection_on_unknown"),
  mapped_data<service>(
    &service::flap_detection_on_warning,
    NDO_DATA_FLAPDETECTIONONWARNING,
    "flap_detection_on_warning"),
  mapped_data<service>(
    &service::freshness_threshold,
    NDO_DATA_SERVICEFRESHNESSTHRESHOLD,
    "freshness_threshold"),
  mapped_data<service>(
    &service::has_been_checked,
    NDO_DATA_HASBEENCHECKED,
    "checked"),
  mapped_data<service>(
    &service::high_flap_threshold,
    NDO_DATA_HIGHSERVICEFLAPTHRESHOLD,
    "high_flap_threshold"),
  mapped_data<service>(
    &service::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<service>(
    &service::icon_image,
    NDO_DATA_ICONIMAGE,
    "icon_image"),
  mapped_data<service>(
    &service::icon_image_alt,
    NDO_DATA_ICONIMAGEALT,
    "icon_image_alt"),
  mapped_data<service>(
    &service::service_id,
    NDO_DATA_SERVICE,
    "service_id",
    true),
  mapped_data<service>(
    &service::is_flapping,
    NDO_DATA_ISFLAPPING,
    "flapping"),
  mapped_data<service>(
    &service::is_volatile,
    NDO_DATA_SERVICEISVOLATILE,
    "volatile"),
  mapped_data<service>(
    &service::last_check,
    NDO_DATA_LASTSERVICECHECK,
    "last_check"),
  mapped_data<service>(
    &service::last_hard_state,
    NDO_DATA_LASTHARDSTATE,
    "last_hard_state"),
  mapped_data<service>(
    &service::last_hard_state_change,
    NDO_DATA_LASTHARDSTATECHANGE,
    "last_hard_state_change"),
  mapped_data<service>(
    &service::last_notification,
    NDO_DATA_LASTSERVICENOTIFICATION,
    "last_notification"),
  mapped_data<service>(
    &service::last_state_change,
    NDO_DATA_LASTSTATECHANGE,
    "last_state_change"),
  mapped_data<service>(
    &service::last_time_critical,
    NDO_DATA_LASTTIMECRITICAL,
    "last_time_critical"),
  mapped_data<service>(
    &service::last_time_ok,
    NDO_DATA_LASTTIMEOK,
    "last_time_ok"),
  mapped_data<service>(
    &service::last_time_unknown,
    NDO_DATA_LASTTIMEUNKNOWN,
    "last_time_unknown"),
  mapped_data<service>(
    &service::last_time_warning,
    NDO_DATA_LASTTIMEWARNING,
    "last_time_warning"),
  mapped_data<service>(
    &service::last_update,
    0, // XXX : should find macro
    "last_update"),
  mapped_data<service>(
    &service::latency,
    NDO_DATA_LATENCY,
    "latency"),
  mapped_data<service>(
    &service::low_flap_threshold,
    NDO_DATA_LOWSERVICEFLAPTHRESHOLD,
    "low_flap_threshold"),
  mapped_data<service>(
    &service::max_check_attempts,
    NDO_DATA_MAXCHECKATTEMPTS,
    "max_check_attempts"),
  mapped_data<service>(
    &service::modified_attributes,
    NDO_DATA_MODIFIEDSERVICEATTRIBUTES,
    "modified_attributes"),
  mapped_data<service>(
    &service::next_check,
    NDO_DATA_NEXTSERVICECHECK,
    "next_check"),
  mapped_data<service>(
    &service::next_notification,
    NDO_DATA_NEXTSERVICENOTIFICATION,
    "next_notification"),
  mapped_data<service>(
    &service::no_more_notifications,
    NDO_DATA_NOMORENOTIFICATIONS,
    "no_more_notifications"),
  mapped_data<service>(
    &service::notes,
    NDO_DATA_NOTES,
    "notes"),
  mapped_data<service>(
    &service::notes_url,
    NDO_DATA_NOTESURL,
    "notes_url"),
  mapped_data<service>(
    &service::notification_interval,
    NDO_DATA_SERVICENOTIFICATIONINTERVAL,
    "notification_interval"),
  mapped_data<service>(
    &service::notification_period,
    NDO_DATA_SERVICENOTIFICATIONPERIOD,
    "notification_period"),
  mapped_data<service>(
    &service::notifications_enabled,
    NDO_DATA_NOTIFICATIONSENABLED,
    "notify"),
  mapped_data<service>(
    &service::notify_on_critical,
    NDO_DATA_NOTIFYSERVICECRITICAL,
    "notify_on_critical"),
  mapped_data<service>(
    &service::notify_on_downtime,
    NDO_DATA_NOTIFYSERVICEDOWNTIME,
    "notify_on_downtime"),
  mapped_data<service>(
    &service::notify_on_flapping,
    NDO_DATA_NOTIFYSERVICEFLAPPING,
    "notify_on_flapping"),
  mapped_data<service>(
    &service::notify_on_recovery,
    NDO_DATA_NOTIFYSERVICERECOVERY,
    "notify_on_recovery"),
  mapped_data<service>(
    &service::notify_on_unknown,
    NDO_DATA_NOTIFYSERVICEUNKNOWN,
    "notify_on_unknown"),
  mapped_data<service>(
    &service::notify_on_warning,
    NDO_DATA_NOTIFYSERVICEWARNING,
    "notify_on_warning"),
  mapped_data<service>(
    &service::obsess_over,
    NDO_DATA_OBSESSOVERSERVICE,
    "obsess_over_service"),
  mapped_data<service>(
    &service::passive_checks_enabled,
    NDO_DATA_PASSIVESERVICECHECKSENABLED,
    "passive_checks"),
  mapped_data<service>(
    &service::percent_state_change,
    NDO_DATA_PERCENTSTATECHANGE,
    "percent_state_change"),
  mapped_data<service>(
    &service::problem_has_been_acknowledged,
    NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
    "acknowledged"),
  mapped_data<service>(
    &service::process_performance_data,
    NDO_DATA_PROCESSPERFORMANCEDATA,
    "process_perfdata"),
  mapped_data<service>(
    &service::retain_nonstatus_information,
    NDO_DATA_RETAINSERVICENONSTATUSINFORMATION,
    "retain_nonstatus_information"),
  mapped_data<service>(
    &service::retain_status_information,
    NDO_DATA_RETAINSERVICESTATUSINFORMATION,
    "retain_status_information"),
  mapped_data<service>(
    &service::retry_interval,
    NDO_DATA_RETRYCHECKINTERVAL,
    "retry_interval"),
  mapped_data<service>(
    &service::scheduled_downtime_depth,
    NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
    "scheduled_downtime_depth"),
  mapped_data<service>(
    &service::service_description,
    NDO_DATA_SERVICEDESCRIPTION,
    "description"),
  mapped_data<service>(
    &service::should_be_scheduled,
    NDO_DATA_SHOULDBESCHEDULED,
    "should_be_scheduled"),
  mapped_data<service>(
    &service::stalk_on_critical,
    NDO_DATA_STALKSERVICEONCRITICAL,
    "stalk_on_critical"),
  mapped_data<service>(
    &service::stalk_on_ok,
    NDO_DATA_STALKSERVICEONOK,
    "stalk_on_ok"),
  mapped_data<service>(
    &service::stalk_on_unknown,
    NDO_DATA_STALKSERVICEONUNKNOWN,
    "stalk_on_unknown"),
  mapped_data<service>(
    &service::stalk_on_warning,
    NDO_DATA_STALKSERVICEONWARNING,
    "stalk_on_warning"),
  mapped_data<service>(
    &service::state_type,
    NDO_DATA_STATETYPE,
    "state_type"),
  mapped_data<service>(
    &service::check_command,
    NDO_DATA_CHECKCOMMAND,
    "check_command"),
  mapped_data<service>(
    &service::output,
    NDO_DATA_OUTPUT,
    "output"),
  mapped_data<service>(
    &service::perf_data,
    NDO_DATA_PERFDATA,
    "perfdata"),
  mapped_data<service>()
};

// service_check members mapping.
static mapped_data<service_check> const service_check_mapping[] = {
  mapped_data<service_check>(
    &service_check::command_line,
    NDO_DATA_COMMANDLINE,
    "command_line"),
  mapped_data<service_check>(
    &service_check::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<service_check>(
    &service_check::service_id,
    NDO_DATA_SERVICE,
    "service_id",
    true),
  mapped_data<service_check>()
};

// service_dependency members mapping.
static mapped_data<service_dependency> const service_dependency_mapping[] = {
  mapped_data<service_dependency>(
    &service_dependency::dependency_period,
    NDO_DATA_DEPENDENCYPERIOD,
    "dependency_period"),
  mapped_data<service_dependency>(
    &service_dependency::dependent_host_id,
    NDO_DATA_DEPENDENTHOSTNAME,
    "dependent_host_id",
    true),
  mapped_data<service_dependency>(
    &service_dependency::dependent_service_id,
    NDO_DATA_DEPENDENTSERVICEDESCRIPTION,
    "dependent_service_id",
    true),
  mapped_data<service_dependency>(
    &service_dependency::execution_failure_options,
    NDO_DATA_SERVICEFAILUREPREDICTIONOPTIONS,
    "execution_failure_options"),
  mapped_data<service_dependency>(
    &service_dependency::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<service_dependency>(
    &service_dependency::inherits_parent,
    NDO_DATA_INHERITSPARENT,
    "inherits_parent"),
  mapped_data<service_dependency>(
    &service_dependency::notification_failure_options,
    NDO_DATA_SERVICENOTIFICATIONCOMMAND,
    "notification_failure_options"),
  mapped_data<service_dependency>(
    &service_dependency::service_id,
    NDO_DATA_SERVICE,
    "service_id",
    true),
  mapped_data<service_dependency>()
};

// service_group members mapping.
static mapped_data<service_group> const service_group_mapping[] = {
  mapped_data<service_group>(
    &service_group::action_url,
    NDO_DATA_ACTIONURL,
    "action_url"),
  mapped_data<service_group>(
    &service_group::alias,
    NDO_DATA_SERVICEGROUPALIAS,
    "alias"),
  mapped_data<service_group>(
    &service_group::instance_id,
    NDO_DATA_INSTANCE,
    "instance_id",
    true),
  mapped_data<service_group>(
    &service_group::name,
    NDO_DATA_SERVICEGROUPNAME,
    "name"),
  mapped_data<service_group>(
    &service_group::notes,
    NDO_DATA_NOTES,
    "notes"),
  mapped_data<service_group>(
    &service_group::notes_url,
    NDO_DATA_NOTESURL,
    "notes_url"),
  mapped_data<service_group>()
};

// service_group_member members mapping.
static mapped_data<service_group_member> const service_group_member_mapping[] = {
  mapped_data<service_group_member>(
    &service_group_member::group, // XXX : should be replaced by servicegroup_id
    NDO_DATA_SERVICEGROUPNAME,
    NULL),
  mapped_data<service_group_member>(
    &service_group_member::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<service_group_member>(
    &service_group_member::instance_id,
    NDO_DATA_INSTANCE,
    NULL),
  mapped_data<service_group_member>(
    &service_group_member::service_id,
    NDO_DATA_SERVICEGROUPMEMBER,
    "service_id",
    true),
  mapped_data<service_group_member>()
};

// service_state members mapping.
static mapped_data<service_state> const service_state_mapping [] = {
  mapped_data<service_state>(
    &service_state::current_state,
    0,
    "state"),
  mapped_data<service_state>(
    &service_state::end_time,
    1,
    "end_time"),
  mapped_data<service_state>(
    &service_state::host_id,
    2,
    "host_id"),
  mapped_data<service_state>(
    &service_state::in_downtime,
    3,
    "in_downtime"),
  mapped_data<service_state>(
    &service_state::service_id,
    4,
    "service_id"),
  mapped_data<service_state>(
    &service_state::start_time,
    5,
    "start_time"),
  mapped_data<service_state>()
};

// service_status members mapping.
static mapped_data<service_status> const service_status_mapping[] = {
  mapped_data<service_status>(
    &service_status::acknowledgement_type,
    NDO_DATA_ACKNOWLEDGEMENTTYPE,
    "acknowledgement_type"),
  mapped_data<service_status>(
    &service_status::active_checks_enabled,
    NDO_DATA_ACTIVESERVICECHECKSENABLED,
    "active_checks"),
  mapped_data<service_status>(
    &service_status::check_interval,
    NDO_DATA_NORMALCHECKINTERVAL,
    "check_interval"),
  mapped_data<service_status>(
    &service_status::check_period,
    NDO_DATA_SERVICECHECKPERIOD,
    "check_period"),
  mapped_data<service_status>(
    &service_status::check_type,
    NDO_DATA_CHECKTYPE,
    "check_type"),
  mapped_data<service_status>(
    &service_status::current_check_attempt,
    NDO_DATA_CURRENTCHECKATTEMPT,
    "check_attempt"),
  mapped_data<service_status>(
    &service_status::current_notification_number,
    NDO_DATA_CURRENTNOTIFICATIONNUMBER,
    "notification_number"),
  mapped_data<service_status>(
    &service_status::current_state,
    NDO_DATA_CURRENTSTATE,
    "state"),
  mapped_data<service_status>(
    &service_status::enabled,
    NDO_DATA_X3D,
    "enabled"),
  mapped_data<service_status>(
    &service_status::event_handler,
    NDO_DATA_EVENTHANDLER,
    "event_handler"),
  mapped_data<service_status>(
    &service_status::event_handler_enabled,
    NDO_DATA_EVENTHANDLERENABLED,
    "event_handler_enabled"),
  mapped_data<service_status>(
    &service_status::execution_time,
    NDO_DATA_EXECUTIONTIME,
    "execution_time"),
  mapped_data<service_status>(
    &service_status::failure_prediction_enabled,
    NDO_DATA_FAILUREPREDICTIONENABLED,
    "failure_prediction"),
  mapped_data<service_status>(
    &service_status::flap_detection_enabled,
    NDO_DATA_FLAPDETECTIONENABLED,
    "flap_detection"),
  mapped_data<service_status>(
    &service_status::has_been_checked,
    NDO_DATA_HASBEENCHECKED,
    "checked"),
  mapped_data<service_status>(
    &service_status::host_id,
    NDO_DATA_HOST,
    "host_id",
    true),
  mapped_data<service_status>(
    &service_status::is_flapping,
    NDO_DATA_ISFLAPPING,
    "flapping"),
  mapped_data<service_status>(
    &service_status::last_check,
    NDO_DATA_LASTSERVICECHECK,
    "last_check"),
  mapped_data<service_status>(
    &service_status::last_hard_state,
    NDO_DATA_LASTHARDSTATE,
    "last_hard_state"),
  mapped_data<service_status>(
    &service_status::last_hard_state_change,
    NDO_DATA_LASTHARDSTATECHANGE,
    "last_hard_state_change"),
  mapped_data<service_status>(
    &service_status::last_notification,
    NDO_DATA_LASTSERVICENOTIFICATION,
    "last_notification"),
  mapped_data<service_status>(
    &service_status::last_state_change,
    NDO_DATA_LASTSTATECHANGE,
    "last_state_change"),
  mapped_data<service_status>(
    &service_status::last_time_critical,
    NDO_DATA_LASTTIMECRITICAL,
    "last_time_critical"),
  mapped_data<service_status>(
    &service_status::last_time_ok,
    NDO_DATA_LASTTIMEOK,
    "last_time_ok"),
  mapped_data<service_status>(
    &service_status::last_time_unknown,
    NDO_DATA_LASTTIMEUNKNOWN,
    "last_time_unknown"),
  mapped_data<service_status>(
    &service_status::last_time_warning,
    NDO_DATA_LASTTIMEWARNING,
    "last_time_warning"),
  mapped_data<service_status>(
    &service_status::last_update,
    0, // XXX : should find macro
    "last_update"),
  mapped_data<service_status>(
    &service_status::latency,
    NDO_DATA_LATENCY,
    "latency"),
  mapped_data<service_status>(
    &service_status::max_check_attempts,
    NDO_DATA_MAXCHECKATTEMPTS,
    "max_check_attempts"),
  mapped_data<service_status>(
    &service_status::modified_attributes,
    NDO_DATA_MODIFIEDSERVICEATTRIBUTES,
    "modified_attributes"),
  mapped_data<service_status>(
    &service_status::next_check,
    NDO_DATA_NEXTSERVICECHECK,
    "next_check"),
  mapped_data<service_status>(
    &service_status::next_notification,
    NDO_DATA_NEXTSERVICENOTIFICATION,
    "next_notification"),
  mapped_data<service_status>(
    &service_status::no_more_notifications,
    NDO_DATA_NOMORENOTIFICATIONS,
    "no_more_notifications"),
  mapped_data<service_status>(
    &service_status::notifications_enabled,
    NDO_DATA_NOTIFICATIONSENABLED,
    "notify"),
  mapped_data<service_status>(
    &service_status::obsess_over,
    NDO_DATA_OBSESSOVERSERVICE,
    "obsess_over_service"),
  mapped_data<service_status>(
    &service_status::passive_checks_enabled,
    NDO_DATA_PASSIVESERVICECHECKSENABLED,
    "passive_checks"),
  mapped_data<service_status>(
    &service_status::percent_state_change,
    NDO_DATA_PERCENTSTATECHANGE,
    "percent_state_change"),
  mapped_data<service_status>(
    &service_status::problem_has_been_acknowledged,
    NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
    "acknowledged"),
  mapped_data<service_status>(
    &service_status::process_performance_data,
    NDO_DATA_PROCESSPERFORMANCEDATA,
    "process_perfdata"),
  mapped_data<service_status>(
    &service_status::retry_interval,
    NDO_DATA_RETRYCHECKINTERVAL,
    "retry_interval"),
  mapped_data<service_status>(
    &service_status::scheduled_downtime_depth,
    NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
    "scheduled_downtime_depth"),
  mapped_data<service_status>(
    &service_status::service_id,
    NDO_DATA_SERVICE,
    "service_id",
    true),
  mapped_data<service_status>(
    &service_status::should_be_scheduled,
    NDO_DATA_SHOULDBESCHEDULED,
    "should_be_scheduled"),
  mapped_data<service_status>(
    &service_status::state_type,
    NDO_DATA_STATETYPE,
    "state_type"),
  mapped_data<service_status>(
    &service_status::check_command,
    NDO_DATA_CHECKCOMMAND,
    "check_command"),
  mapped_data<service_status>(
    &service_status::output,
    NDO_DATA_OUTPUT,
    "output"),
  mapped_data<service_status>(
    &service_status::perf_data,
    NDO_DATA_PERFDATA,
    "perfdata"),
  mapped_data<service_status>()
};

// acknowledgement mapping.
template <> const mapped_data<events::acknowledgement>*
  mapped_type<events::acknowledgement>::members(acknowledgement_mapping);
template <> const char*
  mapped_type<events::acknowledgement>::table("acknowledgements");

// Comment mapping.
template <> const mapped_data<events::comment>*
  mapped_type<events::comment>::members(comment_mapping);
template <> const char*
  mapped_type<events::comment>::table("comments");

// Custom variable mapping.
template <> const mapped_data<events::custom_variable>*
  mapped_type<events::custom_variable>::members(custom_variable_mapping);
template <> char const*
  mapped_type<events::custom_variable>::table("customvariables");

// Custom variable status mapping.
template <> const mapped_data<events::custom_variable_status>*
  mapped_type<events::custom_variable_status>::members(custom_variable_status_mapping);
template <> char const*
  mapped_type<events::custom_variable_status>::table("customvariables");

// downtime mapping.
template <> const mapped_data<events::downtime>*
  mapped_type<events::downtime>::members(downtime_mapping);
template <> const char*
  mapped_type<events::downtime>::table("downtimes");

// Event handler mapping.
template <> const mapped_data<events::event_handler>*
  mapped_type<events::event_handler>::members(event_handler_mapping);
template <> const char*
  mapped_type<events::event_handler>::table("eventhandlers");

// Flapping status mapping.
template <> const mapped_data<events::flapping_status>*
  mapped_type<events::flapping_status>::members(flapping_status_mapping);
template <> char const*
  mapped_type<events::flapping_status>::table("flappingstatuses");

// host mapping.
template <> const mapped_data<events::host>*
  mapped_type<events::host>::members(host_mapping);
template <> const char*
  mapped_type<events::host>::table("hosts");

// host_check mapping.
template <> const mapped_data<events::host_check>*
  mapped_type<events::host_check>::members(host_check_mapping);
template <> const char*
  mapped_type<events::host_check>::table("hosts");

// host_dependency mapping.
template <> const mapped_data<events::host_dependency>*
  mapped_type<events::host_dependency>::members(host_dependency_mapping);
template <> const char*
  mapped_type<events::host_dependency>::table("hosts_hosts_dependencies");

// host_group mapping.
template <> const mapped_data<events::host_group>*
  mapped_type<events::host_group>::members(host_group_mapping);
template <> const char*
  mapped_type<events::host_group>::table("hostgroups");

// host_group_member mapping.
template <> const mapped_data<events::host_group_member>*
  mapped_type<events::host_group_member>::members(host_group_member_mapping);
template <> const char*
  mapped_type<events::host_group_member>::table("hosts_hostgroups");

// host_parent mapping.
template <> const mapped_data<events::host_parent>*
  mapped_type<events::host_parent>::members(host_parent_mapping);
template <> const char*
  mapped_type<events::host_parent>::table("hosts_hosts_parents");

// host_state mapping.
template <> const mapped_data<events::host_state>*
  mapped_type<events::host_state>::members(host_state_mapping);
template <> const char*
  mapped_type<events::host_state>::table("hoststateevents");

// host_status mapping.
template <> const mapped_data<events::host_status>*
  mapped_type<events::host_status>::members(host_status_mapping);
template <> const char*
  mapped_type<events::host_status>::table("hosts");

// instance mapping.
template <> const mapped_data<events::instance>*
  mapped_type<events::instance>::members(instance_mapping);
template <> const char*
  mapped_type<events::instance>::table("instances");

// instance_status mapping.
template <> const mapped_data<events::instance_status>*
  mapped_type<events::instance_status>::members(instance_status_mapping);
template <> const char*
  mapped_type<events::instance_status>::table("instances");

// issue mapping.
template <> const mapped_data<events::issue>*
  mapped_type<events::issue>::members(issue_mapping);
template <> const char*
  mapped_type<events::issue>::table("issues");

// issue_parent mapping.
template <> const mapped_data<events::issue_parent>*
  mapped_type<events::issue_parent>::members(issue_parent_mapping);
template <> const char*
  mapped_type<events::issue_parent>::table("issues_issues_parents");

// log_entry mapping.
template <> const mapped_data<events::log_entry>*
  mapped_type<events::log_entry>::members(log_mapping);
template <> const char*
  mapped_type<events::log_entry>::table("logs");

// module mapping.
template <> mapped_data<events::module> const*
  mapped_type<events::module>::members(module_mapping);
template <> char const*
  mapped_type<events::module>::table("modules");

// notification mapping.
template <> mapped_data<events::notification> const*
  mapped_type<events::notification>::members(notification_mapping);
template <> char const*
  mapped_type<events::notification>::table("notifications");

// service mapping.
template <> const mapped_data<events::service>*
  mapped_type<events::service>::members(service_mapping);
template <> const char*
  mapped_type<events::service>::table("services");

// service_check mapping.
template <> const mapped_data<events::service_check>*
  mapped_type<events::service_check>::members(service_check_mapping);
template <> const char*
  mapped_type<events::service_check>::table("services");

// service_dependency mapping.
template <> const mapped_data<events::service_dependency>*
  mapped_type<events::service_dependency>::members(service_dependency_mapping);
template <> const char*
  mapped_type<events::service_dependency>::table("services_services_dependencies");

// service_group mapping.
template <> const mapped_data<events::service_group>*
  mapped_type<events::service_group>::members(service_group_mapping);
template <> const char*
  mapped_type<events::service_group>::table("servicegroups");

// service_group_member mapping.
template <> const mapped_data<events::service_group_member>*
  mapped_type<events::service_group_member>::members(service_group_member_mapping);
template <> const char*
  mapped_type<events::service_group_member>::table("services_servicegroups");

// service_state mapping.
template <> const mapped_data<events::service_state>*
  mapped_type<events::service_state>::members(service_state_mapping);
template <> const char*
  mapped_type<events::service_state>::table("servicestateevents");

// service_status mapping.
template <> const mapped_data<events::service_status>*
  mapped_type<events::service_status>::members(service_status_mapping);
template <> const char*
  mapped_type<events::service_status>::table("services");
