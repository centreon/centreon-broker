/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include "events/acknowledgement.h"
#include "events/comment.h"
#include "events/connection.h"
#include "events/connection_status.h"
#include "events/host.h"
#include "events/host_group.h"
#include "events/host_status.h"
#include "events/log.h"
#include "events/program_status.h"
#include "events/service.h"
#include "events/service_status.h"
#include "logging.h"
#include "mapping.h"

using namespace CentreonBroker;
using namespace CentreonBroker::Events;

/**
 *  Acknowledgement mapping.
 */
DB::Mapping<Acknowledgement> CentreonBroker::acknowledgement_mapping;

static void InitAcknowledgementMapping()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing Acknowledgement mapping...");
#endif /* !NDEBUG */
  acknowledgement_mapping.SetTable("acknowledgements");
  acknowledgement_mapping.AddShortField("acknowledgement_type",
    &Acknowledgement::type);
  acknowledgement_mapping.AddStringField("author_name",
    &Acknowledgement::author);
  acknowledgement_mapping.AddStringField("comment_data",
    &Acknowledgement::comment);
  acknowledgement_mapping.AddTimeField("entry_time",
    &Acknowledgement::entry_time);
  acknowledgement_mapping.AddShortField("is_sticky",
    &Acknowledgement::is_sticky);
  acknowledgement_mapping.AddShortField("notify_contacts",
    &Acknowledgement::notify_contacts);
  acknowledgement_mapping.AddShortField("persistent_comment",
    &Acknowledgement::persistent_comment);
  acknowledgement_mapping.AddShortField("state",
    &Acknowledgement::state);
  return ;
}

/**
 *  Comment mapping.
 */
DB::Mapping<Comment> CentreonBroker::comment_mapping;

static void InitCommentMapping()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing Comment mapping...");
#endif /* !NDEBUG */
  comment_mapping.SetTable("comment");
  comment_mapping.AddStringField("author_name",
    &Comment::author);
  comment_mapping.AddStringField("comment_data",
    &Comment::comment);
  comment_mapping.AddTimeField("comment_time",
    &Comment::comment_time);
  comment_mapping.AddShortField("comment_type",
    &Comment::type);
  comment_mapping.AddTimeField("deletion_time",
    &Comment::deletion_time);
  comment_mapping.AddTimeField("entry_time",
    &Comment::entry_time);
  comment_mapping.AddShortField("entry_type",
    &Comment::entry_type);
  comment_mapping.AddTimeField("expire_time",
    &Comment::expire_time);
  comment_mapping.AddBoolField("expires",
    &Comment::expires);
  comment_mapping.AddStringField("host_name",
    &Comment::host);
  comment_mapping.AddIntField("internal_comment_id",
    &Comment::internal_id);
  comment_mapping.AddBoolField("persistent",
    &Comment::persistent);
  comment_mapping.AddStringField("service_description",
    &Comment::service);
  comment_mapping.AddShortField("source",
    &Comment::source);
  return ;
}

/**
 *  Connection mapping.
 */
DB::Mapping<Connection> CentreonBroker::connection_mapping;

static void InitConnectionMapping()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing Connection mapping...");
#endif /* !NDEBUG */
  connection_mapping.SetTable("connection_info");
  connection_mapping.AddStringField("agent_name",
    &Connection::agent_name);
  connection_mapping.AddStringField("agent_version",
    &Connection::agent_version);
  connection_mapping.AddIntField("bytes_processed",
    &Connection::bytes_processed);
  connection_mapping.AddStringField("connect_source",
    &Connection::connect_source);
  connection_mapping.AddTimeField("connect_time",
    &Connection::connect_time);
  connection_mapping.AddStringField("connect_type",
    &Connection::connect_type);
  connection_mapping.AddTimeField("data_start_time",
    &Connection::data_start_time);
  connection_mapping.AddIntField("entries_processed",
    &Connection::entries_processed);
  connection_mapping.AddIntField("lines_processed",
    &Connection::lines_processed);
  connection_mapping.AddTimeField("data_end_time",
    &Connection::data_end_time);
  connection_mapping.AddTimeField("disconnect_time",
    &Connection::disconnect_time);
  connection_mapping.AddTimeField("last_checkin_time",
    &Connection::last_checkin_time);
  return ;
}

/**
 *  ConnectionStatus mapping.
 */
DB::Mapping<ConnectionStatus> CentreonBroker::connection_status_mapping;

