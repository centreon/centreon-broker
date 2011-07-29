/*
** Copyright 2009-2011 Merethis
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
#include <set>
#include <time.h>
#include <unistd.h>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/config/logger.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/callbacks.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/initial.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/set_log_data.hh"
#include "nagios/broker.h"
#include "nagios/comments.h"
#include "nagios/common.h"
#include "nagios/nebmodules.h"
#include "nagios/nebstructs.h"
#include "nagios/objects.h"

using namespace com::centreon::broker;

// List of Nagios modules.
extern nebmodule* neb_module_list;

// External function to get program version.
extern "C" {
  char const* get_program_version();
}

/**
 *  @brief Function that process acknowledgement data.
 *
 *  This function is called by Nagios when some acknowledgement data are
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_ACKNOWLEDGEMENT_DATA).
 *  @param[in] data          A pointer to a nebstruct_acknowledgement_data
 *                           containing the acknowledgement data.
 *
 *  @return 0 on success.
 */
int neb::callback_acknowledgement(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM
    << "callbacks: generating acknowledgement event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_acknowledgement_data const* ack_data;
    QSharedPointer<neb::acknowledgement> ack(new neb::acknowledgement);

    // Fill output var.
    ack_data = static_cast<nebstruct_acknowledgement_data*>(data);
    ack->acknowledgement_type = ack_data->acknowledgement_type;
    if (ack_data->author_name)
      ack->author = ack_data->author_name;
    if (ack_data->comment_data)
      ack->comment = ack_data->comment_data;
    ack->entry_time = time(NULL);
    if (ack_data->host_name) {
      std::map<std::string, int>::const_iterator it1;
      it1 = gl_hosts.find(ack_data->host_name);
      if (it1 != gl_hosts.end()) {
        ack->host_id = it1->second;
        if (ack_data->service_description) {
          std::map<std::pair<std::string, std::string>,
                   std::pair<int, int> >::const_iterator it2;
          it2 = gl_services.find(std::make_pair(ack_data->host_name,
            ack_data->service_description));
          if (it2 != gl_services.end())
            ack->service_id = it2->second.second;
        }
      }
    }
    ack->instance_id = instance_id;
    ack->is_sticky = ack_data->is_sticky;
    ack->notify_contacts = ack_data->notify_contacts;
    ack->persistent_comment = ack_data->persistent_comment;
    ack->state = ack_data->state;

    // Send event.
    gl_publisher.write(ack.staticCast<io::data>());
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  @brief Function that process adaptive host data.
 *
 *  This function is called by the monitoring engine when some adaptive
 *  host data is available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_ADAPTIVE_HOST_DATA).
 *  @param[in] data          A pointer to a
 *                           nebstruct_adaptive_host_data.
 */
int neb::callback_adaptive_host(int callback_type, void* data) {
  // Convert data pointer and check that we're interested.
  (void)callback_type;
  logging::debug << logging::LOW << "callbacks: adaptive host data";
  nebstruct_adaptive_host_data* nahd(
    static_cast<nebstruct_adaptive_host_data*>(data));
  if (nahd && (nahd->modified_attribute & MODATTR_CUSTOM_VARIABLE)) {
    try {
      // Log message.
      logging::info << logging::MEDIUM
        << "callbacks: modified host custom variable";

      // Dump host custom variables.
      ::host* h(static_cast< ::host*>(nahd->object_ptr));
      if (h && h->name) {
        // Find host ID.
        unsigned int host_id;
        std::map<std::string, int>::const_iterator id;
        id = gl_hosts.find(h->name);
        if (id != gl_hosts.end()) {
          host_id = id->second;

          // Dump custom variables.
          for (customvariablesmember* it = h->custom_variables;
               it;
               it = it->next)
            if (it->has_been_modified) {
              // Log message.
              logging::debug << logging::LOW
                << "callbacks: dumping host custom variable update";

              // Fill custom variable.
              QSharedPointer<neb::custom_variable_status> cvs(
                new neb::custom_variable_status);
              cvs->host_id = host_id;
              cvs->modified = true;
              if (it->variable_name)
                cvs->name = it->variable_name;
              cvs->service_id = 0;
              cvs->update_time = nahd->timestamp.tv_sec;
              if (it->variable_value)
                cvs->value = it->variable_value;

              // Send event.
              gl_publisher.write(cvs.staticCast<io::data>());
            }
          }
      }
    }
    // Avoid exception propagation in C code.
    catch (...) {}
  }
  return (0);
}

