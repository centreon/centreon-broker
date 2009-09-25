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

#include "db/data_member.hpp"
#include "events/acknowledgement.h"
#include "events/comment.h"
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
using namespace CentreonBroker::DB;
using namespace CentreonBroker::Events;

const DB::DataMember<Acknowledgement> CentreonBroker::acknowledgement_dm[] =
  {
    DataMember<Acknowledgement>("acknowledgement_type",
      &Acknowledgement::type),
    DataMember<Acknowledgement>("author_name",
      &Acknowledgement::author),
    DataMember<Acknowledgement>("comment_data",
      &Acknowledgement::comment),
    DataMember<Acknowledgement>("entry_time",
      &Acknowledgement::entry_time),
    DataMember<Acknowledgement>("is_sticky",
      &Acknowledgement::is_sticky),
    DataMember<Acknowledgement>("notify_contacts",
      &Acknowledgement::notify_contacts),
    DataMember<Acknowledgement>("persistent_comment",
      &Acknowledgement::persistent_comment),
    DataMember<Acknowledgement>("state",
      &Acknowledgement::state),
    DataMember<Acknowledgement>()
  };
DB::MappingGetters<Events::Acknowledgement>
  CentreonBroker::acknowledgement_get_mapping;
DB::MappingSetters<Events::Acknowledgement>
  CentreonBroker::acknowledgement_set_mapping;

const DB::DataMember<Comment> CentreonBroker::comment_dm[] =
  {
    DataMember<Comment>("author_name",
      &Comment::author),
    DataMember<Comment>("comment_data",
      &Comment::comment),
    DataMember<Comment>("comment_time",
      &Comment::comment_time),
    DataMember<Comment>("comment_type",
      &Comment::type),
    DataMember<Comment>("deletion_time",
      &Comment::deletion_time),
    DataMember<Comment>("entry_time",
      &Comment::entry_time),
    DataMember<Comment>("entry_type",
      &Comment::entry_type),
    DataMember<Comment>("expire_time",
      &Comment::expire_time),
    DataMember<Comment>("expires",
      &Comment::expires),
    DataMember<Comment>("host_name",
      &Comment::host),
    DataMember<Comment>("internal_comment_id",
      &Comment::internal_id),
    DataMember<Comment>("persistent",
      &Comment::persistent),
    DataMember<Comment>("service_description",
      &Comment::service),
    DataMember<Comment>("source",
      &Comment::source),
    DataMember<Comment>()
  };
DB::MappingGetters<Events::Comment>
  CentreonBroker::comment_get_mapping;
DB::MappingSetters<Events::Comment>
  CentreonBroker::comment_set_mapping;

const DB::DataMember<Downtime> CentreonBroker::downtime_dm[] =
  {
    DataMember<Downtime>("author_name",
      &Downtime::author),
    DataMember<Downtime>("comment_data",
      &Downtime::comment),
    DataMember<Downtime>("downtime_id",
      &Downtime::id),
    DataMember<Downtime>("downtime_type",
      &Downtime::type),
    DataMember<Downtime>("duration",
      &Downtime::duration),
    DataMember<Downtime>("end_time",
      &Downtime::end_time),
    DataMember<Downtime>("entry_time",
      &Downtime::entry_time),
    DataMember<Downtime>("fixed",
      &Downtime::fixed),
    DataMember<Downtime>("start_time",
      &Downtime::start_time),
    DataMember<Downtime>("triggered_by",
      &Downtime::triggered_by),
    DataMember<Downtime>("was_cancelled",
      &Downtime::was_cancelled),
    DataMember<Downtime>("was_started",
      &Downtime::was_started),
    DataMember<Downtime>()
  };
DB::MappingGetters<Events::Downtime>
  CentreonBroker::downtime_get_mapping;
DB::MappingSetters<Events::Downtime>
  CentreonBroker::downtime_set_mapping;

