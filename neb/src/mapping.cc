/*
** Copyright 2009-2011,2015 Merethis
**
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
*/

#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/neb/events.hh"

using namespace com::centreon::broker::mapping;
using namespace com::centreon::broker::neb;

// acknowledgement mapping.
entry const acknowledgement_members[] = {
  entry(&acknowledgement::acknowledgement_type, "type"),
  entry(&acknowledgement::author, "author"),
  entry(&acknowledgement::comment, "comment"),
  entry(&acknowledgement::entry_time, "entry_time"),
  entry(&acknowledgement::host_id, "host_id"),
  entry(&acknowledgement::instance_id, "instance_id"),
  entry(&acknowledgement::is_sticky, "is_sticky"),
  entry(&acknowledgement::notify_contacts, "notify_contacts"),
  entry(&acknowledgement::persistent_comment, "persistent_comment"),
  entry(&acknowledgement::service_id, "service_id"),
  entry(&acknowledgement::state, "state"),
  entry()
};

// comment mapping.
entry const comment_members[] = {
  entry(&comment::author, "author"),
  entry(&comment::comment_type, "type"),
  entry(&comment::data, "data"),
  entry(&comment::deletion_time, "deletion_time"),
  entry(&comment::entry_time, "entry_time"),
  entry(&comment::entry_type, "entry_type"),
  entry(&comment::expire_time, "expire_time"),
  entry(&comment::expires, "expires"),
  entry(&comment::host_id, "host_id"),
  entry(&comment::instance_id, "instance_id"),
  entry(&comment::internal_id, "internal_id"),
  entry(&comment::persistent, "persistent"),
  entry(&comment::service_id, "service_id"),
  entry(&comment::source, "source"),
  entry()
};

// custom_variable mapping.
entry const custom_variable_members[] = {
  entry(&custom_variable::host_id, "host_id"),
  entry(&custom_variable::modified, "modified"),
  entry(&custom_variable::name, "name"),
  entry(&custom_variable::service_id, "service_id"),
  entry(&custom_variable::update_time, "update_time"),
  entry(&custom_variable::value, "value"),
  entry(&custom_variable::var_type, "var_type"),
  entry()
};

// custom_variable_status mapping.
entry const custom_variable_status_members[] = {
  entry(&custom_variable_status::host_id, "host_id"),
  entry(&custom_variable_status::modified, "modified"),
  entry(&custom_variable_status::name, "name"),
  entry(&custom_variable_status::service_id, "service_id"),
  entry(&custom_variable_status::update_time, "update_time"),
  entry(&custom_variable_status::value, "value"),
  entry()
};

// downtime mapping.
entry const downtime_members[] = {
  entry(&downtime::author, "author"),
  entry(&downtime::comment, "comment"),
  entry(&downtime::downtime_type, "type"),
  entry(&downtime::duration, "duration"),
  entry(&downtime::end_time, "end_time"),
  entry(&downtime::entry_time, "entry_time"),
  entry(&downtime::fixed, "fixed"),
  entry(&downtime::host_id, "host_id"),
  entry(&downtime::instance_id, "instance_id"),
  entry(&downtime::internal_id, "internal_id"),
  entry(&downtime::service_id, "service_id"),
  entry(&downtime::start_time, "start_time"),
  entry(&downtime::triggered_by, "triggered_by"),
  entry(&downtime::was_cancelled, "was_cancelled"),
  entry(&downtime::was_started, "was_started"),
  entry()
};

// event_handler mapping.
entry const event_handler_members[] = {
  entry(&event_handler::command_args, "command_args"),
  entry(&event_handler::command_line, "command_line"),
  entry(&event_handler::early_timeout, "early_timeout"),
  entry(&event_handler::end_time, "end_time"),
  entry(&event_handler::execution_time, "execution_time"),
  entry(&event_handler::handler_type, "type"),
  entry(&event_handler::host_id, "host_id"),
  entry(&event_handler::output, "output"),
  entry(&event_handler::return_code, "return_code"),
  entry(&event_handler::service_id, "service_id"),
  entry(&event_handler::start_time, "start_time"),
  entry(&event_handler::state, "state"),
  entry(&event_handler::state_type, "state_type"),
  entry(&event_handler::timeout, "timeout"),
  entry()
};