/**
 *  @brief Function that process adaptive service data.
 *
 *  This function is called by the monitoring engine when some adaptive
 *  service data is available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_ADAPTIVE_SERVICE_DATA).
 *  @param[in] data          A pointer to a
 *                           nebstruct_adaptive_service_data.
 */
int neb::callback_adaptive_service(int callback_type, void* data) {
  // Convert data pointer and check that we're interested.
  (void)callback_type;
  logging::debug << logging::LOW << "callbacks: adaptive service data";
  nebstruct_adaptive_service_data* nasd(
    static_cast<nebstruct_adaptive_service_data*>(data));
  if (nasd && (nasd->modified_attribute & MODATTR_CUSTOM_VARIABLE)) {
    try {
      // Log message.
      logging::info << logging::MEDIUM
        << "callbacks: modified service custom variable";

      // Dump service custom variables.
      ::service* s(static_cast< ::service*>(nasd->object_ptr));
      if (s && s->host_name && s->description) {
        // Find host/service IDs.
        unsigned int host_id;
        unsigned int service_id;
        std::map<std::pair<std::string, std::string>,
                 std::pair<int, int> >::const_iterator ids;
        ids = gl_services.find(std::make_pair(s->host_name,
                                              s->description));
        if (ids != gl_services.end()) {
          host_id = ids->second.first;
          service_id = ids->second.second;

          // Dump custom variables.
          for (customvariablesmember* it = s->custom_variables;
               it;
               it = it->next)
            if (it->has_been_modified) {
              // Log message.
              logging::debug << logging::LOW
                << "callbacks: dumping service custom variable update";

              // Fill custom variable.
              QSharedPointer<neb::custom_variable_status> cvs(
                new neb::custom_variable_status);
              cvs->host_id = host_id;
              cvs->modified = true;
              if (it->variable_name)
                cvs->name = it->variable_name;
              cvs->service_id = service_id;
              cvs->update_time = nasd->timestamp.tv_sec;
              if (it->variable_value)
                cvs->value = it->variable_value;

              // Send event.
              gl_publisher.write(cvs.staticCast<io::data>());
            }
          }
      }
      else
        logging::debug << logging::LOW << "callbacks: could not dump " \
          "service custom var because of invalid service pointer, " \
          "invalid host name or invalid service description";
    }
    // Avoid exception propagation in C code.
    catch (...) {}
  }
  return (0);
}

/**
 *  @brief Function that process comment data.
 *
 *  This function is called by Nagios when some comment data are available.
 *
 *  @param[in] callback_type Type of the callback (NEBCALLBACK_COMMENT_DATA).
 *  @param[in] data          A pointer to a nebstruct_comment_data containing
 *                           the comment data.
 *
 *  @return 0 on success.
 */