const DB::DataMember<Host> CentreonBroker::host_dm[] =
  {
    DataMember<Host>("acknowledgement_type",
      &Host::acknowledgement_type),
    DataMember<Host>("action_url",
      &Host::action_url),
    DataMember<Host>("active_checks_enabled",
      &Host::active_checks_enabled),
    DataMember<Host>("address",
      &Host::address),
    DataMember<Host>("alias",
      &Host::alias),
    DataMember<Host>("check_command",
      &Host::check_command),
    DataMember<Host>("check_interval",
      &Host::check_interval),
    DataMember<Host>("check_freshness",
      &Host::check_freshness),
    DataMember<Host>("check_period",
      &Host::check_period),
    DataMember<Host>("check_type",
      &Host::check_type),
    DataMember<Host>("current_check_attempt",
      &Host::current_check_attempt),
    DataMember<Host>("current_notification_number",
      &Host::current_notification_number),
    DataMember<Host>("current_state",
      &Host::current_state),
    DataMember<Host>("display_name",
      &Host::display_name),
    DataMember<Host>("event_handler",
      &Host::event_handler),
    DataMember<Host>("event_handler_enabled",
      &Host::event_handler_enabled),
    DataMember<Host>("execution_time",
     &Host::execution_time),
    DataMember<Host>("failure_prediction_enabled",
      &Host::failure_prediction_enabled),
    DataMember<Host>("first_notification_delay",
      &Host::first_notification_delay),
    DataMember<Host>("flap_detection_enabled",
      &Host::flap_detection_enabled),
    DataMember<Host>("flap_detection_on_down",
      &Host::flap_detection_on_down),
    DataMember<Host>("flap_detection_on_unreachable",
      &Host::flap_detection_on_unreachable),
    DataMember<Host>("flap_detection_on_up",
      &Host::flap_detection_on_up),
    DataMember<Host>("freshness_threshold",
      &Host::freshness_threshold),
    DataMember<Host>("has_been_checked",
      &Host::has_been_checked),
    DataMember<Host>("have_2d_coords",
      &Host::have_2d_coords),
    DataMember<Host>("high_flap_threshold",
      &Host::high_flap_threshold),
    DataMember<Host>("host_id",
      &Host::host_id),
    DataMember<Host>("host_name",
      &Host::host),
    DataMember<Host>("icon_image",
      &Host::icon_image),
    DataMember<Host>("icon_image_alt",
      &Host::icon_image_alt),
    DataMember<Host>("is_flapping",
      &Host::is_flapping),
    DataMember<Host>("last_check",
      &Host::last_check),
    DataMember<Host>("last_hard_state",
      &Host::last_hard_state),
    DataMember<Host>("last_hard_state_change",
      &Host::last_hard_state_change),
    DataMember<Host>("last_notification",
      &Host::last_notification),
    DataMember<Host>("last_state_change",
      &Host::last_state_change),
    DataMember<Host>("last_time_down",
      &Host::last_time_down),
    DataMember<Host>("last_time_unreachable",
      &Host::last_time_unreachable),
    DataMember<Host>("last_time_up",
      &Host::last_time_up),
    DataMember<Host>("last_update",
      &Host::last_update),
    DataMember<Host>("latency",
      &Host::latency),
    DataMember<Host>("long_output",
      &Host::long_output),
    DataMember<Host>("low_flap_threshold",
      &Host::low_flap_threshold),
    DataMember<Host>("max_check_attempts",
      &Host::max_check_attempts),
    DataMember<Host>("modified_attributes",
      &Host::modified_attributes),
    DataMember<Host>("next_check",
      &Host::next_check),
    DataMember<Host>("next_host_notification",
      &Host::next_notification),
    DataMember<Host>("no_more_notifications",
      &Host::no_more_notifications),
    DataMember<Host>("notes",
      &Host::notes),
    DataMember<Host>("notes_url",
      &Host::notes_url),
    DataMember<Host>("notification_interval",
      &Host::notification_interval),
    DataMember<Host>("notification_period",
      &Host::notification_period),
    DataMember<Host>("notifications_enabled",
      &Host::notifications_enabled),
    DataMember<Host>("notify_on_down",
      &Host::notify_on_down),
    DataMember<Host>("notify_on_downtime",
      &Host::notify_on_downtime),
    DataMember<Host>("notify_on_flapping",
      &Host::notify_on_flapping),
    DataMember<Host>("notify_on_recovery",
      &Host::notify_on_recovery),
    DataMember<Host>("notify_on_unreachable",
      &Host::notify_on_unreachable),
    DataMember<Host>("obsess_over_host",
      &Host::obsess_over),
    DataMember<Host>("output",
      &Host::output),
    DataMember<Host>("passive_checks_enabled",
      &Host::passive_checks_enabled),
    DataMember<Host>("percent_state_change",
      &Host::percent_state_change),
    DataMember<Host>("perf_data",
      &Host::perf_data),
    DataMember<Host>("problem_has_been_acknowledged",
      &Host::problem_has_been_acknowledged),
    DataMember<Host>("process_performance_data",
      &Host::process_performance_data),
    DataMember<Host>("retain_nonstatus_information",
      &Host::retain_nonstatus_information),
    DataMember<Host>("retain_status_information",
      &Host::retain_status_information),
    DataMember<Host>("retry_interval",
      &Host::retry_interval),
    DataMember<Host>("scheduled_downtime_depth",
      &Host::scheduled_downtime_depth),
    DataMember<Host>("should_be_scheduled",
      &Host::should_be_scheduled),
    DataMember<Host>("stalk_on_down",
      &Host::stalk_on_down),
    DataMember<Host>("stalk_on_unreachable",
      &Host::stalk_on_unreachable),
    DataMember<Host>("stalk_on_up",
      &Host::stalk_on_up),
    DataMember<Host>("state_type",
      &Host::state_type),
    DataMember<Host>("statusmap_image",
      &Host::statusmap_image),
    DataMember<Host>("vrml_image",
      &Host::vrml_image),
    DataMember<Host>("x_2d",
      &Host::x_2d),
    DataMember<Host>("y_2d",
      &Host::y_2d),
    DataMember<Host>()
  };