// flapping_status mapping.
entry const flapping_status_members[] = {
  entry(&flapping_status::comment_time, "comment_time"),
  entry(&flapping_status::event_time, "event_time"),
  entry(&flapping_status::event_type, "event_type"),
  entry(&flapping_status::flapping_type, "type"),
  entry(&flapping_status::high_threshold, "high_threshold"),
  entry(&flapping_status::host_id, "host_id"),
  entry(&flapping_status::internal_comment_id, "internal_comment_id"),
  entry(&flapping_status::low_threshold, "low_threshold"),
  entry(&flapping_status::percent_state_change, "percent_state_change"),
  entry(&flapping_status::reason_type, "reason_type"),
  entry(&flapping_status::service_id, "service_id"),
  entry()
};

namespace       com {
  namespace     centreon {
    namespace   broker {
      namespace mapping {
        /**
         *  @class property<host_service>
         *  @brief Specialization of the property class.
         *
         *  One issue with the host_service class, is that it does not
         *  subclass io::data and is therefore incompatible with the
         *  default implementation of property. This is caused by the
         *  templated constructor of property which deduces wrong type.
         */
        template <>
        class   property<host_service> : public property<host> {
         public:
                property(bool (host_service::* b), source::source_type* t)
            : property<host>(b, t) {}
                property(double (host_service::* d), source::source_type* t)
            : property<host>(d, t) {}
                property(short (host_service::* s), source::source_type* t)
            : property<host>(s, t) {}
                property(QString (host_service::* q), source::source_type* t)
            : property<host>(q, t) {}
        };
      }
    }
  }
}

