/*
** Copyright 2009-2011 Merethis
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

#include <memory>
#include <stdlib.h>
#include <string.h>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/initial.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "nagios/objects.h"

// Internal Nagios host list.
extern "C" {
  extern host* host_list;
  extern hostdependency* hostdependency_list;
  extern hostgroup* hostgroup_list;
  extern service* service_list;
  extern servicedependency* servicedependency_list;
  extern servicegroup* servicegroup_list;
}

using namespace com::centreon::broker;

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Send to the global publisher the list of custom variables.
 */
static void send_custom_variables_list() {
  // Start log message.
  logging::info << logging::MEDIUM
    << "init: beginning custom variables dump";
  time_t now(time(NULL));

  // Iterate through all hosts.
  for (host* h = host_list; h; h = h->next) {
    if (h->name) {
      // Find host ID first.
      unsigned int host_id(neb::gl_hosts[h->name]);

      // Send all custom variables.
      for (customvariablesmember* cv = h->custom_variables; cv; cv = cv->next)
        if (cv->variable_name && strcmp(cv->variable_name, "HOST_ID")) {
          QSharedPointer<neb::custom_variable> c(new neb::custom_variable);
          c->host_id = host_id;
          c->modified = false;
          // c->type = XXX;
          if (cv->variable_name)
            c->name = cv->variable_name;
          c->update_time = now;
          if (cv->variable_value)
            c->value = cv->variable_value;

          // Send custom variable event.
          logging::debug << logging::LOW
            << "init:  new custom variable '" << c->name << "'";
          neb::gl_publisher.write(c.staticCast<io::data>());
        }
    }
  }

  // Iterate through all services.
  for (service* s = service_list; s; s = s->next) {
    if (s->host_name && s->description) {
      // Find host ID and service ID first.
      std::map<std::pair<std::string, std::string>, std::pair<int, int> >::iterator it
        = neb::gl_services.find(std::make_pair(s->host_name, s->description));
      unsigned int host_id(it->second.first);
      unsigned int service_id(it->second.second);

      // Send all custom variables.
      for (customvariablesmember* cv = s->custom_variables; cv; cv = cv->next)
        if (cv->variable_name
            && strcmp(cv->variable_name, "HOST_ID")
            && strcmp(cv->variable_name, "SERVICE_ID")) {
          QSharedPointer<neb::custom_variable> c(new neb::custom_variable);
          c->host_id = host_id;
          c->modified = false;
          c->service_id = service_id;
          // c->type = XXX;
          if (cv->variable_name)
            c->name = cv->variable_name;
          c->update_time = now;
          if (cv->variable_value)
            c->value = cv->variable_value;

          // Send custom variable event.
          logging::debug << logging::LOW
            << "init:  new custom variable '" << c->name << "'";
          neb::gl_publisher.write(c.staticCast<io::data>());
        }
    }
  }

  // End log message.
  logging::info << logging::MEDIUM
    << "init: end of custom variables dump";

  return ;
}

/**
 *  Send to the global publisher the list of host dependencies within Nagios.
 */
static void send_host_dependencies_list() {
  // Start log message.
  logging::info << logging::MEDIUM
    << "init: beginning host dependencies dump";

  // Loop through all dependencies.
  for (hostdependency* hd = hostdependency_list; hd; hd = hd->next) {
    QSharedPointer<neb::host_dependency> host_dependency(
      new neb::host_dependency);
    std::map<std::string, int>::const_iterator it;

    // Set host dependency parameters.
    if (hd->dependency_period)
      host_dependency->dependency_period = hd->dependency_period;
    if (hd->dependent_host_name) {
      it = neb::gl_hosts.find(hd->dependent_host_name);
      if (it != neb::gl_hosts.end())
        host_dependency->dependent_host_id = it->second;
    }
    host_dependency->inherits_parent = hd->inherits_parent;
    if (hd->host_name) {
      it = neb::gl_hosts.find(hd->host_name);
      if (it != neb::gl_hosts.end())
        host_dependency->host_id = it->second;
    }

    // Send host dependency event.
    neb::gl_publisher.write(host_dependency.staticCast<io::data>());
  }

  // End log message.
  logging::info << logging::MEDIUM
    << "init: end of host dependencies dump";

  return ;
}