DB::MappingGetters<Events::Host>
  CentreonBroker::host_get_mapping;
DB::MappingSetters<Events::Host>
  CentreonBroker::host_set_mapping;

const DB::DataMember<HostGroup> CentreonBroker::host_group_dm[] =
  {
    DataMember<HostGroup>("action_url",
      &HostGroup::action_url),
    DataMember<HostGroup>("alias",
      &HostGroup::alias),
    DataMember<HostGroup>("hostgroup_name",
      &HostGroup::name),
    DataMember<HostGroup>("notes",
      &HostGroup::notes),
    DataMember<HostGroup>("notes_url",
      &HostGroup::notes_url),
    DataMember<HostGroup>()
  };
DB::MappingGetters<Events::HostGroup>
  CentreonBroker::host_group_get_mapping;
DB::MappingSetters<Events::HostGroup>
  CentreonBroker::host_group_set_mapping;

const DB::DataMember<HostStatus> CentreonBroker::host_status_dm[] =
  {
    DataMember<HostStatus>("acknowledgement_type",
      &HostStatus::acknowledgement_type),
    DataMember<HostStatus>("active_checks_enabled",
      &HostStatus::active_checks_enabled),
    DataMember<HostStatus>("check_command",
      &HostStatus::check_command),
    DataMember<HostStatus>("check_interval",
      &HostStatus::check_interval),
    DataMember<HostStatus>("check_period",
      &HostStatus::check_period),
    DataMember<HostStatus>("check_type",
      &HostStatus::check_type),
    DataMember<HostStatus>("current_check_attempt",
      &HostStatus::current_check_attempt),
    DataMember<HostStatus>("current_notification_number",
      &HostStatus::current_notification_number),
    DataMember<HostStatus>("current_state",
      &HostStatus::current_state),
    DataMember<HostStatus>("event_handler",
      &HostStatus::event_handler),
    DataMember<HostStatus>("event_handler_enabled",
      &HostStatus::event_handler_enabled),
    DataMember<HostStatus>("execution_time",
      &HostStatus::execution_time),
    DataMember<HostStatus>("failure_prediction_enabled",
      &HostStatus::failure_prediction_enabled),
    DataMember<HostStatus>("flap_detection_enabled",
      &HostStatus::flap_detection_enabled),
    DataMember<HostStatus>("has_been_checked",
      &HostStatus::has_been_checked),
    DataMember<HostStatus>("is_flapping",
      &HostStatus::is_flapping),
    DataMember<HostStatus>("last_check",
      &HostStatus::last_check),
    DataMember<HostStatus>("last_hard_state",
      &HostStatus::last_hard_state),
    DataMember<HostStatus>("last_hard_state_change",
      &HostStatus::last_hard_state_change),
    DataMember<HostStatus>("last_notification",
      &HostStatus::last_notification),
    DataMember<HostStatus>("last_state_change",
      &HostStatus::last_state_change),
    DataMember<HostStatus>("last_time_down",
      &HostStatus::last_time_down),
    DataMember<HostStatus>("last_time_unreachable",
      &HostStatus::last_time_unreachable),
    DataMember<HostStatus>("last_time_up",
      &HostStatus::last_time_up),
    DataMember<HostStatus>("last_update",
      &HostStatus::last_update),
    DataMember<HostStatus>("latency",
      &HostStatus::latency),
    DataMember<HostStatus>("long_output",
      &HostStatus::long_output),
    DataMember<HostStatus>("max_check_attempts",
      &HostStatus::max_check_attempts),
    DataMember<HostStatus>("modified_attributes",
      &HostStatus::modified_attributes),
    DataMember<HostStatus>("next_check",
      &HostStatus::next_check),
    DataMember<HostStatus>("next_host_notification",
      &HostStatus::next_notification),
    DataMember<HostStatus>("no_more_notifications",
      &HostStatus::no_more_notifications),
    DataMember<HostStatus>("notifications_enabled",
      &HostStatus::notifications_enabled),
    DataMember<HostStatus>("obsess_over_host",
      &HostStatus::obsess_over),
    DataMember<HostStatus>("output",
      &HostStatus::output),
    DataMember<HostStatus>("passive_checks_enabled",
      &HostStatus::passive_checks_enabled),
    DataMember<HostStatus>("percent_state_change",
      &HostStatus::percent_state_change),
    DataMember<HostStatus>("perf_data",
      &HostStatus::perf_data),
    DataMember<HostStatus>("problem_has_been_acknowledged",
      &HostStatus::problem_has_been_acknowledged),
    DataMember<HostStatus>("process_performance_data",
      &HostStatus::process_performance_data),
    DataMember<HostStatus>("retry_interval",
      &HostStatus::retry_interval),
    DataMember<HostStatus>("scheduled_downtime_depth",
      &HostStatus::scheduled_downtime_depth),
    DataMember<HostStatus>("should_be_scheduled",
      &HostStatus::should_be_scheduled),
    DataMember<HostStatus>("state_type",
      &HostStatus::state_type),
    DataMember<HostStatus>()
  };