static void InitConnectionStatusMapping()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing ConnectionStatus mapping...");
#endif /* !NDEBUG */
  connection_status_mapping.SetTable("connection_info");
  connection_status_mapping.AddIntField("bytes_processed",
    &ConnectionStatus::bytes_processed);
  connection_status_mapping.AddTimeField("data_end_time",
    &ConnectionStatus::data_end_time);
  connection_status_mapping.AddTimeField("disconnect_time",
    &ConnectionStatus::disconnect_time);
  connection_status_mapping.AddIntField("entries_processed",
    &ConnectionStatus::entries_processed);
  connection_status_mapping.AddTimeField("last_checkin_time",
    &ConnectionStatus::last_checkin_time);
  connection_status_mapping.AddIntField("lines_processed",
    &ConnectionStatus::lines_processed);
  return ;
}

/**
 *  Downtime mapping.
 */
DB::Mapping<Downtime> CentreonBroker::downtime_mapping;

static void InitDowntimeMapping()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing Downtime mapping...");
#endif /* !NDEBUG */
  downtime_mapping.SetTable("scheduled_downtime");
  downtime_mapping.AddStringField("author_name",
    &Downtime::author);
  downtime_mapping.AddStringField("comment_data",
    &Downtime::comment);
  downtime_mapping.AddIntField("downtime_id",
    &Downtime::id);
  downtime_mapping.AddShortField("downtime_type",
    &Downtime::type);
  downtime_mapping.AddShortField("duration",
    &Downtime::duration);
  downtime_mapping.AddTimeField("end_time",
    &Downtime::end_time);
  downtime_mapping.AddTimeField("entry_time",
    &Downtime::entry_time);
  downtime_mapping.AddBoolField("fixed",
    &Downtime::fixed);
  downtime_mapping.AddTimeField("start_time",
    &Downtime::start_time);
  downtime_mapping.AddIntField("triggered_by",
    &Downtime::triggered_by);
  downtime_mapping.AddBoolField("was_cancelled",
    &Downtime::was_cancelled);
  downtime_mapping.AddBoolField("was_started",
    &Downtime::was_started);
  return ;
}

/**
 *  Host mapping.
 */
DB::Mapping<Host> CentreonBroker::host_mapping;