/**
 *  Send to the global publisher the list of host groups within Nagios.
 */
static void send_host_group_list() {
  // Start log message.
  logging::info << logging::MEDIUM
    << "init: beginning host group dump";

  // Loop through all host groups.
  for (hostgroup* hg = hostgroup_list; hg; hg = hg->next) {
    QSharedPointer<neb::host_group> host_group(new neb::host_group);

    // Set host group parameters.
    if (hg->alias)
      host_group->alias = hg->alias;
    host_group->instance_id = neb::instance_id;
    if (hg->group_name)
      host_group->name = hg->group_name;

    // Send host group event.
    neb::gl_publisher.write(host_group.staticCast<io::data>());

    // Dump host group members.
    for (hostsmember* hgm = hg->members; hgm; hgm = hgm->next) {
      QSharedPointer<neb::host_group_member> host_group_member(
        new neb::host_group_member);

      // Set host group members parameters.
      if (hg->group_name)
        host_group_member->group = hg->group_name;
      host_group_member->instance_id = neb::instance_id;
      if (hgm->host_name) {
        std::map<std::string, int>::const_iterator it;
        it = neb::gl_hosts.find(hgm->host_name);
        if (it != neb::gl_hosts.end())
          host_group_member->host_id = it->second;
      }

      // Send host group member event.
      neb::gl_publisher.write(host_group_member.staticCast<io::data>());
    }
  }

  // End log message.
  logging::info << logging::MEDIUM
    << "init: end of host group dump";

  return ;
}

/**
 *  Send to the global publisher the list of hosts within Nagios.
 */