DB::MappingGetters<Events::HostStatus>
  CentreonBroker::host_status_get_mapping;
DB::MappingSetters<Events::HostStatus>
  CentreonBroker::host_status_set_mapping;

const DB::DataMember<Log> CentreonBroker::log_dm[] =
  {
    DataMember<Log>("ctime",
      &Log::c_time),
    DataMember<Log>("host_name",
      &Log::host),
    DataMember<Log>("msg_type",
      &Log::msg_type),
    DataMember<Log>("notification_cmd",
      &Log::notification_cmd),
    DataMember<Log>("notification_contact",
      &Log::notification_contact),
    DataMember<Log>("output",
      &Log::output),
    DataMember<Log>("retry",
      &Log::retry),
    DataMember<Log>("service_description",
      &Log::service),
    DataMember<Log>("status",
      &Log::status),
    DataMember<Log>("type",
      &Log::type),
    DataMember<Log>()
  };
DB::MappingGetters<Events::Log>
  CentreonBroker::log_get_mapping;
DB::MappingSetters<Events::Log>
  CentreonBroker::log_set_mapping;

const DB::DataMember<ProgramStatus> CentreonBroker::program_status_dm[] =
  {
    DataMember<ProgramStatus>("active_host_checks_enabled",
      &ProgramStatus::active_host_checks_enabled),
    DataMember<ProgramStatus>("active_service_checks_enabled",
      &ProgramStatus::active_service_checks_enabled),
    DataMember<ProgramStatus>("daemon_mode",
      &ProgramStatus::daemon_mode),
    DataMember<ProgramStatus>("event_handlers_enabled",
      &ProgramStatus::event_handler_enabled),
    DataMember<ProgramStatus>("failure_prediction_enabled",
      &ProgramStatus::failure_prediction_enabled),
    DataMember<ProgramStatus>("flap_detection_enabled",
      &ProgramStatus::flap_detection_enabled),
    DataMember<ProgramStatus>("global_host_event_handler",
      &ProgramStatus::global_host_event_handler),
    DataMember<ProgramStatus>("global_service_event_handler",
      &ProgramStatus::global_service_event_handler),
    DataMember<ProgramStatus>("is_running",
      &ProgramStatus::is_running),
    DataMember<ProgramStatus>("last_alive",
      &ProgramStatus::last_alive),
    DataMember<ProgramStatus>("last_command_check",
      &ProgramStatus::last_command_check),
    DataMember<ProgramStatus>("last_log_rotation",
      &ProgramStatus::last_log_rotation),
    DataMember<ProgramStatus>("modified_host_attributes",
      &ProgramStatus::modified_host_attributes),
    DataMember<ProgramStatus>("modified_service_attributes",
      &ProgramStatus::modified_service_attributes),
    DataMember<ProgramStatus>("notifications_enabled",
      &ProgramStatus::notifications_enabled),
    DataMember<ProgramStatus>("obsess_over_hosts",
      &ProgramStatus::obsess_over_hosts),
    DataMember<ProgramStatus>("obsess_over_services",
      &ProgramStatus::obsess_over_services),
    DataMember<ProgramStatus>("passive_host_checks_enabled",
      &ProgramStatus::passive_host_checks_enabled),
    DataMember<ProgramStatus>("passive_service_checks_enabled",
      &ProgramStatus::passive_service_checks_enabled),
    DataMember<ProgramStatus>("pid",
      &ProgramStatus::pid),
    DataMember<ProgramStatus>("process_performance_data",
      &ProgramStatus::process_performance_data),
    DataMember<ProgramStatus>("program_end_time",
      &ProgramStatus::program_end),
    DataMember<ProgramStatus>("program_start",
      &ProgramStatus::program_start),
    DataMember<ProgramStatus>()
  };