static void InitHostMapping()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing Host mapping...");
#endif /* !NDEBUG */
  host_mapping.SetTable("host");
  host_mapping.AddShortField("acknowledgement_type",
    &Host::acknowledgement_type);
  host_mapping.AddStringField("action_url",
    &Host::action_url);
  host_mapping.AddBoolField("active_checks_enabled",
    &Host::active_checks_enabled);
  host_mapping.AddStringField("address",
    &Host::address);
  host_mapping.AddStringField("alias",
    &Host::alias);
  host_mapping.AddStringField("check_command",
    &Host::check_command);
  host_mapping.AddDoubleField("check_interval",
    &Host::check_interval);
  host_mapping.AddBoolField("check_freshness",
    &Host::check_freshness);
  host_mapping.AddStringField("check_period",
    &Host::check_period);
  host_mapping.AddShortField("check_type",
    &Host::check_type);
  host_mapping.AddShortField("current_check_attempt",
    &Host::current_check_attempt);
  host_mapping.AddShortField("current_notification_number",
    &Host::current_notification_number);
  host_mapping.AddShortField("current_state",
    &Host::current_state);
  host_mapping.AddStringField("display_name",
    &Host::display_name);
  host_mapping.AddStringField("event_handler",
    &Host::event_handler);
  host_mapping.AddBoolField("event_handler_enabled",
    &Host::event_handler_enabled);
  host_mapping.AddDoubleField("execution_time",
    &Host::execution_time);
  host_mapping.AddBoolField("failure_prediction_enabled",
    &Host::failure_prediction_enabled);
  host_mapping.AddDoubleField("first_notification_delay",
    &Host::first_notification_delay);
  host_mapping.AddBoolField("flap_detection_enabled",
    &Host::flap_detection_enabled);
  host_mapping.AddShortField("flap_detection_on_down",
    &Host::flap_detection_on_down);
  host_mapping.AddShortField("flap_detection_on_unreachable",
    &Host::flap_detection_on_unreachable);
  host_mapping.AddShortField("flap_detection_on_up",
    &Host::flap_detection_on_up);
  host_mapping.AddDoubleField("freshness_threshold",
    &Host::freshness_threshold);
  host_mapping.AddBoolField("has_been_checked",
    &Host::has_been_checked);
  host_mapping.AddShortField("have_2d_coords",
    &Host::have_2d_coords);
  host_mapping.AddDoubleField("high_flap_threshold",
    &Host::high_flap_threshold);
  host_mapping.AddStringField("host_name",
    &Host::host);
  host_mapping.AddStringField("icon_image",
    &Host::icon_image);
  host_mapping.AddStringField("icon_image_alt",
    &Host::icon_image_alt);
  host_mapping.AddBoolField("is_flapping",
    &Host::is_flapping);
  host_mapping.AddTimeField("last_check",
    &Host::last_check);
  host_mapping.AddShortField("last_hard_state",
    &Host::last_hard_state);
  host_mapping.AddTimeField("last_hard_state_change",
    &Host::last_hard_state_change);
  host_mapping.AddTimeField("last_notification",
    &Host::last_notification);
  host_mapping.AddTimeField("last_state_change",
    &Host::last_state_change);
  host_mapping.AddTimeField("last_time_down",
    &Host::last_time_down);
  host_mapping.AddTimeField("last_time_unreachable",
    &Host::last_time_unreachable);
  host_mapping.AddTimeField("last_time_up",
    &Host::last_time_up);
  host_mapping.AddTimeField("last_update",
    &Host::last_update);
  host_mapping.AddDoubleField("latency",
    &Host::latency);
  host_mapping.AddStringField("long_output",
    &Host::long_output);
  host_mapping.AddDoubleField("low_flap_threshold",
    &Host::low_flap_threshold);
  host_mapping.AddShortField("max_check_attempts",
    &Host::max_check_attempts);
  host_mapping.AddIntField("modified_attributes",
    &Host::modified_attributes);
  host_mapping.AddTimeField("next_check",
    &Host::next_check);
  host_mapping.AddTimeField("next_host_notification",
    &Host::next_notification);
  host_mapping.AddBoolField("no_more_notifications",
    &Host::no_more_notifications);
  host_mapping.AddStringField("notes",
    &Host::notes);
  host_mapping.AddStringField("notes_url",
    &Host::notes_url);
  host_mapping.AddDoubleField("notification_interval",
    &Host::notification_interval);
  host_mapping.AddStringField("notification_period",
    &Host::notification_period);
  host_mapping.AddBoolField("notifications_enabled",
    &Host::notifications_enabled);
  host_mapping.AddShortField("notify_on_down",
    &Host::notify_on_down);
  host_mapping.AddShortField("notify_on_downtime",
    &Host::notify_on_downtime);
  host_mapping.AddShortField("notify_on_flapping",
    &Host::notify_on_flapping);
  host_mapping.AddShortField("notify_on_recovery",
    &Host::notify_on_recovery);
  host_mapping.AddShortField("notify_on_unreachable",
    &Host::notify_on_unreachable);
  host_mapping.AddBoolField("obsess_over_host",
    &Host::obsess_over);
  host_mapping.AddStringField("output",
    &Host::output);
  host_mapping.AddBoolField("passive_checks_enabled",
    &Host::passive_checks_enabled);
  host_mapping.AddDoubleField("percent_state_change",
    &Host::percent_state_change);
  host_mapping.AddStringField("perf_data",
    &Host::perf_data);
  host_mapping.AddBoolField("problem_has_been_acknowledged",
    &Host::problem_has_been_acknowledged);
  host_mapping.AddBoolField("process_performance_data",
    &Host::process_performance_data);
  host_mapping.AddBoolField("retain_nonstatus_information",
    &Host::retain_nonstatus_information);
  host_mapping.AddBoolField("retain_status_information",
    &Host::retain_status_information);
  host_mapping.AddDoubleField("retry_interval",
    &Host::retry_interval);
  host_mapping.AddShortField("scheduled_downtime_depth",
    &Host::scheduled_downtime_depth);
  host_mapping.AddBoolField("should_be_scheduled",
    &Host::should_be_scheduled);
  host_mapping.AddShortField("stalk_on_down",
    &Host::stalk_on_down);
  host_mapping.AddShortField("stalk_on_unreachable",
    &Host::stalk_on_unreachable);
  host_mapping.AddShortField("stalk_on_up",
    &Host::stalk_on_up);
  host_mapping.AddShortField("state_type",
    &Host::state_type);
  host_mapping.AddStringField("statusmap_image",
    &Host::statusmap_image);
  host_mapping.AddStringField("vrml_image",
    &Host::vrml_image);
  host_mapping.AddShortField("x_2d",
    &Host::x_2d);
  host_mapping.AddShortField("y_2d",
    &Host::y_2d);
  return ;
}