// host mapping.
entry const host_members[] = {
  entry(&host::acknowledgement_type, "acknowledgement_type"),
  entry(&host::action_url, "action_url"),
  entry(&host::active_checks_enabled, "active_checks_enabled"),
  entry(&host::address, "address"),
  entry(&host::alias, "alias"),
  entry(&host::check_command, "check_command"),
  entry(&host::check_freshness, "check_freshness"),
  entry(&host::check_interval, "check_interval"),
  entry(&host::check_period, "check_period"),
  entry(&host::check_type, "check_type"),
  entry(&host::current_check_attempt, "current_check_attempt"),
  entry(&host::current_notification_number, "current_notification_number"),
  entry(&host::current_state, "current_state"),
  entry(&host::default_active_checks_enabled, "default_active_checks_enabled"),
  entry(&host::default_event_handler_enabled, "default_event_handler_enabled"),
  entry(&host::default_failure_prediction, "default_failure_prediction"),
  entry(&host::default_flap_detection_enabled, "default_flap_detection_enabled"),
  entry(&host::default_notifications_enabled, "default_notifications_enabled"),
  entry(&host::default_passive_checks_enabled, "default_passive_checks_enabled"),
  entry(&host::display_name, "display_name"),
  entry(&host::enabled, "enabled"),
  entry(&host::event_handler, "event_handler"),
  entry(&host::event_handler_enabled, "event_handler_enabled"),
  entry(&host::execution_time, "execution_time"),
  entry(&host::failure_prediction_enabled, "failure_prediction_enabled"),
  entry(&host::first_notification_delay, "first_notification_delay"),
  entry(&host::flap_detection_enabled, "flap_detection_enabled"),
  entry(&host::flap_detection_on_down, "flap_detection_on_down"),
  entry(&host::flap_detection_on_unreachable, "flap_detection_on_unreachable"),
  entry(&host::flap_detection_on_up, "flap_detection_on_up"),
  entry(&host::freshness_threshold, "freshness_threshold"),
  entry(&host::has_been_checked, "has_been_checked"),
  entry(&host::high_flap_threshold, "high_flap_threshold"),
  entry(&host::host_id, "host_id"),
  entry(&host::host_name, "host_name"),
  entry(&host::icon_image, "icon_image"),
  entry(&host::icon_image_alt, "icon_image_alt"),
  entry(&host::instance_id, "instance_id"),
  entry(&host::is_flapping, "is_flapping"),
  entry(&host::last_check, "last_check"),
  entry(&host::last_hard_state, "last_hard_state"),
  entry(&host::last_hard_state_change, "last_hard_state_change"),
  entry(&host::last_notification, "last_notification"),
  entry(&host::last_state_change, "last_state_change"),
  entry(&host::last_time_down, "last_time_down"),
  entry(&host::last_time_unreachable, "last_time_unreachable"),
  entry(&host::last_time_up, "last_time_up"),
  entry(&host::last_update, "last_update"),
  entry(&host::latency, "latency"),
  entry(&host::low_flap_threshold, "low_flap_threshold"),
  entry(&host::max_check_attempts, "max_check_attempts"),
  entry(&host::modified_attributes, "modified_attributes"),
  entry(&host::next_check, "next_check"),
  entry(&host::next_notification, "next_notification"),
  entry(&host::no_more_notifications, "no_more_notifications"),
  entry(&host::notes, "notes"),
  entry(&host::notes_url, "notes_url"),
  entry(&host::notification_interval, "notification_interval"),
  entry(&host::notification_period, "notification_period"),
  entry(&host::notifications_enabled, "notifications_enabled"),
  entry(&host::notify_on_down, "notify_on_down"),
  entry(&host::notify_on_downtime, "notify_on_downtime"),
  entry(&host::notify_on_flapping, "notify_on_flapping"),
  entry(&host::notify_on_recovery, "notify_on_recovery"),
  entry(&host::notify_on_unreachable, "notify_on_unreachable"),
  entry(&host::obsess_over, "obsess_over"),
  entry(&host::output, "output"),
  entry(&host::passive_checks_enabled, "passive_checks_enabled"),
  entry(&host::percent_state_change, "percent_state_change"),
  entry(&host::perf_data, "perf_data"),
  entry(&host::problem_has_been_acknowledged, "problem_has_been_acknowledged"),
  entry(&host::retain_nonstatus_information, "retain_nonstatus_information"),
  entry(&host::retain_status_information, "retain_status_information"),
  entry(&host::retry_interval, "retry_interval"),
  entry(&host::scheduled_downtime_depth, "scheduled_downtime_depth"),
  entry(&host::should_be_scheduled, "should_be_scheduled"),
  entry(&host::stalk_on_down, "stalk_on_down"),
  entry(&host::stalk_on_unreachable, "stalk_on_unreachable"),
  entry(&host::stalk_on_up, "stalk_on_up"),
  entry(&host::state_type, "state_type"),
  entry(&host::statusmap_image, "statusmap_image"),
  entry()
};

// host_check mapping.
entry const host_check_members[] = {
  entry(&host_check::command_line, "command_line"),
  entry(&host_check::host_id, "host_id"),
  entry()
};

// host_dependency mapping.
entry const host_dependency_members[] = {
  entry(&host_dependency::dependency_period, "dependency_period"),
  entry(&host_dependency::dependent_host_id, "dependent_host_id"),
  entry(&host_dependency::execution_failure_options, "execution_failure_options"),
  entry(&host_dependency::host_id, "host_id"),
  entry(&host_dependency::inherits_parent, "inherits_parent"),
  entry(&host_dependency::notification_failure_options, "notification_failure_options"),
  entry()
};

// host_group mapping.
entry const host_group_members[] = {
  entry(&host_group::action_url, "action_url"),
  entry(&host_group::alias, "alias"),
  entry(&host_group::instance_id, "instance_id"),
  entry(&host_group::name, "name"),
  entry(&host_group::notes, "notes"),
  entry(&host_group::notes_url, "notes_url"),
  entry()
};

// host_group_member mapping.
entry const host_group_member_members[] = {
  entry(&host_group_member::group, "group"),
  entry(&host_group_member::host_id, "host_id"),
  entry(&host_group_member::instance_id, "instance_id"),
  entry()
};

// host_parent mapping.
entry const host_parent_members[] = {
  entry(&host_parent::host_id, "host_id"),
  entry(&host_parent::parent_id, "parent_id"),
  entry()
};