DB::MappingGetters<Events::ProgramStatus>
  CentreonBroker::program_status_get_mapping;
DB::MappingSetters<Events::ProgramStatus>
  CentreonBroker::program_status_set_mapping;

const DB::DataMember<Service> CentreonBroker::service_dm[] =
  {
    DataMember<Service>("acknowledgement_type",
      &Service::acknowledgement_type),
    DataMember<Service>("action_url",
      &Service::action_url),
    DataMember<Service>("active_checks_enabled",
      &Service::active_checks_enabled),
    DataMember<Service>("check_command",
      &Service::check_command),
    DataMember<Service>("check_interval",
      &Service::check_interval),
    DataMember<Service>("check_freshness",
      &Service::check_freshness),
    DataMember<Service>("check_period",
      &Service::check_period),
    DataMember<Service>("check_type",
      &Service::check_type),
    DataMember<Service>("current_attempt",
      &Service::current_check_attempt),
    DataMember<Service>("current_notification_number",
      &Service::current_notification_number),
    DataMember<Service>("current_state",
      &Service::current_state),
    DataMember<Service>("default_active_checks_enabled",
      &Service::active_checks_enabled),
    DataMember<Service>("default_event_handler_enabled",
      &Service::event_handler_enabled),
    DataMember<Service>("default_failure_prediction_enabled",
      &Service::failure_prediction_enabled),
    DataMember<Service>("default_flap_detection_enabled",
      &Service::flap_detection_enabled),
    DataMember<Service>("default_notifications_enabled",
      &Service::notifications_enabled),
    DataMember<Service>("default_passive_checks_enabled",
      &Service::passive_checks_enabled),
    DataMember<Service>("default_process_performance_data",
      &Service::process_performance_data),
    DataMember<Service>("display_name",
      &Service::display_name),
    DataMember<Service>("event_handler",
      &Service::event_handler),
    DataMember<Service>("event_handler_enabled",
      &Service::event_handler_enabled),
    DataMember<Service>("execution_time",
      &Service::execution_time),
    DataMember<Service>("failure_prediction_enabled",
      &Service::failure_prediction_enabled),
    DataMember<Service>("failure_prediction_options",
      &Service::failure_prediction_options),
    DataMember<Service>("first_notification_delay",
      &Service::first_notification_delay),
    DataMember<Service>("flap_detection_enabled",
      &Service::flap_detection_enabled),
    DataMember<Service>("flap_detection_on_critical",
      &Service::flap_detection_on_critical),
    DataMember<Service>("flap_detection_on_ok",
      &Service::flap_detection_on_ok),
    DataMember<Service>("flap_detection_on_unknown",
      &Service::flap_detection_on_unknown),
    DataMember<Service>("flap_detection_on_warning",
      &Service::flap_detection_on_warning),
    DataMember<Service>("freshness_threshold",
      &Service::freshness_threshold),
    DataMember<Service>("has_been_checked",
      &Service::has_been_checked),
    DataMember<Service>("high_flap_threshold",
      &Service::high_flap_threshold),
    DataMember<Service>("host_id",
      &Service::host_id),
    DataMember<Service>("host_name",
      &Service::host),
    DataMember<Service>("icon_image",
      &Service::icon_image),
    DataMember<Service>("icon_image_alt",
      &Service::icon_image_alt),
    DataMember<Service>("is_flapping",
      &Service::is_flapping),
    DataMember<Service>("is_volatile",
      &Service::is_volatile),
    DataMember<Service>("last_check",
      &Service::last_check),
    DataMember<Service>("last_hard_state",
      &Service::last_hard_state),
    DataMember<Service>("last_hard_state_change",
      &Service::last_hard_state_change),
    DataMember<Service>("last_notification",
      &Service::last_notification),
    DataMember<Service>("last_state_change",
      &Service::last_state_change),
    DataMember<Service>("last_time_critical",
      &Service::last_time_critical),
    DataMember<Service>("last_time_ok",
      &Service::last_time_ok),
    DataMember<Service>("last_time_unknown",
      &Service::last_time_unknown),
    DataMember<Service>("last_time_warning",
      &Service::last_time_warning),
    DataMember<Service>("last_update",
      &Service::last_update),
    DataMember<Service>("latency",
      &Service::latency),
    DataMember<Service>("long_output",
      &Service::long_output),
    DataMember<Service>("low_flap_threshold",
      &Service::low_flap_threshold),
    DataMember<Service>("max_check_attempts",
      &Service::max_check_attempts),
    DataMember<Service>("modified_attributes",
      &Service::modified_attributes),
    DataMember<Service>("next_check",
      &Service::next_check),
    DataMember<Service>("next_notification",
      &Service::next_notification),
    DataMember<Service>("no_more_notifications",
      &Service::no_more_notifications),
    DataMember<Service>("notes",
      &Service::notes),
    DataMember<Service>("notes_url",
      &Service::notes_url),
    DataMember<Service>("notification_interval",
      &Service::notification_interval),
    DataMember<Service>("notification_period",
      &Service::notification_period),
    DataMember<Service>("notifications_enabled",
      &Service::notifications_enabled),
    DataMember<Service>("notified_on_critical",
      &Service::notified_on_critical),
    DataMember<Service>("notified_on_unknown",
      &Service::notified_on_unknown),
    DataMember<Service>("notified_on_warning",
      &Service::notified_on_warning),
    DataMember<Service>("notify_on_downtime",
      &Service::notify_on_downtime),
    DataMember<Service>("notify_on_flapping",
      &Service::notify_on_flapping),
    DataMember<Service>("notify_on_recovery",
      &Service::notify_on_recovery),
    DataMember<Service>("obsess_over_service",
      &Service::obsess_over),
    DataMember<Service>("output",
      &Service::output),
    DataMember<Service>("passive_checks_enabled",
      &Service::passive_checks_enabled),
    DataMember<Service>("percent_state_change",
      &Service::percent_state_change),
    DataMember<Service>("perf_data",
      &Service::perf_data),
    DataMember<Service>("problem_has_been_acknowledged",
      &Service::problem_has_been_acknowledged),
    DataMember<Service>("process_performance_data",
      &Service::process_performance_data),
    DataMember<Service>("retain_nonstatus_information",
      &Service::retain_nonstatus_information),
    DataMember<Service>("retain_status_information",
      &Service::retain_status_information),
    DataMember<Service>("retry_interval",
      &Service::retry_interval),
    DataMember<Service>("scheduled_downtime_depth",
      &Service::scheduled_downtime_depth),
    DataMember<Service>("service_description",
      &Service::service),
    DataMember<Service>("service_id",
      &Service::service_id),
    DataMember<Service>("should_be_scheduled",
      &Service::should_be_scheduled),
    DataMember<Service>("stalk_on_critical",
      &Service::stalk_on_critical),
    DataMember<Service>("stalk_on_ok",
      &Service::stalk_on_ok),
    DataMember<Service>("stalk_on_unknown",
      &Service::stalk_on_unknown),
    DataMember<Service>("stalk_on_warning",
      &Service::stalk_on_warning),
    DataMember<Service>("state_type",
      &Service::state_type),
    DataMember<Service>()
  };