/**
 *  HostGroup mapping.
 */
DB::Mapping<HostGroup> CentreonBroker::host_group_mapping;

static void InitHostGroupMapping()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing HostGroup mapping...");
#endif /* !NDEBUG */
  host_group_mapping.SetTable("hostgroup");
  host_group_mapping.AddStringField("action_url",
    &HostGroup::action_url);
  host_group_mapping.AddStringField("alias",
    &HostGroup::alias);
  host_group_mapping.AddStringField("hostgroup_name",
    &HostGroup::name);
  host_group_mapping.AddStringField("notes",
    &HostGroup::notes);
  host_group_mapping.AddStringField("notes_url",
    &HostGroup::notes_url);
  return ;
}

/**
 *  HostStatus mapping.
 */
DB::Mapping<HostStatus> CentreonBroker::host_status_mapping;

static void InitHostStatusMapping()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing HostStatus mapping...");
#endif /* !NDEBUG */
  host_status_mapping.SetTable("host");
  host_status_mapping.AddShortField("acknowledgement_type",
    &HostStatus::acknowledgement_type);
  host_status_mapping.AddBoolField("active_checks_enabled",
    &HostStatus::active_checks_enabled);
  host_status_mapping.AddStringField("check_command",
    &HostStatus::check_command);
  host_status_mapping.AddDoubleField("check_interval",
    &HostStatus::check_interval);
  host_status_mapping.AddStringField("check_period",
    &HostStatus::check_period);
  host_status_mapping.AddShortField("check_type",
    &HostStatus::check_type);
  host_status_mapping.AddShortField("current_check_attempt",
    &HostStatus::current_check_attempt);
  host_status_mapping.AddShortField("current_notification_number",
    &HostStatus::current_notification_number);
  host_status_mapping.AddShortField("current_state",
    &HostStatus::current_state);
  host_status_mapping.AddStringField("event_handler",
    &HostStatus::event_handler);
  host_status_mapping.AddBoolField("event_handler_enabled",
    &HostStatus::event_handler_enabled);
  host_status_mapping.AddDoubleField("execution_time",
    &HostStatus::execution_time);
  host_status_mapping.AddBoolField("failure_prediction_enabled",
    &HostStatus::failure_prediction_enabled);
  host_status_mapping.AddBoolField("flap_detection_enabled",
    &HostStatus::flap_detection_enabled);
  host_status_mapping.AddBoolField("has_been_checked",
    &HostStatus::has_been_checked);
  host_status_mapping.AddBoolField("is_flapping",
    &HostStatus::is_flapping);
  host_status_mapping.AddTimeField("last_check",
    &HostStatus::last_check);
  host_status_mapping.AddShortField("last_hard_state",
    &HostStatus::last_hard_state);
  host_status_mapping.AddTimeField("last_hard_state_change",
    &HostStatus::last_hard_state_change);
  host_status_mapping.AddTimeField("last_notification",
    &HostStatus::last_notification);
  host_status_mapping.AddTimeField("last_state_change",
    &HostStatus::last_state_change);
  host_status_mapping.AddTimeField("last_time_down",
    &HostStatus::last_time_down);
  host_status_mapping.AddTimeField("last_time_unreachable",
    &HostStatus::last_time_unreachable);
  host_status_mapping.AddTimeField("last_time_up",
    &HostStatus::last_time_up);
  host_status_mapping.AddTimeField("last_update",
    &HostStatus::last_update);
  host_status_mapping.AddDoubleField("latency",
    &HostStatus::latency);
  host_status_mapping.AddStringField("long_output",
    &HostStatus::long_output);
  host_status_mapping.AddShortField("max_check_attempts",
    &HostStatus::max_check_attempts);
  host_status_mapping.AddIntField("modified_attributes",
    &HostStatus::modified_attributes);
  host_status_mapping.AddTimeField("next_check",
    &HostStatus::next_check);
  host_status_mapping.AddTimeField("next_host_notification",
    &HostStatus::next_notification);
  host_status_mapping.AddBoolField("no_more_notifications",
    &HostStatus::no_more_notifications);
  host_status_mapping.AddBoolField("notifications_enabled",
    &HostStatus::notifications_enabled);
  host_status_mapping.AddBoolField("obsess_over_host",
    &HostStatus::obsess_over);
  host_status_mapping.AddStringField("output",
    &HostStatus::output);
  host_status_mapping.AddBoolField("passive_checks_enabled",
    &HostStatus::passive_checks_enabled);
  host_status_mapping.AddDoubleField("percent_state_change",
    &HostStatus::percent_state_change);
  host_status_mapping.AddStringField("perf_data",
    &HostStatus::perf_data);
  host_status_mapping.AddBoolField("problem_has_been_acknowledged",
    &HostStatus::problem_has_been_acknowledged);
  host_status_mapping.AddBoolField("process_performance_data",
    &HostStatus::process_performance_data);
  host_status_mapping.AddDoubleField("retry_interval",
    &HostStatus::retry_interval);
  host_status_mapping.AddShortField("scheduled_downtime_depth",
    &HostStatus::scheduled_downtime_depth);
  host_status_mapping.AddBoolField("should_be_scheduled",
    &HostStatus::should_be_scheduled);
  host_status_mapping.AddShortField("state_type",
    &HostStatus::state_type);
  return ;
}

