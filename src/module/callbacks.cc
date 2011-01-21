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

#include <memory>
#include <time.h>
#include <unistd.h>
#include "callbacks.hh"
#include "config/globals.hh"
#include "config/handle.hh"
#include "events/events.hh"
#include "initial.hh"
#include "logging/logging.hh"
#include "module/internal.hh"
#include "module/set_log_data.hh"
#include "nagios/broker.h"
#include "nagios/comments.h"
#include "nagios/nebmodules.h"
#include "nagios/nebstructs.h"
#include "nagios/objects.h"

// List of Nagios modules.
extern nebmodule* nebmodule_list;

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
int callback_acknowledgement(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM << "generating acknowledgement event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_acknowledgement_data const* ack_data;
    std::auto_ptr<events::acknowledgement> ack(new events::acknowledgement);

    // Fill output var.
    ack_data = static_cast<nebstruct_acknowledgement_data*>(data);
    ack->acknowledgement_type = ack_data->type;
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
    ack->is_sticky = ack_data->is_sticky;
    ack->notify_contacts = ack_data->notify_contacts;
    ack->persistent_comment = ack_data->persistent_comment;
    ack->state = ack_data->state;

    // Send event.
    ack->add_reader();
    gl_publisher.event(ack.get());
    ack.release();
  }
  // Avoid exception propagation in C code.
  catch (...) {}
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
int callback_comment(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM << "generating comment event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_comment_data const* comment_data;
    std::auto_ptr<events::comment> comment(new events::comment);

    // Fill output var.
    comment_data = static_cast<nebstruct_comment_data*>(data);
    if (comment_data->author_name)
      comment->author = comment_data->author_name;
    if (comment_data->comment_data)
      comment->data = comment_data->comment_data;
    comment->type = comment_data->type;
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
    comment->internal_id = comment_data->comment_id;
    comment->persistent = comment_data->persistent;
    comment->source = comment_data->source;

    // Send event.
    comment->add_reader();
    gl_publisher.event(comment.get());
    comment.release();
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
int callback_downtime(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM << "generating downtime event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_downtime_data const* downtime_data;
    std::auto_ptr<events::downtime> downtime(new events::downtime);

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
    downtime->internal_id = downtime_data->downtime_id;
    downtime->start_time = downtime_data->start_time;
    downtime->triggered_by = downtime_data->triggered_by;
    if ((NEBTYPE_DOWNTIME_DELETE == downtime_data->downtime_type)
        || (NEBTYPE_DOWNTIME_STOP == downtime_data->downtime_type))
      downtime->was_cancelled = true;
    if (NEBTYPE_DOWNTIME_START == downtime_data->downtime_type)
      downtime->was_started = true;

    // Send event.
    downtime->add_reader();
    gl_publisher.event(downtime.get());
    downtime.release();
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
int callback_event_handler(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM << "generating event handler event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_event_handler_data const* event_handler_data;
    std::auto_ptr<events::event_handler> event_handler(new events::event_handler);

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
    event_handler->type = event_handler_data->eventhandler_type;

    // Send event.
    event_handler->add_reader();
    gl_publisher.event(event_handler.get());
    event_handler.release();
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
int callback_flapping_status(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM << "generating flapping event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_flapping_data const* flapping_data;
    std::auto_ptr<events::flapping_status> flapping_status(new events::flapping_status);

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
          comment* com = find_service_comment(flapping_data->comment_id);
          if (com)
            flapping_status->comment_time = com->entry_time;
        }
        else {
          comment* com = find_host_comment(flapping_data->comment_id);
          if (com)
            flapping_status->comment_time = com->entry_time;
        }
      }
    }
    flapping_status->internal_comment_id = flapping_data->comment_id;
    flapping_status->low_threshold = flapping_data->low_threshold;
    flapping_status->percent_state_change = flapping_data->percent_change;
    // flapping_status->reason_type = XXX;
    flapping_status->type = flapping_data->flapping_type;

    // Send event.
    flapping_status->add_reader();
    gl_publisher.event(flapping_status.get());
    flapping_status.release();
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
int callback_host_check(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM << "generating host check event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_host_check_data const* hcdata;
    std::auto_ptr<events::host_check> host_check(new events::host_check);

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
      host_check->add_reader();
      gl_publisher.event(host_check.get());
      host_check.release();
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
int callback_host_status(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM << "generating host status event";
  (void)callback_type;

  try {
    // In/Out variables.
    host const* h;
    std::auto_ptr<events::host_status> host_status(new events::host_status);

    // Fill output var.
    h = static_cast<host*>(
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
    host_status->add_reader();
    gl_publisher.event(host_status.get());
    host_status.release();
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
int callback_log(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM << "generating log event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_log_data const* log_data;
    std::auto_ptr<events::log_entry> le(new events::log_entry);

    // Fill output var.
    log_data = static_cast<nebstruct_log_data*>(data);
    le->c_time = log_data->entry_time;
    le->instance_name = config::globals::instance_name;
    if (log_data->data) {
      if (log_data->data)
        le->output = log_data->data;
      set_log_data(*le, log_data->data);
    }

    // Send event.
    le->add_reader();
    gl_publisher.event(le.get());
    le.release();
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
int callback_process(int callback_type, void *data) {
  // Log message.
  logging::debug << logging::LOW << "process event callback";
  (void)callback_type;

  try {
    // Input variables.
    nebstruct_process_data const* process_data;
    static time_t start_time;

    // Check process event type.
    process_data = static_cast<nebstruct_process_data*>(data);
    if (NEBTYPE_PROCESS_EVENTLOOPSTART == process_data->type) {
      logging::info << logging::MEDIUM << "generating process start event";
      // Output variable.
      std::auto_ptr<events::instance> instance(new events::instance);

      // Fill output var.
      config::handle(gl_configuration_file);
      logging::log_on(gl_initial_logger, 0, logging::NONE);
      instance->id = config::globals::instance;
      instance->is_running = true;
      instance->name = config::globals::instance_name;
      instance->pid = getpid();
      instance->program_start = time(NULL);
      start_time = instance->program_start;

      // Send initial event and then configuration.
      instance->add_reader();
      gl_publisher.event(instance.get());
      instance.release();
      send_initial_configuration();

      // Generate module list.
      for (nebmodule* nm = nebmodule_list; nm; nm = nm->next)
        if (nm->filename) {
          // Output variable.
          std::auto_ptr<events::module> module(new events::module);

          // Fill output var.
          if (nm->args)
            module->args = nm->args;
          module->filename = nm->filename;
          module->instance_id = config::globals::instance;
          module->loaded = nm->is_currently_loaded;
          module->should_be_loaded = nm->should_be_loaded;

          // Send events.
          module->add_reader();
          gl_publisher.event(module.get());
          module.release();
        }
    }
    else if (NEBTYPE_PROCESS_EVENTLOOPEND == process_data->type) {
      logging::info << logging::MEDIUM << "generating process end event";
      // Output variable.
      std::auto_ptr<events::instance> instance(new events::instance);

      // Fill output var.
      instance->id = config::globals::instance;
      instance->is_running = false;
      instance->name = config::globals::instance_name;
      instance->pid = getpid();
      instance->program_end = time(NULL);
      instance->program_start = start_time;

      // Send event.
      instance->add_reader();
      gl_publisher.event(instance.get());
      instance.release();
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
int callback_program_status(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM << "generating instance status event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_program_status_data const* program_status_data;
    std::auto_ptr<events::instance_status> is(
      new events::instance_status);

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
    is->id = config::globals::instance;
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
    is->add_reader();
    gl_publisher.event(is.get());
    is.release();
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
int callback_service_check(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM << "generating service check event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_service_check_data const* scdata;
    std::auto_ptr<events::service_check> service_check(
      new events::service_check);

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
      service_check->add_reader();
      gl_publisher.event(service_check.get());
      service_check.release();
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
int callback_service_status(int callback_type, void* data) {
  // Log message.
  logging::info << logging::MEDIUM << "generating service status event";
  (void)callback_type;

  try {
    // In/Out variables.
    service const* s;
    std::auto_ptr<events::service_status> service_status(
      new events::service_status);

    // Fill output var.
    s = static_cast<service*>(
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
    service_status->add_reader();
    gl_publisher.event(service_status.get());
    service_status.release();
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}