static void send_host_list() {
  // Start log message.
  logging::info << logging::MEDIUM
    << "init: beginning host dump";

  // Loop through all hosts.
  for (host* h = host_list; h; h = h->next) {
    QSharedPointer<neb::host> my_host(new neb::host);

    // Set host parameters.
    my_host->acknowledgement_type = h->acknowledgement_type;
    if (h->action_url)
      my_host->action_url = h->action_url;
    my_host->active_checks_enabled = h->checks_enabled;
    if (h->address)
      my_host->address = h->address;
    if (h->alias)
      my_host->alias = h->alias;
    my_host->check_freshness = h->check_freshness;
    if (h->host_check_command)
      my_host->check_command = h->host_check_command;
    my_host->check_interval = h->check_interval;
    if (h->check_period)
      my_host->check_period = h->check_period;
    my_host->check_type = h->check_type;
    my_host->current_check_attempt = h->current_attempt;
    my_host->current_notification_number = h->current_notification_number;
    my_host->current_state = (h->has_been_checked
                              ? h->current_state
                              : 4); // Pending state.
    my_host->default_active_checks_enabled = h->checks_enabled;
    my_host->default_event_handler_enabled = h->event_handler_enabled;
    my_host->default_failure_prediction = h->failure_prediction_enabled;
    my_host->default_flap_detection_enabled = h->flap_detection_enabled;
    my_host->default_notifications_enabled = h->notifications_enabled;
    my_host->default_passive_checks_enabled = h->accept_passive_host_checks;
    my_host->default_process_perf_data = h->process_performance_data;
    if (h->display_name)
      my_host->display_name = h->display_name;
    if (h->event_handler)
      my_host->event_handler = h->event_handler;
    my_host->event_handler_enabled = h->event_handler_enabled;
    my_host->execution_time = h->execution_time;
    my_host->failure_prediction_enabled = h->failure_prediction_enabled;
    my_host->first_notification_delay = h->first_notification_delay;
    my_host->flap_detection_enabled = h->flap_detection_enabled;
    my_host->flap_detection_on_down = h->flap_detection_on_down;
    my_host->flap_detection_on_unreachable
      = h->flap_detection_on_unreachable;
    my_host->flap_detection_on_up = h->flap_detection_on_up;
    my_host->freshness_threshold = h->freshness_threshold;
    my_host->has_been_checked = h->has_been_checked;
    my_host->high_flap_threshold = h->high_flap_threshold;
    if (h->name)
      my_host->host_name = h->name;
    if (h->icon_image)
      my_host->icon_image = h->icon_image;
    if (h->icon_image_alt)
      my_host->icon_image_alt = h->icon_image_alt;
    my_host->instance_id = neb::instance_id;
    my_host->is_flapping = h->is_flapping;
    my_host->last_check = h->last_check;
    my_host->last_hard_state = h->last_hard_state;
    my_host->last_hard_state_change = h->last_hard_state_change;
    my_host->last_notification = h->last_host_notification;
    my_host->last_state_change = h->last_state_change;
    my_host->last_time_down = h->last_time_down;
    my_host->last_time_unreachable = h->last_time_unreachable;
    my_host->last_time_up = h->last_time_up;
    my_host->last_update = h->last_state_history_update;
    my_host->latency = h->latency;
    my_host->low_flap_threshold = h->low_flap_threshold;
    my_host->max_check_attempts = h->max_attempts;
    my_host->modified_attributes = h->modified_attributes;
    my_host->next_check = h->next_check;
    my_host->next_notification = h->next_host_notification;
    my_host->no_more_notifications = h->no_more_notifications;
    if (h->notes)
      my_host->notes = h->notes;
    if (h->notes_url)
      my_host->notes_url = h->notes_url;
    my_host->notification_interval = h->notification_interval;
    if (h->notification_period)
      my_host->notification_period = h->notification_period;
    my_host->notifications_enabled = h->notifications_enabled;
    my_host->notify_on_down = h->notify_on_down;
    my_host->notify_on_downtime = h->notify_on_downtime;
    my_host->notify_on_flapping = h->notify_on_flapping;
    my_host->notify_on_recovery = h->notify_on_recovery;
    my_host->notify_on_unreachable = h->notify_on_unreachable;
    my_host->obsess_over = h->obsess_over_host;
    if (h->plugin_output)
      my_host->output = h->plugin_output;
    if (h->long_plugin_output)
        my_host->output.append(h->long_plugin_output);
    my_host->passive_checks_enabled = h->accept_passive_host_checks;
    my_host->percent_state_change = h->percent_state_change;
    if (h->perf_data)
      my_host->perf_data = h->perf_data;
    my_host->problem_has_been_acknowledged = h->problem_has_been_acknowledged;
    my_host->process_performance_data = h->process_performance_data;
    my_host->retain_nonstatus_information = h->retain_nonstatus_information;
    my_host->retain_status_information = h->retain_status_information;
    my_host->retry_interval = h->retry_interval;
    my_host->scheduled_downtime_depth = h->scheduled_downtime_depth;
    my_host->should_be_scheduled = h->should_be_scheduled;
    my_host->stalk_on_down = h->stalk_on_down;
    my_host->stalk_on_unreachable = h->stalk_on_unreachable;
    my_host->stalk_on_up = h->stalk_on_up;
    my_host->state_type = (h->has_been_checked
                           ? h->state_type
                           : HARD_STATE);
    if (h->statusmap_image)
      my_host->statusmap_image = h->statusmap_image;

    // Search host_id through customvars.
    for (customvariablesmember* cv = h->custom_variables; cv; cv = cv->next)
      if (cv->variable_name
          && cv->variable_value
          && !strcmp(cv->variable_name, "HOST_ID")) {
        my_host->host_id = strtol(cv->variable_value, NULL, 0);
        neb::gl_hosts[my_host->host_name.toStdString()] = my_host->host_id;
      }

    // Send host event.
    if (my_host->host_id)
      neb::gl_publisher.write(my_host.staticCast<io::data>());
    else
      logging::error << logging::MEDIUM << "init: host '"
        << (h->name ? h->name : "(unknown)") << "' has no ID defined";
  }

  // End log message.
  logging::info << logging::MEDIUM
    << "init: end of host dump";

  return ;
}

/**
 *  Send to the global publisher the list of host parents within Nagios.
 */