DB::Mapping<Log> CentreonBroker::log_mapping;

static void InitLogMapping()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing Log mapping");
#endif /* !NDEBUG */
  log_mapping.AddTimeField("ctime",
    &Log::c_time);
  log_mapping.AddStringField("host_name",
    &Log::host);
  log_mapping.AddIntField("msg_type",
    &Log::msg_type);
  log_mapping.AddStringField("notification_cmd",
    &Log::notification_cmd);
  log_mapping.AddStringField("notification_contact",
    &Log::notification_contact);
  log_mapping.AddStringField("output",
    &Log::output);
  log_mapping.AddIntField("retry",
    &Log::retry);
  log_mapping.AddStringField("service_description",
    &Log::service);
  log_mapping.AddStringField("status",
    &Log::status);
  log_mapping.AddStringField("type",
    &Log::type);
  return ;
}

DB::Mapping<ProgramStatus> CentreonBroker::program_status_mapping;

static void InitProgramStatusMapping()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing ProgramStatus mapping");
#endif /* !NDEBUG */
  program_status_mapping.SetTable("program_status");
  program_status_mapping.AddBoolField("active_host_checks_enabled",
    &ProgramStatus::active_host_checks_enabled);
  program_status_mapping.AddBoolField("active_service_checks_enabled",
    &ProgramStatus::active_service_checks_enabled);
  program_status_mapping.AddBoolField("daemon_mode",
    &ProgramStatus::daemon_mode);
  program_status_mapping.AddBoolField("event_handlers_enabled",
    &ProgramStatus::event_handler_enabled);
  program_status_mapping.AddBoolField("failure_prediction_enabled",
    &ProgramStatus::failure_prediction_enabled);
  program_status_mapping.AddBoolField("flap_detection_enabled",
    &ProgramStatus::flap_detection_enabled);
  program_status_mapping.AddStringField("global_host_event_handler",
    &ProgramStatus::global_host_event_handler);
  program_status_mapping.AddStringField("global_service_event_handler",
    &ProgramStatus::global_service_event_handler);
  program_status_mapping.AddBoolField("is_running",
    &ProgramStatus::is_running);
  program_status_mapping.AddTimeField("last_alive",
    &ProgramStatus::last_alive);
  program_status_mapping.AddTimeField("last_command_check",
    &ProgramStatus::last_command_check);
  program_status_mapping.AddTimeField("last_log_rotation",
    &ProgramStatus::last_log_rotation);
  program_status_mapping.AddIntField("modified_host_attributes",
    &ProgramStatus::modified_host_attributes);
  program_status_mapping.AddIntField("modified_service_attributes",
    &ProgramStatus::modified_service_attributes);
  program_status_mapping.AddBoolField("notifications_enabled",
    &ProgramStatus::notifications_enabled);
  program_status_mapping.AddBoolField("obsess_over_hosts",
    &ProgramStatus::obsess_over_hosts);
  program_status_mapping.AddBoolField("obsess_over_services",
    &ProgramStatus::obsess_over_services);
  program_status_mapping.AddBoolField("passive_host_checks_enabled",
    &ProgramStatus::passive_host_checks_enabled);
  program_status_mapping.AddBoolField("passive_service_checks_enabled",
    &ProgramStatus::passive_service_checks_enabled);
  program_status_mapping.AddIntField("pid",
    &ProgramStatus::pid);
  program_status_mapping.AddBoolField("process_performance_data",
    &ProgramStatus::process_performance_data);
  program_status_mapping.AddTimeField("program_end_time",
    &ProgramStatus::program_end);
  program_status_mapping.AddTimeField("program_start",
    &ProgramStatus::program_start);
  return ;
}