// host_status mapping.
entry const host_status_members[] = {
  entry(&host_status::acknowledgement_type, "acknowledgement_type"),
  entry(&host_status::active_checks_enabled, "active_checks_enabled"),
  entry(&host_status::check_command, "check_command"),
  entry(&host_status::check_interval, "check_interval"),
  entry(&host_status::check_period, "check_period"),
  entry(&host_status::check_type, "check_type"),
  entry(&host_status::current_check_attempt, "current_check_attempt"),
  entry(&host_status::current_notification_number, "current_notification_number"),
  entry(&host_status::current_state, "current_state"),
  entry(&host_status::enabled, "enabled"),
  entry(&host_status::event_handler, "event_handler"),
  entry(&host_status::event_handler_enabled, "event_handler_enabled"),
  entry(&host_status::execution_time, "execution_time"),
  entry(&host_status::failure_prediction_enabled, "failure_prediction_enabled"),
  entry(&host_status::flap_detection_enabled, "flap_detection_enabled"),
  entry(&host_status::has_been_checked, "has_been_checked"),
  entry(&host_status::host_id, "host_id"),
  entry(&host_status::is_flapping, "is_flapping"),
  entry(&host_status::last_check, "last_check"),
  entry(&host_status::last_hard_state, "last_hard_state"),
  entry(&host_status::last_hard_state_change, "last_hard_state_change"),
  entry(&host_status::last_notification, "last_notification"),
  entry(&host_status::last_state_change, "last_state_change"),
  entry(&host_status::last_time_down, "last_time_down"),
  entry(&host_status::last_time_unreachable, "last_time_unreachable"),
  entry(&host_status::last_time_up, "last_time_up"),
  entry(&host_status::last_update, "last_update"),
  entry(&host_status::latency, "latency"),
  entry(&host_status::max_check_attempts, "max_check_attempts"),
  entry(&host_status::modified_attributes, "modified_attributes"),
  entry(&host_status::next_check, "next_check"),
  entry(&host_status::next_notification, "next_notification"),
  entry(&host_status::no_more_notifications, "no_more_notifications"),
  entry(&host_status::notifications_enabled, "notifications_enabled"),
  entry(&host_status::obsess_over, "obsess_over"),
  entry(&host_status::output, "output"),
  entry(&host_status::passive_checks_enabled, "passive_checks_enabled"),
  entry(&host_status::percent_state_change, "percent_state_change"),
  entry(&host_status::perf_data, "perf_data"),
  entry(&host_status::problem_has_been_acknowledged, "problem_has_been_acknowledged"),
  entry(&host_status::retry_interval, "retry_interval"),
  entry(&host_status::scheduled_downtime_depth, "scheduled_downtime_depth"),
  entry(&host_status::should_be_scheduled, "should_be_scheduled"),
  entry(&host_status::state_type, "state_type"),
  entry()
};

// instance mapping.
entry const instance_members[] = {
  entry(&instance::engine, "engine"),
  entry(&instance::id, "id"),
  entry(&instance::is_running, "is_running"),
  entry(&instance::name, "name"),
  entry(&instance::pid, "pid"),
  entry(&instance::program_end, "program_end"),
  entry(&instance::program_start, "program_start"),
  entry(&instance::version, "version"),
  entry()
};

// instance_status mapping.
entry const instance_status_members[] = {
  entry(&instance_status::active_host_checks_enabled, "active_host_checks_enabled"),
  entry(&instance_status::active_service_checks_enabled, "active_service_checks_enabled"),
  entry(&instance_status::address, "address"),
  entry(&instance_status::check_hosts_freshness, "check_hosts_freshness"),
  entry(&instance_status::check_services_freshness, "check_services_freshness"),
  entry(&instance_status::daemon_mode, "daemon_mode"),
  entry(&instance_status::description, "description"),
  entry(&instance_status::event_handler_enabled, "event_handler_enabled"),
  entry(&instance_status::failure_prediction_enabled, "failure_prediction_enabled"),
  entry(&instance_status::flap_detection_enabled, "flap_detection_enabled"),
  entry(&instance_status::global_host_event_handler, "global_host_event_handler"),
  entry(&instance_status::global_service_event_handler, "global_service_event_handler"),
  entry(&instance_status::id, "id"),
  entry(&instance_status::last_alive, "last_alive"),
  entry(&instance_status::last_command_check, "last_command_check"),
  entry(&instance_status::last_log_rotation, "last_log_rotation"),
  entry(&instance_status::modified_host_attributes, "modified_host_attributes"),
  entry(&instance_status::modified_service_attributes, "modified_service_attributes"),
  entry(&instance_status::notifications_enabled, "notifications_enabled"),
  entry(&instance_status::obsess_over_hosts, "obsess_over_hosts"),
  entry(&instance_status::obsess_over_services, "obsess_over_services"),
  entry(&instance_status::passive_host_checks_enabled, "passive_host_checks_enabled"),
  entry(&instance_status::passive_service_checks_enabled, "passive_service_checks_enabled"),
  entry()
};