static void send_host_parents_list() {
  // Start log message.
  logging::info << logging::MEDIUM
    << "init: beginning host parents dump";

  // Loop through all hosts.
  int host_id;
  for (host* h = host_list; h; h = h->next) {
    std::map<std::string, int>::const_iterator it;

    // Search host_id.
    if (h->name) {
      it = neb::gl_hosts.find(h->name);
      if (it != neb::gl_hosts.end())
        host_id = it->second;
      else
        host_id = 0;
    }
    else
      host_id = 0;

    // Loop through all dependencies.
    for (hostsmember* parent = h->parent_hosts; parent; parent = parent->next) {
      QSharedPointer<neb::host_parent> hp(new neb::host_parent);
      std::map<std::string, int>::const_iterator it;

      hp->host_id = host_id;
      if (parent->host_name) {
        it = neb::gl_hosts.find(parent->host_name);
        if (it != neb::gl_hosts.end())
          hp->parent_id = it->second;
      }

      // Send host parent event.
      neb::gl_publisher.write(hp.staticCast<io::data>());
    }
  }

  // End log message.
  logging::info << logging::MEDIUM
    << "init: end of host parents dump";

  return ;
}

/**
 *  Send to the global publisher the list of service dependencies within
 *  Nagios.
 */
static void send_service_dependencies_list() {
  // Start log message.
  logging::info << logging::MEDIUM
    << "init: beginning service dependencies dump";

  // Loop through all dependencies.
  for (servicedependency* sd = servicedependency_list; sd; sd = sd->next) {
    std::map<std::pair<std::string, std::string>, std::pair<int, int> >::const_iterator it;
    QSharedPointer<neb::service_dependency> service_dependency(
      new neb::service_dependency);

    // Search IDs.
    if (sd->dependent_host_name && sd->dependent_service_description) {
      it = neb::gl_services.find(std::make_pair<std::string, std::string>(
             sd->dependent_host_name, sd->dependent_service_description));
      if (it != neb::gl_services.end()) {
        service_dependency->dependent_host_id = it->second.first;
        service_dependency->dependent_service_id = it->second.second;
      }
    }
    if (sd->dependency_period)
      service_dependency->dependency_period = sd->dependency_period;
    service_dependency->inherits_parent = sd->inherits_parent;
    if (sd->host_name && sd->service_description) {
      it = neb::gl_services.find(std::make_pair<std::string, std::string>(
             sd->host_name, sd->service_description));
      if (it != neb::gl_services.end()) {
        service_dependency->host_id = it->second.first;
        service_dependency->service_id = it->second.second;
      }
    }

    // Send service dependency event.
    neb::gl_publisher.write(service_dependency.staticCast<io::data>());
  }

  // End log message.
  logging::info << logging::MEDIUM
    << "init: end of service dependencies dump";

  return ;
}

/**
 *  Send to the global publisher the list of service groups within Nagios.
 */
static void send_service_group_list() {
  // Start log message.
  logging::info << logging::MEDIUM
    << "init: beginning service group dump";

  // Loop through all service groups.
  for (servicegroup* sg = servicegroup_list; sg; sg = sg->next) {
    QSharedPointer<neb::service_group> service_group(
      new neb::service_group);

    // Set service group parameters.
    if (sg->alias)
      service_group->alias = sg->alias;
    service_group->instance_id = neb::instance_id;
    if (sg->group_name)
      service_group->name = sg->group_name;

    // Send service group event.
    neb::gl_publisher.write(service_group.staticCast<io::data>());

    // Dump service group members.
    for (servicesmember* sgm = sg->members; sgm; sgm = sgm->next) {
      QSharedPointer<neb::service_group_member> service_group_member(
        new neb::service_group_member);

      // Set service group members parameters.
      if (sg->group_name)
        service_group_member->group = sg->group_name;
      service_group_member->instance_id = neb::instance_id;
      if (sgm->host_name && sgm->service_description) {
        std::map<std::pair<std::string, std::string>, std::pair<int, int> >::const_iterator it;

        it = neb::gl_services.find(std::make_pair(sgm->host_name,
                                             sgm->service_description));
        if (it != neb::gl_services.end()) {
          service_group_member->host_id = it->second.first;
          service_group_member->service_id = it->second.second;
        }
      }

      // Send service group event.
      neb::gl_publisher.write(service_group_member.staticCast<io::data>());
    }
  }

  // End log message.
  logging::info << logging::MEDIUM
    << "init: end of service groups dump";

  return ;
}