DB::Mapping<Service> CentreonBroker::service_mapping;

static void InitServiceMapping()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing Service mapping...");
#endif /* !NDEBUG */
  service_mapping.SetTable("service");
  service_mapping.AddShortField("acknowledgement_type",
    &Service::acknowledgement_type);
  service_mapping.AddStringField("action_url",
    &Service::action_url);
  service_mapping.AddBoolField("active_checks_enabled",
    &Service::active_checks_enabled);
  service_mapping.AddStringField("check_command",
    &Service::check_command);
  service_mapping.AddDoubleField("check_interval",
    &Service::check_interval);
  service_mapping.AddBoolField("check_freshness",
    &Service::check_freshness);
  service_mapping.AddStringField("check_period",
    &Service::check_period);
  service_mapping.AddShortField("check_type",
    &Service::check_type);
  service_mapping.AddShortField("current_attempt",
    &Service::current_check_attempt);
  service_mapping.AddShortField("current_notification_number",
    &Service::current_notification_number);
  service_mapping.AddShortField("current_state",
    &Service::current_state);
  service_mapping.AddShortField("default_active_checks_enabled",
    &Service::active_checks_enabled);
  service_mapping.AddShortField("default_event_handler_enabled",
    &Service::event_handler_enabled);
  service_mapping.AddShortField("default_failure_prediction_enabled",
    &Service::failure_prediction_enabled);
  service_mapping.AddShortField("default_flap_detection_enabled",
    &Service::flap_detection_enabled);
  service_mapping.AddShortField("default_notifications_enabled",
    &Service::notifications_enabled);
  service_mapping.AddShortField("default_passive_checks_enabled",
    &Service::passive_checks_enabled);
  service_mapping.AddShortField("default_process_performance_data",
    &Service::process_performance_data);
  service_mapping.AddStringField("display_name",
    &Service::display_name);
  service_mapping.AddStringField("event_handler",
    &Service::event_handler);
  service_mapping.AddBoolField("event_handler_enabled",
    &Service::event_handler_enabled);
  service_mapping.AddDoubleField("execution_time",
    &Service::execution_time);
  service_mapping.AddBoolField("failure_prediction_enabled",
    &Service::failure_prediction_enabled);
  service_mapping.AddStringField("failure_prediction_options",
    &Service::failure_prediction_options);
  service_mapping.AddDoubleField("first_notification_delay",
    &Service::first_notification_delay);
  service_mapping.AddBoolField("flap_detection_enabled",
    &Service::flap_detection_enabled);
  service_mapping.AddShortField("flap_detection_on_critical",
    &Service::flap_detection_on_critical);
  service_mapping.AddShortField("flap_detection_on_ok",
    &Service::flap_detection_on_ok);
  service_mapping.AddShortField("flap_detection_on_unknown",
    &Service::flap_detection_on_unknown);
  service_mapping.AddShortField("flap_detection_on_warning",
    &Service::flap_detection_on_warning);
  service_mapping.AddDoubleField("freshness_threshold",
    &Service::freshness_threshold);
  service_mapping.AddBoolField("has_been_checked",
    &Service::has_been_checked);
  service_mapping.AddDoubleField("high_flap_threshold",
    &Service::high_flap_threshold);
  service_mapping.AddStringField("host_name",
    &Service::host);
  service_mapping.AddStringField("icon_image",
    &Service::icon_image);
  service_mapping.AddStringField("icon_image_alt",
    &Service::icon_image_alt);
  service_mapping.AddBoolField("is_flapping",
    &Service::is_flapping);
  service_mapping.AddBoolField("is_volatile",
    &Service::is_volatile);
  service_mapping.AddTimeField("last_check",
    &Service::last_check);
  service_mapping.AddShortField("last_hard_state",
    &Service::last_hard_state);
  service_mapping.AddTimeField("last_hard_state_change",
    &Service::last_hard_state_change);
  service_mapping.AddTimeField("last_notification",
    &Service::last_notification);
  service_mapping.AddTimeField("last_state_change",
    &Service::last_state_change);
  service_mapping.AddTimeField("last_time_critical",
    &Service::last_time_critical);
  service_mapping.AddTimeField("last_time_ok",
    &Service::last_time_ok);
  service_mapping.AddTimeField("last_time_unknown",
    &Service::last_time_unknown);
  service_mapping.AddTimeField("last_time_warning",
    &Service::last_time_warning);
  service_mapping.AddTimeField("last_update",
    &Service::last_update);
  service_mapping.AddDoubleField("latency",
    &Service::latency);
  service_mapping.AddStringField("long_output",
    &Service::long_output);
  service_mapping.AddDoubleField("low_flap_threshold",
    &Service::low_flap_threshold);
  service_mapping.AddShortField("max_check_attempts",
    &Service::max_check_attempts);
  service_mapping.AddIntField("modified_attributes",
    &Service::modified_attributes);
  service_mapping.AddTimeField("next_check",
    &Service::next_check);
  service_mapping.AddTimeField("next_notification",
    &Service::next_notification);
  service_mapping.AddBoolField("no_more_notifications",
    &Service::no_more_notifications);
  service_mapping.AddStringField("notes",
    &Service::notes);
  service_mapping.AddStringField("notes_url",
    &Service::notes_url);
  service_mapping.AddDoubleField("notification_interval",
    &Service::notification_interval);
  service_mapping.AddStringField("notification_period",
    &Service::notification_period);
  service_mapping.AddBoolField("notifications_enabled",
    &Service::notifications_enabled);
  service_mapping.AddBoolField("notified_on_critical",
    &Service::notified_on_critical);
  service_mapping.AddBoolField("notified_on_unknown",
    &Service::notified_on_unknown);
  service_mapping.AddBoolField("notified_on_warning",
    &Service::notified_on_warning);
  service_mapping.AddShortField("notify_on_downtime",
    &Service::notify_on_downtime);
  service_mapping.AddShortField("notify_on_flapping",
    &Service::notify_on_flapping);
  service_mapping.AddShortField("notify_on_recovery",
    &Service::notify_on_recovery);
  service_mapping.AddBoolField("obsess_over_service",
    &Service::obsess_over);
  service_mapping.AddStringField("output",
    &Service::output);
  service_mapping.AddBoolField("passive_checks_enabled",
    &Service::passive_checks_enabled);
  service_mapping.AddDoubleField("percent_state_change",
    &Service::percent_state_change);
  service_mapping.AddStringField("perf_data",
    &Service::perf_data);
  service_mapping.AddBoolField("problem_has_been_acknowledged",
    &Service::problem_has_been_acknowledged);
  service_mapping.AddBoolField("process_performance_data",
    &Service::process_performance_data);
  service_mapping.AddBoolField("retain_nonstatus_information",
    &Service::retain_nonstatus_information);
  service_mapping.AddBoolField("retain_status_information",
    &Service::retain_status_information);
  service_mapping.AddDoubleField("retry_interval",
    &Service::retry_interval);
  service_mapping.AddShortField("scheduled_downtime_depth",
    &Service::scheduled_downtime_depth);
  service_mapping.AddStringField("service_description",
    &Service::service);
  service_mapping.AddBoolField("should_be_scheduled",
    &Service::should_be_scheduled);
  service_mapping.AddShortField("stalk_on_critical",
    &Service::stalk_on_critical);
  service_mapping.AddShortField("stalk_on_ok",
    &Service::stalk_on_ok);
  service_mapping.AddShortField("stalk_on_unknown",
    &Service::stalk_on_unknown);
  service_mapping.AddShortField("stalk_on_warning",
    &Service::stalk_on_warning);
  service_mapping.AddShortField("state_type",
    &Service::state_type);
  return ;
}