DB::MappingGetters<Events::Service>
  CentreonBroker::service_get_mapping;
DB::MappingSetters<Events::Service>
  CentreonBroker::service_set_mapping;

const DB::DataMember<ServiceStatus> CentreonBroker::service_status_dm[] =
  {
    DataMember<ServiceStatus>("acknowledgement_type",
      &ServiceStatus::acknowledgement_type),
    DataMember<ServiceStatus>("active_checks_enabled",
      &ServiceStatus::active_checks_enabled),
    DataMember<ServiceStatus>("check_command",
      &ServiceStatus::check_command),
    DataMember<ServiceStatus>("check_interval",
      &ServiceStatus::check_interval),
    DataMember<ServiceStatus>("check_period",
      &ServiceStatus::check_period),
    DataMember<ServiceStatus>("check_type",
      &ServiceStatus::check_type),
    DataMember<ServiceStatus>("current_attempt",
      &ServiceStatus::current_check_attempt),
    DataMember<ServiceStatus>("current_notification_number",
      &ServiceStatus::current_notification_number),
    DataMember<ServiceStatus>("current_state",
      &ServiceStatus::current_state),
    DataMember<ServiceStatus>("event_handler",
      &ServiceStatus::event_handler),
    DataMember<ServiceStatus>("event_handler_enabled",
      &ServiceStatus::event_handler_enabled),
    DataMember<ServiceStatus>("execution_time",
      &ServiceStatus::execution_time),
    DataMember<ServiceStatus>("failure_prediction_enabled",
      &ServiceStatus::failure_prediction_enabled),
    DataMember<ServiceStatus>("flap_detection_enabled",
      &ServiceStatus::flap_detection_enabled),
    DataMember<ServiceStatus>("has_been_checked",
      &ServiceStatus::has_been_checked),
    DataMember<ServiceStatus>("is_flapping",
      &ServiceStatus::is_flapping),
    DataMember<ServiceStatus>("last_check",
      &ServiceStatus::last_check),
    DataMember<ServiceStatus>("last_hard_state",
      &ServiceStatus::last_hard_state),
    DataMember<ServiceStatus>("last_hard_state_change",
      &ServiceStatus::last_hard_state_change),
    DataMember<ServiceStatus>("last_notification",
      &ServiceStatus::last_notification),
    DataMember<ServiceStatus>("last_state_change",
      &ServiceStatus::last_state_change),
    DataMember<ServiceStatus>("last_time_critical",
      &ServiceStatus::last_time_critical),
    DataMember<ServiceStatus>("last_time_ok",
      &ServiceStatus::last_time_ok),
    DataMember<ServiceStatus>("last_time_unknown",
      &ServiceStatus::last_time_unknown),
    DataMember<ServiceStatus>("last_time_warning",
      &ServiceStatus::last_time_warning),
    DataMember<ServiceStatus>("last_update",
      &ServiceStatus::last_update),
    DataMember<ServiceStatus>("latency",
      &ServiceStatus::latency),
    DataMember<ServiceStatus>("long_output",
      &ServiceStatus::long_output),
    DataMember<ServiceStatus>("max_check_attempts",
      &ServiceStatus::max_check_attempts),
    DataMember<ServiceStatus>("modified_attributes",
      &ServiceStatus::modified_attributes),
    DataMember<ServiceStatus>("next_check",
      &ServiceStatus::next_check),
    DataMember<ServiceStatus>("next_notification",
      &ServiceStatus::next_notification),
    DataMember<ServiceStatus>("no_more_notifications",
      &ServiceStatus::no_more_notifications),
    DataMember<ServiceStatus>("notifications_enabled",
      &ServiceStatus::notifications_enabled),
    DataMember<ServiceStatus>("obsess_over_service",
      &ServiceStatus::obsess_over),
    DataMember<ServiceStatus>("output",
      &ServiceStatus::output),
    DataMember<ServiceStatus>("passive_checks_enabled",
      &ServiceStatus::passive_checks_enabled),
    DataMember<ServiceStatus>("percent_state_change",
      &ServiceStatus::percent_state_change),
    DataMember<ServiceStatus>("perf_data",
      &ServiceStatus::perf_data),
    DataMember<ServiceStatus>("problem_has_been_acknowledged",
      &ServiceStatus::problem_has_been_acknowledged),
    DataMember<ServiceStatus>("process_performance_data",
      &ServiceStatus::process_performance_data),
    DataMember<ServiceStatus>("retry_interval",
      &ServiceStatus::retry_interval),
    DataMember<ServiceStatus>("scheduled_downtime_depth",
      &ServiceStatus::scheduled_downtime_depth),
    DataMember<ServiceStatus>("should_be_scheduled",
      &ServiceStatus::should_be_scheduled),
    DataMember<ServiceStatus>("state_type",
      &ServiceStatus::state_type),
    DataMember<ServiceStatus>()
  };