/**
 *  Send to the global publisher the list of services within Nagios.
 */
static void send_service_list() {
  // Start log message.
  logging::info << logging::MEDIUM
    << "init: beginning service dump";

  // Loop through all services.
  for (service* s = service_list; s; s = s->next) {
    QSharedPointer<neb::service> my_service(new neb::service);

    // Set service parameters.
    my_service->acknowledgement_type = s->acknowledgement_type;
    if (s->action_url)
      my_service->action_url = s->action_url;
    my_service->active_checks_enabled = s->checks_enabled;
    if (s->service_check_command)
      my_service->check_command = s->service_check_command;
    my_service->check_freshness = s->check_freshness;
    my_service->check_interval = s->check_interval;
    if (s->check_period)
      my_service->check_period = s->check_period;
    my_service->check_type = s->check_type;
    my_service->current_check_attempt = s->current_attempt;
    my_service->current_notification_number = s->current_notification_number;
    my_service->current_state = (s->has_been_checked
                                 ? s->current_state
                                 : 4); // Pending state.
    my_service->default_active_checks_enabled = s->checks_enabled;
    my_service->default_event_handler_enabled = s->event_handler_enabled;
    my_service->default_failure_prediction = s->failure_prediction_enabled;
    my_service->default_flap_detection_enabled = s->flap_detection_enabled;
    my_service->default_notifications_enabled = s->notifications_enabled;
    my_service->default_passive_checks_enabled
      = s->accept_passive_service_checks;
    my_service->default_process_perf_data
      = s->process_performance_data;
    if (s->display_name)
      my_service->display_name = s->display_name;
    if (s->event_handler)
      my_service->event_handler = s->event_handler;
    my_service->event_handler_enabled = s->event_handler_enabled;
    my_service->execution_time = s->execution_time;
    my_service->failure_prediction_enabled = s->failure_prediction_enabled;
    if (s->failure_prediction_options)
      my_service->failure_prediction_options = s->failure_prediction_options;
    my_service->first_notification_delay = s->first_notification_delay;
    my_service->flap_detection_enabled = s->flap_detection_enabled;
    my_service->flap_detection_on_critical = s->flap_detection_on_critical;
    my_service->flap_detection_on_ok = s->flap_detection_on_ok;
    my_service->flap_detection_on_unknown = s->flap_detection_on_unknown;
    my_service->flap_detection_on_warning = s->flap_detection_on_warning;
    my_service->freshness_threshold = s->freshness_threshold;
    my_service->has_been_checked = s->has_been_checked;
    my_service->high_flap_threshold = s->high_flap_threshold;
    if (s->host_name) { // Redonduncy with custom var browsing.
      my_service->host_name = s->host_name;
      std::map<std::string, int>::const_iterator it;
      it = neb::gl_hosts.find(s->host_name);
      if (it != neb::gl_hosts.end())
        my_service->host_id = it->second;
    }
    if (s->icon_image)
      my_service->icon_image = s->icon_image;
    if (s->icon_image_alt)
      my_service->icon_image_alt = s->icon_image_alt;
    my_service->is_flapping = s->is_flapping;
    my_service->is_volatile = s->is_volatile;
    my_service->last_check = s->last_check;
    my_service->last_hard_state = s->last_hard_state;
    my_service->last_hard_state_change = s->last_hard_state_change;
    my_service->last_notification = s->last_notification;
    my_service->last_state_change = s->last_state_change;
    my_service->last_time_critical = s->last_time_critical;
    my_service->last_time_ok = s->last_time_ok;
    my_service->last_time_unknown = s->last_time_unknown;
    my_service->last_time_warning = s->last_time_warning;
    my_service->last_update = time(NULL); // XXX
    my_service->latency = s->latency;
    my_service->low_flap_threshold = s->low_flap_threshold;
    my_service->max_check_attempts = s->max_attempts;
    my_service->modified_attributes = s->modified_attributes;
    my_service->next_check = s->next_check;
    my_service->next_notification = s->next_notification;
    my_service->no_more_notifications = s->no_more_notifications;
    if (s->notes)
      my_service->notes = s->notes;
    if (s->notes_url)
      my_service->notes_url = s->notes_url;
    my_service->notification_interval = s->notification_interval;
    if (s->notification_period)
      my_service->notification_period = s->notification_period;
    my_service->notifications_enabled = s->notifications_enabled;
    my_service->notify_on_critical = s->notify_on_critical;
    my_service->notify_on_downtime = s->notify_on_downtime;
    my_service->notify_on_flapping = s->notify_on_flapping;
    my_service->notify_on_recovery = s->notify_on_recovery;
    my_service->notify_on_unknown = s->notify_on_unknown;
    my_service->notify_on_warning = s->notify_on_warning;
    my_service->obsess_over = s->obsess_over_service;
    if (s->plugin_output)
      my_service->output = s->plugin_output;
    if (s->long_plugin_output)
      my_service->output.append(s->long_plugin_output);
    my_service->passive_checks_enabled = s->accept_passive_service_checks;
    my_service->percent_state_change = s->percent_state_change;
    if (s->perf_data)
      my_service->perf_data = s->perf_data;
    my_service->problem_has_been_acknowledged = s->problem_has_been_acknowledged;
    my_service->process_performance_data = s->process_performance_data;
    my_service->retain_nonstatus_information
      = s->retain_nonstatus_information;
    my_service->retain_status_information = s->retain_status_information;
    my_service->retry_interval = s->retry_interval;
    my_service->scheduled_downtime_depth = s->scheduled_downtime_depth;
    if (s->description)
      my_service->service_description = s->description;
    my_service->should_be_scheduled = s->should_be_scheduled;
    my_service->stalk_on_critical = s->stalk_on_critical;
    my_service->stalk_on_ok = s->stalk_on_ok;
    my_service->stalk_on_unknown = s->stalk_on_unknown;
    my_service->stalk_on_warning = s->stalk_on_warning;
    my_service->state_type = (s->has_been_checked
                              ? s->state_type
                              : HARD_STATE);

    // Search host_id and service_id through customvars.
    for (customvariablesmember* cv = s->custom_variables; cv; cv = cv->next)
      if (cv->variable_name && cv->variable_value) {
        if (!strcmp(cv->variable_name, "HOST_ID"))
          my_service->host_id = strtol(cv->variable_value, NULL, 0);
        else if (!strcmp(cv->variable_name, "SERVICE_ID")) {
          my_service->service_id = strtol(cv->variable_value, NULL, 0);
          neb::gl_services[std::make_pair((s->host_name ? s->host_name : ""),
                                     my_service->service_description.toStdString())]
            = std::make_pair(my_service->host_id, my_service->service_id);
        }
      }

    // Send service event.
    if (my_service->host_id && my_service->service_id)
      neb::gl_publisher.write(my_service.staticCast<io::data>());
    else
      logging::error << logging::HIGH
        << "init: service has no host ID or no service ID (host '"
        << ((s->host_ptr && s->host_ptr->name)
              ? s->host_ptr->name : "(unknown)")
        << "', service '"
        << (s->description ? s->description : "(unknown)")
        << "')";
  }

  // End log message.
  logging::info << logging::MEDIUM
    << "init: end of services dump";

  return ;
}

/**************************************
*                                     *
*          Global Functions           *
*                                     *
**************************************/

/**
 *  Send initial configuration to the global publisher.
 */
void neb::send_initial_configuration() {
  send_host_list();
  send_host_parents_list();
  send_service_list();
  send_host_group_list();
  send_service_group_list();
  send_host_dependencies_list();
  send_service_dependencies_list();
  send_custom_variables_list();
  return ;
}