// log_entry mapping.
entry const log_entry_members[] = {
  entry(&log_entry::c_time, "ctime"),
  entry(&log_entry::host_id, "host_id"),
  entry(&log_entry::host_name, "host_name"),
  entry(&log_entry::instance_name, "instance_name"),
  entry(&log_entry::issue_start_time, "issue_start_time"),
  entry(&log_entry::log_type, "type"),
  entry(&log_entry::msg_type, "msg_type"),
  entry(&log_entry::notification_cmd, "notification_cmd"),
  entry(&log_entry::notification_contact, "notification_contact"),
  entry(&log_entry::output, "output"),
  entry(&log_entry::retry, "retry"),
  entry(&log_entry::service_description, "service_description"),
  entry(&log_entry::service_id, "service_id"),
  entry(&log_entry::status, "status"),
  entry()
};

// module mapping.
entry const module_members[] = {
  entry(&module::args, "args"),
  entry(&module::filename, "filename"),
  entry(&module::instance_id, "instance_id"),
  entry(&module::loaded, "loaded"),
  entry(&module::should_be_loaded, "should_be_loaded"),
  entry()
};

// notification mapping.
entry const notification_members[] = {
  entry(&notification::ack_author, "ack_author"),
  entry(&notification::ack_data, "ack_data"),
  entry(&notification::command_name, "command_name"),
  entry(&notification::contact_name, "contact_name"),
  entry(&notification::contacts_notified, "contacts_notified"),
  entry(&notification::end_time, "end_time"),
  entry(&notification::escalated, "escalated"),
  entry(&notification::host_id, "host_id"),
  entry(&notification::notification_type, "type"),
  entry(&notification::output, "output"),
  entry(&notification::reason_type, "reason_type"),
  entry(&notification::service_id, "service_id"),
  entry(&notification::start_time, "start_time"),
  entry(&notification::state, "state"),
  entry()
};