int neb::callback_comment(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM
    << "callbacks: generating comment event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_comment_data const* comment_data;
    QSharedPointer<neb::comment> comment(new neb::comment);

    // Fill output var.
    comment_data = static_cast<nebstruct_comment_data*>(data);
    if (comment_data->author_name)
      comment->author = comment_data->author_name;
    if (comment_data->comment_data)
      comment->data = comment_data->comment_data;
    comment->comment_type = comment_data->type;
    if (NEBTYPE_COMMENT_DELETE == comment_data->type)
      comment->deletion_time = time(NULL);
    comment->entry_time = time(NULL);
    comment->entry_type = comment_data->entry_type;
    comment->expire_time = comment_data->expire_time;
    comment->expires = comment_data->expires;
    if (comment_data->host_name) {
      std::map<std::string, int>::const_iterator it1;
      it1 = gl_hosts.find(comment_data->host_name);
      if (it1 != gl_hosts.end()) {
        comment->host_id = it1->second;
        if (comment_data->service_description) {
          std::map<std::pair<std::string, std::string>,
                   std::pair<int, int> >::const_iterator it2;
          it2 = gl_services.find(std::make_pair(comment_data->host_name,
            comment_data->service_description));
          if (it2 != gl_services.end())
            comment->service_id = it2->second.second;
        }
      }
    }
    comment->instance_id = instance_id;
    comment->internal_id = comment_data->comment_id;
    comment->persistent = comment_data->persistent;
    comment->source = comment_data->source;

    // Send event.
    gl_publisher.write(comment.staticCast<io::data>());
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  @brief Function that process downtime data.
 *
 *  This function is called by Nagios when some downtime data are available.
 *
 *  @param[in] callback_type Type of the callback (NEBCALLBACK_DOWNTIME_DATA).
 *  @param[in] data          A pointer to a nebstruct_downtime_data containing
 *                           the downtime data.
 *
 *  @return 0 on success.
 */
int neb::callback_downtime(int callback_type, void* data) {
  // Unstarted downtimes.
  std::set<unsigned int> unstarted;

  // Log message.
  logging::info << logging::MEDIUM
    << "callbacks: generating downtime event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_downtime_data const* downtime_data;
    QSharedPointer<neb::downtime> downtime(new neb::downtime);

    // Fill output var.
    downtime_data = static_cast<nebstruct_downtime_data*>(data);
    if (downtime_data->author_name)
      downtime->author = downtime_data->author_name;
    if (downtime_data->comment_data)
      downtime->comment = downtime_data->comment_data;
    downtime->downtime_type = downtime_data->downtime_type;
    downtime->duration = downtime_data->duration;
    downtime->end_time = downtime_data->end_time;
    downtime->entry_time = downtime_data->entry_time;
    downtime->fixed = downtime_data->fixed;
    if (downtime_data->host_name) {
      std::map<std::string, int>::const_iterator it1;
      it1 = gl_hosts.find(downtime_data->host_name);
      if (it1 != gl_hosts.end()) {
        downtime->host_id = it1->second;
        if (downtime_data->service_description) {
          std::map<std::pair<std::string, std::string>,
                   std::pair<int, int> >::const_iterator it2;
          it2 = gl_services.find(std::make_pair(downtime_data->host_name,
            downtime_data->service_description));
          if (it2 != gl_services.end())
            downtime->service_id = it2->second.second;
        }
      }
    }
    downtime->instance_id = instance_id;
    downtime->internal_id = downtime_data->downtime_id;
    downtime->start_time = downtime_data->start_time;
    downtime->triggered_by = downtime_data->triggered_by;
    if ((NEBTYPE_DOWNTIME_ADD == downtime_data->type)
        || (NEBTYPE_DOWNTIME_LOAD == downtime_data->type)) {
      downtime->was_started = true;
      unstarted.insert(downtime_data->downtime_id);
    }
    if ((NEBTYPE_DOWNTIME_START == downtime_data->type)
       || (NEBTYPE_DOWNTIME_STOP == downtime_data->type)) {
      downtime->was_started = true;
      unstarted.erase(downtime_data->downtime_id);
    }
    if (NEBTYPE_DOWNTIME_DELETE == downtime_data->type) {
      downtime->was_cancelled = true;
      std::set<unsigned int>::iterator it;
      it = unstarted.find(downtime_data->downtime_id);
      if (it != unstarted.end()) {
        downtime->was_started = false;
        unstarted.erase(it);
      }
      else
        downtime->was_started = true;
    }

    // Send event.
    gl_publisher.write(downtime.staticCast<io::data>());
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  @brief Function that process event handler data.
 *
 *  This function is called by Nagios when some event handler data are
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_EVENT_HANDLER_DATA).
 *  @param[in] data          A pointer to a nebstruct_event_handler_data
 *                           containing the event handler data.
 *
 *  @return 0 on success.
 */
int neb::callback_event_handler(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM
    << "callbacks: generating event handler event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_event_handler_data const* event_handler_data;
    QSharedPointer<neb::event_handler> event_handler(new neb::event_handler);

    // Fill output var.
    event_handler_data = static_cast<nebstruct_event_handler_data*>(data);
    if (event_handler_data->command_args)
      event_handler->command_args = event_handler_data->command_args;
    if (event_handler_data->command_line)
      event_handler->command_line = event_handler_data->command_line;
    event_handler->early_timeout = event_handler_data->early_timeout;
    event_handler->end_time = event_handler_data->end_time.tv_sec;
    event_handler->execution_time = event_handler_data->execution_time;
    if (event_handler_data->host_name) {
      std::map<std::string, int>::const_iterator it1;
      it1 = gl_hosts.find(event_handler_data->host_name);
      if (it1 != gl_hosts.end()) {
        event_handler->host_id = it1->second;
        if (event_handler_data->service_description) {
          std::map<std::pair<std::string, std::string>,
                   std::pair<int, int> >::const_iterator it2;
          it2 = gl_services.find(std::make_pair(event_handler_data->host_name,
                                                event_handler_data->service_description));
          if (it2 != gl_services.end())
            event_handler->service_id = it2->second.second;
        }
      }
    }
    if (event_handler_data->output)
      event_handler->output = event_handler_data->output;
    event_handler->return_code = event_handler_data->return_code;
    event_handler->start_time = event_handler_data->start_time.tv_sec;
    event_handler->state = event_handler_data->state;
    event_handler->state_type = event_handler_data->state_type;
    event_handler->timeout = event_handler_data->timeout;
    event_handler->handler_type = event_handler_data->eventhandler_type;

    // Send event.
    gl_publisher.write(event_handler.staticCast<io::data>());
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  @brief Function that process flapping status data.
 *
 *  This function is called by Nagios when some flapping status data is
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_FLAPPING_DATA).
 *  @param[in] data          A pointer to a nebstruct_flapping_data
 *                           containing the flapping status data.
 *
 *  @return 0 on success.
 */
int neb::callback_flapping_status(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM
    << "callbacks: generating flapping event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_flapping_data const* flapping_data;
    QSharedPointer<neb::flapping_status> flapping_status(new neb::flapping_status);

    // Fill output var.
    flapping_data = static_cast<nebstruct_flapping_data*>(data);
    flapping_status->event_time = flapping_data->timestamp.tv_sec;
    flapping_status->event_type = flapping_data->type;
    flapping_status->high_threshold = flapping_data->high_threshold;
    if (flapping_data->host_name) {
      std::map<std::string, int>::const_iterator it1;
      it1 = gl_hosts.find(flapping_data->host_name);
      if (it1 != gl_hosts.end()) {
        flapping_status->host_id = it1->second;
        if (flapping_data->service_description) {
          std::map<std::pair<std::string, std::string>,
                   std::pair<int, int> >::const_iterator it2;
          it2 = gl_services.find(std::make_pair(flapping_data->host_name,
                                                flapping_data->service_description));
          if (it2 != gl_services.end())
            flapping_status->service_id = it2->second.second;

          // Set comment time.
          ::comment* com = find_service_comment(flapping_data->comment_id);
          if (com)
            flapping_status->comment_time = com->entry_time;
        }
        else {
          ::comment* com = find_host_comment(flapping_data->comment_id);
          if (com)
            flapping_status->comment_time = com->entry_time;
        }
      }
    }
    flapping_status->internal_comment_id = flapping_data->comment_id;
    flapping_status->low_threshold = flapping_data->low_threshold;
    flapping_status->percent_state_change = flapping_data->percent_change;
    // flapping_status->reason_type = XXX;
    flapping_status->flapping_type = flapping_data->flapping_type;

    // Send event.
    gl_publisher.write(flapping_status.staticCast<io::data>());
  }
  // Avoid exception propagation to C code.
  catch (...) {}
  return (0);
}