DB::Mapping<ServiceStatus> CentreonBroker::service_status_mapping;

static void InitServiceStatusMapping()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing ServiceStatus mapping...");
#endif /* !NDEBUG */
  service_status_mapping.SetTable("service");
  service_status_mapping.AddShortField("acknowledgement_type",
    &ServiceStatus::acknowledgement_type);
  service_status_mapping.AddBoolField("active_checks_enabled",
    &ServiceStatus::active_checks_enabled);
  service_status_mapping.AddStringField("check_command",
    &ServiceStatus::check_command);
  service_status_mapping.AddDoubleField("check_interval",
    &ServiceStatus::check_interval);
  service_status_mapping.AddStringField("check_period",
    &ServiceStatus::check_period);
  service_status_mapping.AddShortField("check_type",
    &ServiceStatus::check_type);
  service_status_mapping.AddShortField("current_attempt",
    &ServiceStatus::current_check_attempt);
  service_status_mapping.AddShortField("current_notification_number",
    &ServiceStatus::current_notification_number);
  service_status_mapping.AddShortField("current_state",
    &ServiceStatus::current_state);
  service_status_mapping.AddStringField("event_handler",
    &ServiceStatus::event_handler);
  service_status_mapping.AddBoolField("event_handler_enabled",
    &ServiceStatus::event_handler_enabled);
  service_status_mapping.AddDoubleField("execution_time",
    &ServiceStatus::execution_time);
  service_status_mapping.AddBoolField("failure_prediction_enabled",
    &ServiceStatus::failure_prediction_enabled);
  service_status_mapping.AddBoolField("flap_detection_enabled",
    &ServiceStatus::flap_detection_enabled);
  service_status_mapping.AddBoolField("has_been_checked",
    &ServiceStatus::has_been_checked);
  service_status_mapping.AddBoolField("is_flapping",
    &ServiceStatus::is_flapping);
  service_status_mapping.AddTimeField("last_check",
    &ServiceStatus::last_check);
  service_status_mapping.AddShortField("last_hard_state",
    &ServiceStatus::last_hard_state);
  service_status_mapping.AddTimeField("last_hard_state_change",
    &ServiceStatus::last_hard_state_change);
  service_status_mapping.AddTimeField("last_notification",
    &ServiceStatus::last_notification);
  service_status_mapping.AddTimeField("last_state_change",
    &ServiceStatus::last_state_change);
  service_status_mapping.AddTimeField("last_time_critical",
    &ServiceStatus::last_time_critical);
  service_status_mapping.AddTimeField("last_time_ok",
    &ServiceStatus::last_time_ok);
  service_status_mapping.AddTimeField("last_time_unknown",
    &ServiceStatus::last_time_unknown);
  service_status_mapping.AddTimeField("last_time_warning",
    &ServiceStatus::last_time_warning);
  service_status_mapping.AddTimeField("last_update",
    &ServiceStatus::last_update);
  service_status_mapping.AddDoubleField("latency",
    &ServiceStatus::latency);
  service_status_mapping.AddStringField("long_output",
    &ServiceStatus::long_output);
  service_status_mapping.AddShortField("max_check_attempts",
    &ServiceStatus::max_check_attempts);
  service_status_mapping.AddIntField("modified_attributes",
    &ServiceStatus::modified_attributes);
  service_status_mapping.AddTimeField("next_check",
    &ServiceStatus::next_check);
  service_status_mapping.AddTimeField("next_notification",
    &ServiceStatus::next_notification);
  service_status_mapping.AddBoolField("no_more_notifications",
    &ServiceStatus::no_more_notifications);
  service_status_mapping.AddBoolField("notifications_enabled",
    &ServiceStatus::notifications_enabled);
  service_status_mapping.AddBoolField("obsess_over_service",
    &ServiceStatus::obsess_over);
  service_status_mapping.AddStringField("output",
    &ServiceStatus::output);
  service_status_mapping.AddBoolField("passive_checks_enabled",
    &ServiceStatus::passive_checks_enabled);
  service_status_mapping.AddDoubleField("percent_state_change",
    &ServiceStatus::percent_state_change);
  service_status_mapping.AddStringField("perf_data",
    &ServiceStatus::perf_data);
  service_status_mapping.AddBoolField("problem_has_been_acknowledged",
    &ServiceStatus::problem_has_been_acknowledged);
  service_status_mapping.AddBoolField("process_performance_data",
    &ServiceStatus::process_performance_data);
  service_status_mapping.AddDoubleField("retry_interval",
    &ServiceStatus::retry_interval);
  service_status_mapping.AddShortField("scheduled_downtime_depth",
    &ServiceStatus::scheduled_downtime_depth);
  service_status_mapping.AddBoolField("should_be_scheduled",
    &ServiceStatus::should_be_scheduled);
  service_status_mapping.AddShortField("state_type",
    &ServiceStatus::state_type);
  return ;
}

void CentreonBroker::InitMappings()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing Object-Relational mappings...", true);
#endif /* !NDEBUG */
  InitAcknowledgementMapping();
  InitCommentMapping();
  InitConnectionMapping();
  InitConnectionStatusMapping();
  InitDowntimeMapping();
  InitHostMapping();
  InitHostGroupMapping();
  InitHostStatusMapping();
  InitLogMapping();
  InitProgramStatusMapping();
  InitServiceMapping();
  InitServiceStatusMapping();
#ifndef NDEBUG
  logging.Deindent();
  logging.LogDebug("Object-Relational mappings initialized");
#endif /* !NDEBUG */
  return ;
}