// service mapping.
entry const service_members[] = {
  entry(&service::acknowledgement_type, "acknowledgement_type"),
  entry(&service::action_url, "action_url"),
  entry(&service::active_checks_enabled, "active_checks_enabled"),
  entry(&service::check_command, "check_command"),
  entry(&service::check_freshness, "check_freshness"),
  entry(&service::check_interval, "check_interval"),
  entry(&service::check_period, "check_period"),
  entry(&service::check_type, "check_type"),
  entry(&service::current_check_attempt, "current_check_attempt"),
  entry(&service::current_notification_number, "current_notification_number"),
  entry(&service::current_state, "current_state"),
  entry(&service::default_active_checks_enabled, "default_active_checks_enabled"),
  entry(&service::default_event_handler_enabled, "default_event_handler_enabled"),
  entry(&service::default_failure_prediction, "default_failure_prediction"),
  entry(&service::default_flap_detection_enabled, "default_flap_detection_enabled"),
  entry(&service::default_notifications_enabled, "default_notifications_enabled"),
  entry(&service::default_passive_checks_enabled, "default_passive_checks_enabled"),
  entry(&service::display_name, "display_name"),
  entry(&service::enabled, "enabled"),
  entry(&service::event_handler, "event_handler"),
  entry(&service::event_handler_enabled, "event_handler_enabled"),
  entry(&service::execution_time, "execution_time"),
  entry(&service::failure_prediction_enabled, "failure_prediction_enabled"),
  entry(&service::failure_prediction_options, "failure_prediction_options"),
  entry(&service::first_notification_delay, "first_notification_delay"),
  entry(&service::flap_detection_enabled, "flap_detection_enabled"),
  entry(&service::flap_detection_on_ok, "flap_detection_on_ok"),
  entry(&service::flap_detection_on_unknown, "flap_detection_on_unknown"),
  entry(&service::flap_detection_on_warning, "flap_detection_on_warning"),
  entry(&service::freshness_threshold, "freshness_threshold"),
  entry(&service::has_been_checked, "has_been_checked"),
  entry(&service::high_flap_threshold, "high_flap_threshold"),
  entry(&service::host_id, "host_id"),
  entry(&service::icon_image, "icon_image"),
  entry(&service::icon_image_alt, "icon_image_alt"),
  entry(&service::is_flapping, "is_flapping"),
  entry(&service::is_volatile, "is_volatile"),
  entry(&service::last_check, "last_check"),
  entry(&service::last_hard_state, "last_hard_state"),
  entry(&service::last_hard_state_change, "last_hard_state_change"),
  entry(&service::last_notification, "last_notification"),
  entry(&service::last_state_change, "last_state_change"),
  entry(&service::last_time_critical, "last_time_critical"),
  entry(&service::last_time_ok, "last_time_ok"),
  entry(&service::last_time_unknown, "last_time_unknown"),
  entry(&service::last_time_warning, "last_time_warning"),
  entry(&service::last_update, "last_update"),
  entry(&service::latency, "latency"),
  entry(&service::low_flap_threshold, "low_flap_threshold"),
  entry(&service::max_check_attempts, "max_check_attempts"),
  entry(&service::modified_attributes, "modified_attributes"),
  entry(&service::next_check, "next_check"),
  entry(&service::next_notification, "next_notification"),
  entry(&service::no_more_notifications, "no_more_notifications"),
  entry(&service::notes, "notes"),
  entry(&service::notes_url, "notes_url"),
  entry(&service::notification_interval, "notification_interval"),
  entry(&service::notification_period, "notification_period"),
  entry(&service::notifications_enabled, "notifications_enabled"),
  entry(&service::notify_on_critical, "notify_on_critical"),
  entry(&service::notify_on_downtime, "notify_on_downtime"),
  entry(&service::notify_on_flapping, "notify_on_flapping"),
  entry(&service::notify_on_recovery, "notify_on_recovery"),
  entry(&service::notify_on_unknown, "notify_on_unknown"),
  entry(&service::notify_on_warning, "notify_on_warning"),
  entry(&service::obsess_over, "obsess_over"),
  entry(&service::output, "output"),
  entry(&service::passive_checks_enabled, "passive_checks_enabled"),
  entry(&service::percent_state_change, "percent_state_change"),
  entry(&service::perf_data, "perf_data"),
  entry(&service::problem_has_been_acknowledged, "problem_has_been_acknowledged"),
  entry(&service::retain_nonstatus_information, "retain_nonstatus_information"),
  entry(&service::retain_status_information, "retain_status_information"),
  entry(&service::retry_interval, "retry_interval"),
  entry(&service::scheduled_downtime_depth, "scheduled_downtime_depth"),
  entry(&service::service_description, "service_description"),
  entry(&service::service_id, "service_id"),
  entry(&service::should_be_scheduled, "should_be_scheduled"),
  entry(&service::stalk_on_critical, "stalk_on_critical"),
  entry(&service::stalk_on_ok, "stalk_on_ok"),
  entry(&service::stalk_on_unknown, "stalk_on_unknown"),
  entry(&service::stalk_on_warning, "stalk_on_warning"),
  entry(&service::state_type, "state_type"),
  entry()
};

// service_check mapping.
entry const service_check_members[] = {
  entry(&service_check::command_line, "command_line"),
  entry(&service_check::host_id, "host_id"),
  entry(&service_check::service_id, "service_id"),
  entry()
};