DB::MappingGetters<Events::ServiceStatus>
  CentreonBroker::service_status_get_mapping;
DB::MappingSetters<Events::ServiceStatus>
  CentreonBroker::service_status_set_mapping;

template <typename T>
void InitMapping(const DB::DataMember<T>* datamembers,
                 DB::MappingGetters<T>& mapping_get,
                 DB::MappingSetters<T>& mapping_set)
{
  for (unsigned int i = 0; datamembers[i].name; i++)
    switch (datamembers[i].type)
      {
       case 'b':
	mapping_get.AddField(datamembers[i].name, datamembers[i].value.b);
        mapping_set.AddField(datamembers[i].name, datamembers[i].value.b);
	break ;
       case 'd':
	mapping_get.AddField(datamembers[i].name, datamembers[i].value.d);
	mapping_set.AddField(datamembers[i].name, datamembers[i].value.d);
	break ;
       case 'i':
	mapping_get.AddField(datamembers[i].name, datamembers[i].value.i);
	mapping_set.AddField(datamembers[i].name, datamembers[i].value.i);
	break ;
       case 's':
	mapping_get.AddField(datamembers[i].name, datamembers[i].value.s);
	mapping_set.AddField(datamembers[i].name, datamembers[i].value.s);
	break ;
       case 'S':
	mapping_get.AddField(datamembers[i].name, datamembers[i].value.S);
	mapping_set.AddField(datamembers[i].name, datamembers[i].value.S);
	break ;
       case 't':
	mapping_get.AddField(datamembers[i].name, datamembers[i].value.t);
	mapping_set.AddField(datamembers[i].name, datamembers[i].value.t);
	break ;
      }
  return ;
}

/**
 *  \brief Destroy Object-Relational mappings.
 *
 *  All mappings previously initialized with MappingsInit() are destroyed and
 *  related memory is free.
 *
 *  \see MappingsInit
 */