/**
 *  @brief Function that process host check data.
 *
 *  This function is called by Nagios when some host check data are available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_HOST_CHECK_DATA).
 *  @param[in] data          A pointer to a nebstruct_host_check_data
 *                           containing the host check data.
 *
 *  @return 0 on success.
 */
int neb::callback_host_check(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM
    << "callbacks: generating host check event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_host_check_data const* hcdata;
    QSharedPointer<neb::host_check> host_check(new neb::host_check);

    // Fill output var.
    hcdata = static_cast<nebstruct_host_check_data*>(data);
    if (hcdata->command_line) {
      host_check->command_line = hcdata->command_line;
      if (hcdata->host_name) {
        std::map<std::string, int>::const_iterator it;
        it = gl_hosts.find(hcdata->host_name);
        if (it != gl_hosts.end())
          host_check->host_id = it->second;
      }

      // Send event.
      gl_publisher.write(host_check.staticCast<io::data>());
    }
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  @brief Function that process host status data.
 *
 *  This function is called by Nagios when some host status data are available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_HOST_STATUS_DATA).
 *  @param[in] data          A pointer to a nebstruct_host_status_data
 *                           containing the host status data.
 *
 *  @return 0 on success.
 */
int neb::callback_host_status(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM
    << "callbacks: generating host status event";
  (void)callback_type;

  try {
    // In/Out variables.
    ::host const* h;
    QSharedPointer<neb::host_status> host_status(new neb::host_status);

    // Fill output var.
    h = static_cast< ::host*>(
      static_cast<nebstruct_host_status_data*>(data)->object_ptr);
    host_status->acknowledgement_type = h->acknowledgement_type;
    host_status->active_checks_enabled = h->checks_enabled;
    if (h->host_check_command)
      host_status->check_command = h->host_check_command;
    host_status->check_interval = h->check_interval;
    if (h->check_period)
      host_status->check_period = h->check_period;
    host_status->check_type = h->check_type;
    host_status->current_check_attempt = h->current_attempt;
    host_status->current_notification_number
      = h->current_notification_number;
    host_status->current_state = h->current_state;
    if (h->event_handler)
      host_status->event_handler = h->event_handler;
    host_status->event_handler_enabled = h->event_handler_enabled;
    host_status->execution_time = h->execution_time;
    host_status->failure_prediction_enabled = h->failure_prediction_enabled;
    host_status->flap_detection_enabled = h->flap_detection_enabled;
    host_status->has_been_checked = h->has_been_checked;
    if (h->name) {
      std::map<std::string, int>::const_iterator it;
      it = gl_hosts.find(h->name);
      if (it != gl_hosts.end())
        host_status->host_id = it->second;
    }
    host_status->is_flapping = h->is_flapping;
    host_status->last_check = h->last_check;
    host_status->last_hard_state = h->last_hard_state;
    host_status->last_hard_state_change = h->last_hard_state_change;
    host_status->last_notification = h->last_host_notification;
    host_status->last_state_change = h->last_state_change;
    host_status->last_time_down = h->last_time_down;
    host_status->last_time_unreachable = h->last_time_unreachable;
    host_status->last_time_up = h->last_time_up;
    host_status->last_update = time(NULL);
    host_status->latency = h->latency;
    host_status->max_check_attempts = h->max_attempts;
    host_status->modified_attributes = h->modified_attributes;
    host_status->next_check = h->next_check;
    host_status->next_notification = h->next_host_notification;
    host_status->no_more_notifications = h->no_more_notifications;
    host_status->notifications_enabled = h->notifications_enabled;
    host_status->obsess_over = h->obsess_over_host;
    if (h->plugin_output)
      host_status->output = h->plugin_output;
    if (h->long_plugin_output)
      host_status->output.append(h->long_plugin_output);
    host_status->passive_checks_enabled = h->accept_passive_host_checks;
    host_status->percent_state_change = h->percent_state_change;
    if (h->perf_data)
      host_status->perf_data = h->perf_data;
    host_status->problem_has_been_acknowledged
      = h->problem_has_been_acknowledged;
    host_status->process_performance_data = h->process_performance_data;
    host_status->retry_interval = h->retry_interval;
    host_status->scheduled_downtime_depth = h->scheduled_downtime_depth;
    host_status->should_be_scheduled = h->should_be_scheduled;
    host_status->state_type = h->state_type;

    // Send event.
    gl_publisher.write(host_status.staticCast<io::data>());
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  @brief Function that process log data.
 *
 *  This function is called by Nagios when some log data are available.
 *
 *  @param[in] callback_type Type of the callback (NEBCALLBACK_LOG_DATA).
 *  @param[in] data          A pointer to a nebstruct_log_data containing the
 *                           log data.
 *
 *  @return 0 on success.
 */
int neb::callback_log(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM << "callbacks: generating log event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_log_data const* log_data;
    QSharedPointer<neb::log_entry> le(new neb::log_entry);

    // Fill output var.
    log_data = static_cast<nebstruct_log_data*>(data);
    le->c_time = log_data->entry_time;
    le->instance_name = instance_name;
    if (log_data->data) {
      if (log_data->data)
        le->output = log_data->data;
      set_log_data(*le, log_data->data);
    }

    // Send event.
    gl_publisher.write(le.staticCast<io::data>());
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  @brief Function that process process data.
 *
 *  This function is called by Nagios when some process data is available.
 *
 *  @param[in] callback_type Type of the callback (NEBCALLBACK_PROCESS_DATA).
 *  @param[in] data          A pointer to a nebstruct_process_data containing
 *                           the process data.
 *
 *  @return 0 on success.
 */
int neb::callback_process(int callback_type, void *data) {
  // Log message.
  logging::debug << logging::LOW << "callbacks: process event callback";
  (void)callback_type;

  try {
    // Input variables.
    nebstruct_process_data const* process_data;
    static time_t start_time;

    // Check process event type.
    process_data = static_cast<nebstruct_process_data*>(data);
    if (NEBTYPE_PROCESS_EVENTLOOPSTART == process_data->type) {
      logging::info << logging::MEDIUM
        << "callbacks: generating process start event";
      // Output variable.
      QSharedPointer<neb::instance> instance(new neb::instance);

      // Logging object.
      config::logger default_log;
      default_log.config(true);
      default_log.debug(false);
      default_log.error(true);
      default_log.info(true);
      default_log.level(logging::HIGH);
      default_log.name("stderr");
      default_log.type(config::logger::standard);

      // Configuration object.
      config::state default_state;
      default_state.loggers().push_back(default_log);

      // Apply configuration.
      config::applier::state::instance().apply(default_state);

      // Parse configuration file.
      try {
        config::parser parsr;
        config::state conf;
        parsr.parse(gl_configuration_file, conf);

        // Apply resulting configuration.
        config::applier::state::instance().apply(conf);

        // Set variables.
        QMap<QString, QString>::const_iterator it;
        it = conf.params().find("instance");
        if (it != conf.params().end())
          instance_id = it.value().toUInt();
        it = conf.params().find("instance_name");
        if (it != conf.params().end())
          instance_name = it.value();
      }
      catch (exceptions::msg const& e) {
        logging::config << logging::HIGH << e.what();
        return (0);
      }

#ifdef PROGRAM_NAME
      instance->engine = PROGRAM_NAME;
#else
      instance->engine = "Nagios";
#endif /* PROGRAM_NAME */
      instance->id = instance_id;
      instance->is_running = true;
      instance->name = instance_name;
      instance->pid = getpid();
      instance->program_start = time(NULL);
      instance->version = get_program_version();
      start_time = instance->program_start;

      // Send initial event and then configuration.
      gl_publisher.write(instance.staticCast<io::data>());
      send_initial_configuration();

      // Generate module list.
      for (nebmodule* nm = neb_module_list; nm; nm = nm->next)
        if (nm->filename) {
          // Output variable.
          QSharedPointer<neb::module> module(new neb::module);

          // Fill output var.
          if (nm->args)
            module->args = nm->args;
          module->filename = nm->filename;
          module->instance_id = instance_id;
          module->loaded = nm->is_currently_loaded;
          module->should_be_loaded = nm->should_be_loaded;

          // Send events.
          gl_publisher.write(module.staticCast<io::data>());
        }
    }
    else if (NEBTYPE_PROCESS_EVENTLOOPEND == process_data->type) {
      logging::info << logging::MEDIUM << "generating process end event";
      // Output variable.
      QSharedPointer<neb::instance> instance(new neb::instance);

      // Fill output var.
#ifdef PROGRAM_NAME
      instance->engine = PROGRAM_NAME;
#else
      instance->engine = "Nagios";
#endif /* PROGRAM_NAME */
      instance->id = instance_id;
      instance->is_running = false;
      instance->name = instance_name;
      instance->pid = getpid();
      instance->program_end = time(NULL);
      instance->program_start = start_time;
      instance->version = get_program_version();

      // Send event.
      gl_publisher.write(instance.staticCast<io::data>());
    }
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  @brief Function that process instance status data.
 *
 *  This function is called by Nagios when some instance status data are
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_PROGRAM_STATUS_DATA).
 *  @param[in] data          A pointer to a nebstruct_program_status_data
 *                           containing the program status data.
 *
 *  @return 0 on success.
 */
int neb::callback_program_status(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM
    << "callbacks: generating instance status event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_program_status_data const* program_status_data;
    QSharedPointer<neb::instance_status> is(
      new neb::instance_status);

    // Fill output var.
    program_status_data = static_cast<nebstruct_program_status_data*>(data);
    is->active_host_checks_enabled
      = program_status_data->active_host_checks_enabled;
    is->active_service_checks_enabled
      = program_status_data->active_service_checks_enabled;
    is->daemon_mode = program_status_data->daemon_mode;
    is->event_handler_enabled
      = program_status_data->event_handlers_enabled;
    is->failure_prediction_enabled
      = program_status_data->failure_prediction_enabled;
    is->flap_detection_enabled
      = program_status_data->flap_detection_enabled;
    if (program_status_data->global_host_event_handler)
      is->global_host_event_handler
        = program_status_data->global_host_event_handler;
    if (program_status_data->global_service_event_handler)
      is->global_service_event_handler
        = program_status_data->global_service_event_handler;
    is->id = instance_id;
    is->last_alive = time(NULL);
    is->last_command_check = program_status_data->last_command_check;
    is->last_log_rotation
      = program_status_data->last_log_rotation;
    is->modified_host_attributes
      = program_status_data->modified_host_attributes;
    is->modified_service_attributes
      = program_status_data->modified_service_attributes;
    is->notifications_enabled
      = program_status_data->notifications_enabled;
    is->obsess_over_hosts
      = program_status_data->obsess_over_hosts;
    is->obsess_over_services
      = program_status_data->obsess_over_services;
    is->passive_host_checks_enabled
      = program_status_data->passive_host_checks_enabled;
    is->passive_service_checks_enabled
      = program_status_data->passive_service_checks_enabled;
    is->process_performance_data
      = program_status_data->process_performance_data;

    // Send event.
    gl_publisher.write(is.staticCast<io::data>());
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  @brief Function that process service check data.
 *
 *  This function is called by Nagios when some service check data are
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_SERVICE_CHECK_DATA).
 *  @param[in] data          A pointer to a nebstruct_service_check_data
 *                           containing the service check data.
 *
 *  @return 0 on success.
 */
int neb::callback_service_check(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM
    << "callbacks: generating service check event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_service_check_data const* scdata;
    QSharedPointer<neb::service_check> service_check(
      new neb::service_check);

    // Fill output var.
    scdata = static_cast<nebstruct_service_check_data*>(data);
    if (scdata->command_line) {
      service_check->command_line = scdata->command_line;
      if (scdata->host_name && scdata->service_description) {
        std::map<std::pair<std::string, std::string>, std::pair<int, int> >::const_iterator it;
        it = gl_services.find(std::make_pair(scdata->host_name,
                                             scdata->service_description));
        if (it != gl_services.end()) {
          service_check->host_id = it->second.first;
          service_check->service_id = it->second.second;
        }
      }

      // Send event.
      gl_publisher.write(service_check.staticCast<io::data>());
    }
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  @brief Function that process service status data.
 *
 *  This function is called by Nagios when some service status data are
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_SERVICE_STATUS_DATA).
 *  @param[in] data          A pointer to a nebstruct_service_status_data
 *                           containing the service status data.
 *
 *  @return 0 on success.
 */
int neb::callback_service_status(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM
    << "callbacks: generating service status event";
  (void)callback_type;

  try {
    // In/Out variables.
    ::service const* s;
    QSharedPointer<neb::service_status> service_status(
      new neb::service_status);

    // Fill output var.
    s = static_cast< ::service*>(
      static_cast<nebstruct_service_status_data*>(data)->object_ptr);
    service_status->acknowledgement_type = s->acknowledgement_type;
    service_status->active_checks_enabled = s->checks_enabled;
    if (s->service_check_command)
      service_status->check_command = s->service_check_command;
    service_status->check_interval = s->check_interval;
    if (s->check_period)
      service_status->check_period = s->check_period;
    service_status->check_type = s->check_type;
    service_status->current_check_attempt = s->current_attempt;
    service_status->current_notification_number
      = s->current_notification_number;
    service_status->current_state = s->current_state;
    if (s->event_handler)
      service_status->event_handler = s->event_handler;
    service_status->event_handler_enabled = s->event_handler_enabled;
    service_status->execution_time = s->execution_time;
    service_status->failure_prediction_enabled
      = s->failure_prediction_enabled;
    service_status->flap_detection_enabled = s->flap_detection_enabled;
    service_status->has_been_checked = s->has_been_checked;
    service_status->is_flapping = s->is_flapping;
    service_status->last_check = s->last_check;
    service_status->last_hard_state = s->last_hard_state;
    service_status->last_hard_state_change = s->last_hard_state_change;
    service_status->last_notification = s->last_notification;
    service_status->last_state_change = s->last_state_change;
    service_status->last_time_critical = s->last_time_critical;
    service_status->last_time_ok = s->last_time_ok;
    service_status->last_time_unknown = s->last_time_unknown;
    service_status->last_time_warning = s->last_time_warning;
    service_status->last_update = time(NULL);
    service_status->latency = s->latency;
    service_status->max_check_attempts = s->max_attempts;
    service_status->modified_attributes = s->modified_attributes;
    service_status->next_check = s->next_check;
    service_status->next_notification = s->next_notification;
    service_status->no_more_notifications = s->no_more_notifications;
    service_status->notifications_enabled = s->notifications_enabled;
    service_status->obsess_over = s->obsess_over_service;
    if (s->plugin_output)
      service_status->output = s->plugin_output;
    if (s->long_plugin_output)
      service_status->output.append(s->long_plugin_output);
    service_status->passive_checks_enabled
      = s->accept_passive_service_checks;
    service_status->percent_state_change = s->percent_state_change;
    if (s->perf_data)
      service_status->perf_data = s->perf_data;
    service_status->problem_has_been_acknowledged
      = s->problem_has_been_acknowledged;
    service_status->process_performance_data = s->process_performance_data;
    service_status->retry_interval = s->retry_interval;
    service_status->scheduled_downtime_depth = s->scheduled_downtime_depth;
    if (s->host_name && s->description) {
      std::map<std::pair<std::string, std::string>,
               std::pair<int, int> >::const_iterator it;
      it = gl_services.find(std::make_pair(s->host_name,
                                           s->description));
      if (it != gl_services.end()) {
        service_status->host_id = it->second.first;
        service_status->service_id = it->second.second;
      }
    }
    service_status->should_be_scheduled = s->should_be_scheduled;
    service_status->state_type = s->state_type;

    // Send event.
    gl_publisher.write(service_status.staticCast<io::data>());
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}