// service_dependency mapping.
entry const service_dependency_members[] = {
  entry(&service_dependency::dependency_period, "dependency_period"),
  entry(&service_dependency::dependent_host_id, "dependent_host_id"),
  entry(&service_dependency::dependent_service_id, "dependent_service_id"),
  entry(&service_dependency::execution_failure_options, "execution_failure_options"),
  entry(&service_dependency::host_id, "host_id"),
  entry(&service_dependency::inherits_parent, "inherits_parent"),
  entry(&service_dependency::notification_failure_options, "notification_failure_options"),
  entry(&service_dependency::service_id, "service_id"),
  entry()
};

// service_group mapping.
entry const service_group_members[] = {
  entry(&service_group::action_url, "action_url"),
  entry(&service_group::alias, "alias"),
  entry(&service_group::instance_id, "instance_id"),
  entry(&service_group::name, "name"),
  entry(&service_group::notes, "notes"),
  entry(&service_group::notes_url, "notes_url"),
  entry()
};

// service_group_member mapping.
entry const service_group_member_members[] = {
  entry(&service_group_member::group, "group"),
  entry(&service_group_member::host_id, "host_id"),
  entry(&service_group_member::instance_id, "instance_id"),
  entry(&service_group_member::service_id, "service_id"),
  entry()
};

// service_status mapping.
entry const service_status_members[] = {
  entry(&service_status::acknowledgement_type, "acknowledgement_type"),
  entry(&service_status::active_checks_enabled, "active_checks_enabled"),
  entry(&service_status::check_command, "check_command"),
  entry(&service_status::check_interval, "check_interval"),
  entry(&service_status::check_period, "check_period"),
  entry(&service_status::check_type, "check_type"),
  entry(&service_status::current_check_attempt, "current_check_attempt"),
  entry(&service_status::current_notification_number, "current_notification_number"),
  entry(&service_status::current_state, "current_state"),
  entry(&service_status::enabled, "enabled"),
  entry(&service_status::event_handler, "event_handler"),
  entry(&service_status::event_handler_enabled, "event_handler_enabled"),
  entry(&service_status::execution_time, "execution_time"),
  entry(&service_status::failure_prediction_enabled, "failure_prediction_enabled"),
  entry(&service_status::flap_detection_enabled, "flap_detection_enabled"),
  entry(&service_status::has_been_checked, "has_been_checked"),
  entry(&service_status::host_id, "host_id"),
  entry(&service_status::is_flapping, "is_flapping"),
  entry(&service_status::last_check, "last_check"),
  entry(&service_status::last_hard_state, "last_hard_state"),
  entry(&service_status::last_hard_state_change, "last_hard_state_change"),
  entry(&service_status::last_notification, "last_notification"),
  entry(&service_status::last_state_change, "last_state_change"),
  entry(&service_status::last_time_critical, "last_time_critical"),
  entry(&service_status::last_time_ok, "last_time_ok"),
  entry(&service_status::last_time_unknown, "last_time_unknown"),
  entry(&service_status::last_time_warning, "last_time_warning"),
  entry(&service_status::last_update, "last_update"),
  entry(&service_status::latency, "latency"),
  entry(&service_status::max_check_attempts, "max_check_attempts"),
  entry(&service_status::modified_attributes, "modified_attributes"),
  entry(&service_status::next_check, "next_check"),
  entry(&service_status::next_notification, "next_notification"),
  entry(&service_status::no_more_notifications, "no_more_notifications"),
  entry(&service_status::notifications_enabled, "notifications_enabled"),
  entry(&service_status::obsess_over, "obsess_over"),
  entry(&service_status::output, "output"),
  entry(&service_status::passive_checks_enabled, "passive_checks_enabled"),
  entry(&service_status::percent_state_change, "percent_state_change"),
  entry(&service_status::perf_data, "perf_data"),
  entry(&service_status::problem_has_been_acknowledged, "problem_has_been_acknowledged"),
  entry(&service_status::retry_interval, "retry_interval"),
  entry(&service_status::scheduled_downtime_depth, "scheduled_downtime_depth"),
  entry(&service_status::service_id, "service_id"),
  entry(&service_status::should_be_scheduled, "should_be_scheduled"),
  entry(&service_status::state_type, "state_type"),
  entry()
};