void CentreonBroker::MappingsDestroy()
{
#ifndef NDEBUG
  logging.LogDebug("Destroying Acknowledgement mapping ...");
#endif /* !NDEBUG */
  acknowledgement_get_mapping.Clear();
  acknowledgement_set_mapping.Clear();

#ifndef NDEBUG
  logging.LogDebug("Destroying Comment mapping ...");
#endif /* !NDEBUG */
  comment_get_mapping.Clear();
  comment_set_mapping.Clear();

#ifndef NDEBUG
  logging.LogDebug("Destroying Downtime mapping ...");
#endif /* !NDEBUG */
  downtime_get_mapping.Clear();
  downtime_set_mapping.Clear();

#ifndef NDEBUG
  logging.LogDebug("Destroying Host mapping ...");
#endif /* !NDEBUG */
  host_get_mapping.Clear();
  host_set_mapping.Clear();

#ifndef NDEBUG
  logging.LogDebug("Destroying HostGroup mapping ...");
#endif /* !NDEBUG */
  host_group_get_mapping.Clear();
  host_group_set_mapping.Clear();

#ifndef NDEBUG
  logging.LogDebug("Destroying HostStatus mapping ...");
#endif /* !NDEBUG */
  host_status_get_mapping.Clear();
  host_status_set_mapping.Clear();

#ifndef NDEBUG
  logging.LogDebug("Destroying Log mapping ...");
#endif /* !NDEBUG */
  log_get_mapping.Clear();
  log_set_mapping.Clear();

#ifndef NDEBUG
  logging.LogDebug("Destroying ProgramStatus mapping ...");
#endif /* !NDEBUG */
  program_status_get_mapping.Clear();
  program_status_set_mapping.Clear();

#ifndef NDEBUG
  logging.LogDebug("Destroying Service mapping ...");
#endif /* !NDEBUG */
  service_get_mapping.Clear();
  service_set_mapping.Clear();

#ifndef NDEBUG
  logging.LogDebug("Destroying ServiceStatus mapping ...");
#endif /* !NDEBUG */
  service_status_get_mapping.Clear();
  service_status_set_mapping.Clear();

  return ;
}

/**
 *  \brief Initialize Object-Relational mappings for every type of event.
 *
 *  When done, mappings should be cleaned with MappingsDestroy.
 *
 *  \see MappingsDelete
 */
void CentreonBroker::MappingsInit()
{
#ifndef NDEBUG
  logging.LogDebug("Initializing Acknowledgement mapping ...");
#endif /* !NDEBUG */
  InitMapping<Events::Acknowledgement>(acknowledgement_dm,
    acknowledgement_get_mapping,
    acknowledgement_set_mapping);
#ifndef NDEBUG
  logging.LogDebug("Initializing Comment mapping ...");
#endif /* !NDEBUG */
  InitMapping<Events::Comment>(comment_dm,
    comment_get_mapping,
    comment_set_mapping);
#ifndef NDEBUG
  logging.LogDebug("Initializing Downtime mapping ...");
#endif /* !NDEBUG */
  InitMapping<Events::Downtime>(downtime_dm,
    downtime_get_mapping,
    downtime_set_mapping);
#ifndef NDEBUG
  logging.LogDebug("Initializing Host mapping ...");
#endif /* !NDEBUG */
  InitMapping<Events::Host>(host_dm,
    host_get_mapping,
    host_set_mapping);
#ifndef NDEBUG
  logging.LogDebug("Initializing HostGroup mapping ...");
#endif /* !NDEBUG */
  InitMapping<Events::HostGroup>(host_group_dm,
    host_group_get_mapping,
    host_group_set_mapping);
#ifndef NDEBUG
  logging.LogDebug("Initializing HostStatus mapping ...");
#endif /* !NDEBUG */
  InitMapping<Events::HostStatus>(host_status_dm,
    host_status_get_mapping,
    host_status_set_mapping);
#ifndef NDEBUG
  logging.LogDebug("Initializing Log mapping ...");
#endif /* !NDEBUG */
  InitMapping<Events::Log>(log_dm,
    log_get_mapping,
    log_set_mapping);
#ifndef NDEBUG
  logging.LogDebug("Initializing ProgramStatus mapping ...");
#endif /* !NDEBUG */
  InitMapping<Events::ProgramStatus>(program_status_dm,
    program_status_get_mapping,
    program_status_set_mapping);
#ifndef NDEBUG
  logging.LogDebug("Initializing Service mapping ...");
#endif /* !NDEBUG */
  InitMapping<Events::Service>(service_dm,
    service_get_mapping,
    service_set_mapping);
#ifndef NDEBUG
  logging.LogDebug("Initializing ServiceStatus mapping ...");
#endif /* !NDEBUG */
  InitMapping<Events::ServiceStatus>(service_status_dm,
    service_status_get_mapping,
    service_status_set_mapping);

  return ;
}
