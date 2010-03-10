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

#include <memory>
#include <time.h>                  // for time
#include <unistd.h>                // for getpid
#include "callbacks.h"
#include "configuration/manager.h"
// XXX : dirty hack
#include "configuration/parser.h"
#include "events/events.h"
#include "initial.h"
#include "logging.h"
#include "module/internal.h"
#include "module/set_log_data.h"
#include "nagios/broker.h"
#include "nagios/nebstructs.h"
#include "nagios/objects.h"

/**
 *  \brief Function that process acknowledgement data.
 *
 *  This function is called by Nagios when some acknowledgement data are
 *  available.
 *
 *  \param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_ACKNOWLEDGEMENT_DATA).
 *  \param[in] data          A pointer to a nebstruct_acknowledgement_data
 *                           containing the acknowledgement data.
 *
 *  \return 0 on success.
 */
int CallbackAcknowledgement(int callback_type, void* data)
{
  LOGDEBUG("Processing acknowledgement ...");
  (void)callback_type;
  try
    {
      nebstruct_acknowledgement_data* ack_data;
      std::auto_ptr<Events::Acknowledgement> ack(new Events::Acknowledgement);

      ack_data = static_cast<nebstruct_acknowledgement_data*>(data);
      ack->acknowledgement_type = ack_data->type; // XXX : duplicate with type
      if (ack_data->author_name)
        ack->author = ack_data->author_name;
      if (ack_data->comment_data)
        ack->comment = ack_data->comment_data;
      //ack->entry_time = XXX;
      if (ack_data->host_name)
        ack->host_name = ack_data->host_name;
      ack->instance_name = gl_instance_name;
      ack->is_sticky = ack_data->is_sticky;
      ack->notify_contacts = ack_data->notify_contacts;
      ack->persistent_comment = ack_data->persistent_comment;
      if (ack_data->service_description)
        ack->service_description = ack_data->service_description;
      ack->state = ack_data->state;
      ack->type = ack_data->type; // XXX : duplicate with acknowledgement_type

      ack->AddReader();
      gl_publisher.Event(ack.get());
      ack.release();
    }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  \brief Function that process comment data.
 *
 *  This function is called by Nagios when some comment data are available.
 *
 *  \param[in] callback_type Type of the callback (NEBCALLBACK_COMMENT_DATA).
 *  \param[in] data          A pointer to a nebstruct_comment_data containing
 *                           the comment data.
 *
 *  \return 0 on success.
 */
int CallbackComment(int callback_type, void* data)
{
  LOGDEBUG("Processing comment ...");
  (void)callback_type;
  try
    {
      nebstruct_comment_data* comment_data;
      std::auto_ptr<Events::Comment> comment(new Events::Comment);

      comment_data = static_cast<nebstruct_comment_data*>(data);
      if (comment_data->author_name)
        comment->author = comment_data->author_name;
      if (comment_data->comment_data)
        comment->comment = comment_data->comment_data;
      // comment->comment_time = XXX;
      comment->comment_type = comment_data->type;
      if (NEBTYPE_COMMENT_DELETE == comment_data->type)
        comment->deletion_time = time(NULL);
      // comment->entry_time = XXX;
      comment->entry_type = comment_data->entry_type;
      comment->expire_time = comment_data->expire_time;
      comment->expires = comment_data->expires;
      if (comment_data->host_name)
        comment->host_name = comment_data->host_name;
      comment->instance_name = gl_instance_name;
      comment->internal_id = comment_data->comment_id;
      comment->persistent = comment_data->persistent;
      if (comment_data->service_description)
        comment->service_description = comment_data->service_description;
      comment->source = comment_data->source;

      comment->AddReader();
      gl_publisher.Event(comment.get());
      comment.release();
    }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  \brief Function that process downtime data.
 *
 *  This function is called by Nagios when some downtime data are available.
 *
 *  \param[in] callback_type Type of the callback (NEBCALLBACK_DOWNTIME_DATA).
 *  \param[in] data          A pointer to a nebstruct_downtime_data containing
 *                           the downtime data.
 *
 *  \return 0 on success.
 */
int CallbackDowntime(int callback_type, void* data)
{
  LOGDEBUG("Processing downtime ...");
  (void)callback_type;
  try
    {
      nebstruct_downtime_data* downtime_data;
      std::auto_ptr<Events::Downtime> downtime(new Events::Downtime);

      downtime_data = static_cast<nebstruct_downtime_data*>(data);
      if (downtime_data->author_name)
        downtime->author = downtime_data->author_name;
      if (downtime_data->comment_data)
        downtime->comment = downtime_data->comment_data;
      downtime->downtime_type = downtime_data->type; // XXX : duplicate with type
      downtime->duration = downtime_data->duration;
      downtime->end_time = downtime_data->end_time;
      //downtime->entry_time = XXX;
      downtime->fixed = downtime_data->fixed;
      if (downtime_data->host_name)
        downtime->host_name = downtime_data->host_name;
      downtime->id = downtime_data->downtime_id;
      downtime->instance_name = gl_instance_name;
      if (downtime_data->service_description)
        downtime->service_description = downtime_data->service_description;
      downtime->start_time = downtime_data->start_time;
      downtime->triggered_by = downtime_data->triggered_by;
      downtime->type = downtime_data->type; // XXX : duplicate with downtime_type
      if ((NEBTYPE_DOWNTIME_DELETE == downtime_data->type)
          || (NEBTYPE_DOWNTIME_STOP == downtime_data->type))
        downtime->was_cancelled = true;
      // downtime->was_started = XXX;

      downtime->AddReader();
      gl_publisher.Event(downtime.get());
      downtime.release();
    }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  \brief Function that process host check data.
 *
 *  This function is called by Nagios when some host check data are available.
 *
 *  \param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_HOST_CHECK_DATA).
 *  \param[in] data          A pointer to a nebstruct_host_check_data
 *                           containing the host check data.
 *
 *  \return 0 on success.
 */
int CallbackHostCheck(int callback_type, void* data)
{
  LOGDEBUG("Processing host check ...");
  (void)callback_type;
  try
    {
      nebstruct_host_check_data* hcdata;
      std::auto_ptr<Events::HostCheck> host_check(new Events::HostCheck);

      hcdata = static_cast<nebstruct_host_check_data*>(data);
      if (hcdata->command_line)
        {
          host_check->command_line = hcdata->command_line;
          if (hcdata->host_name)
            {
              std::map<std::string, int>::const_iterator it;

              it = gl_hosts.find(hcdata->host_name);
              if (it != gl_hosts.end())
                host_check->id = it->second;
            }

          host_check->AddReader();
          gl_publisher.Event(host_check.get());
          host_check.release();
        }
    }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  \brief Function that process host status data.
 *
 *  This function is called by Nagios when some host status data are available.
 *
 *  \param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_HOST_STATUS_DATA).
 *  \param[in] data          A pointer to a nebstruct_host_status_data
 *                           containing the host status data.
 *
 *  \return 0 on success.
 */
int CallbackHostStatus(int callback_type, void* data)
{
  LOGDEBUG("Processing host status ...");
  (void)callback_type;
  try
    {
      host* h;
      std::auto_ptr<Events::HostStatus> host_status(new Events::HostStatus);

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
      if (h->name)
        {
          std::map<std::string, int>::const_iterator it;

          it = gl_hosts.find(h->name);
          if (it != gl_hosts.end())
            host_status->id = it->second;
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
      // host_status->last_update = XXX;
      host_status->latency = h->latency;
      if (h->long_plugin_output)
        host_status->long_output = h->long_plugin_output;
      host_status->max_check_attempts = h->max_attempts;
      host_status->modified_attributes = h->modified_attributes;
      host_status->next_check = h->next_check;
      host_status->next_notification = h->next_host_notification;
      host_status->no_more_notifications = h->no_more_notifications;
      host_status->notifications_enabled = h->notifications_enabled;
      host_status->obsess_over = h->obsess_over_host;
      if (h->plugin_output)
        host_status->output = h->plugin_output;
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

      host_status->AddReader();
      gl_publisher.Event(host_status.get());
      host_status.release();
    }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  \brief Function that process log data.
 *
 *  This function is called by Nagios when some log data are available.
 *
 *  \param[in] callback_type Type of the callback (NEBCALLBACK_LOG_DATA).
 *  \param[in] data          A pointer to a nebstruct_log_data containing the
 *                           log data.
 *
 *  \return 0 on success.
 */
int CallbackLog(int callback_type, void* data)
{
  LOGDEBUG("Processing log ...");
  (void)callback_type;
  try
    {
      nebstruct_log_data* log_data;
      std::auto_ptr<Events::Log> log(new Events::Log);

      log_data = static_cast<nebstruct_log_data*>(data);
      log->c_time = log_data->entry_time;
      log->instance = gl_instance;
      if (log_data->data)
        {
          log->output = log_data->data;
          SetLogData(*log, log_data->data);
        }
      log->type = log_data->data_type;

      log->AddReader();
      gl_publisher.Event(log.get());
      log.release();
    }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  \brief Function that process process data.
 *
 *  This function is called by Nagios when some process data is available.
 *
 *  \param[in] callback_type Type of the callback (NEBCALLBACK_PROCESS_DATA).
 *  \param[in] data          A pointer to a nebstruct_process_data containing
 *                           the process data.
 *
 *  \return 0 on success.
 */
int CallbackProcess(int callback_type, void *data)
{
  LOGDEBUG("Processing Nagios process event ...");
  (void)callback_type;
  try
    {
      nebstruct_process_data* process_data;
      static time_t start_time;

      process_data = static_cast<nebstruct_process_data*>(data);
      if (NEBTYPE_PROCESS_EVENTLOOPSTART == process_data->type)
        {
          std::auto_ptr<Events::Program> program(new Events::Program);

          Configuration::Manager::Instance().Open(gl_configuration_file);
          // program->daemon_mode = XXX;
          program->instance = gl_instance;
          program->instance_name = gl_instance_name;
          program->is_running = true;
          program->pid = getpid();
          program->program_start = time(NULL);
          start_time = program->program_start;

          program->AddReader();
          gl_publisher.Event(program.get());
          program.release();
          SendInitialConfiguration();
        }
      else if (NEBTYPE_PROCESS_EVENTLOOPEND == process_data->type)
        {
          std::auto_ptr<Events::Program> program(new Events::Program);

          program->instance = gl_instance;
          program->instance_name = gl_instance_name;
          program->is_running = false;
          program->pid = getpid();
          program->program_end = time(NULL);
          program->program_start = start_time;

          program->AddReader();
          gl_publisher.Event(program.get());
          program.release();
        }
    }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  \brief Function that process program status data.
 *
 *  This function is called by Nagios when some program status data are
 *  available.
 *
 *  \param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_PROGRAM_STATUS_DATA).
 *  \param[in] data          A pointer to a nebstruct_program_status_data
 *                           containing the program status data.
 *
 *  \return 0 on success.
 */
int CallbackProgramStatus(int callback_type, void* data)
{
  LOGDEBUG("Processing program status ...");
  (void)callback_type;
  try
    {
      nebstruct_program_status_data* program_status_data;
      std::auto_ptr<Events::ProgramStatus> program_status(
                                             new Events::ProgramStatus);

      program_status_data = static_cast<nebstruct_program_status_data*>(data);
      program_status->active_host_checks_enabled
        = program_status_data->active_host_checks_enabled;
      program_status->active_service_checks_enabled
        = program_status_data->active_service_checks_enabled;
      program_status->event_handler_enabled
        = program_status_data->event_handlers_enabled;
      program_status->failure_prediction_enabled
        = program_status_data->failure_prediction_enabled;
      program_status->flap_detection_enabled
        = program_status_data->flap_detection_enabled;
      if (program_status_data->global_host_event_handler)
        program_status->global_host_event_handler
          = program_status_data->global_host_event_handler;
      if (program_status_data->global_service_event_handler)
        program_status->global_service_event_handler
          = program_status_data->global_service_event_handler;
      program_status->instance = gl_instance;
      // program_status->last_alive = XXX;
      program_status->last_command_check
        = program_status_data->last_command_check;
      program_status->last_log_rotation
        = program_status_data->last_log_rotation;
      program_status->modified_host_attributes
        = program_status_data->modified_host_attributes;
      program_status->modified_service_attributes
        = program_status_data->modified_service_attributes;
      program_status->notifications_enabled
        = program_status_data->notifications_enabled;
      program_status->obsess_over_hosts
        = program_status_data->obsess_over_hosts;
      program_status->obsess_over_services
        = program_status_data->obsess_over_services;
      program_status->passive_host_checks_enabled
        = program_status_data->passive_host_checks_enabled;
      program_status->passive_service_checks_enabled
        = program_status_data->passive_service_checks_enabled;
      program_status->process_performance_data
        = program_status_data->process_performance_data;

      program_status->AddReader();
      gl_publisher.Event(program_status.get());
      program_status.release();
    }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  \brief Function that process service check data.
 *
 *  This function is called by Nagios when some service check data are
 *  available.
 *
 *  \param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_SERVICE_CHECK_DATA).
 *  \param[in] data          A pointer to a nebstruct_service_check_data
 *                           containing the service check data.
 *
 *  \return 0 on success.
 */
int CallbackServiceCheck(int callback_type, void* data)
{
  LOGDEBUG("Processing service check ...");
  (void)callback_type;
  try
    {
      nebstruct_service_check_data* scdata;
      std::auto_ptr<Events::ServiceCheck> service_check(
        new Events::ServiceCheck);

      scdata = static_cast<nebstruct_service_check_data*>(data);
      if (scdata->command_line)
        {
          service_check->command_line = scdata->command_line;
          if (scdata->host_name && scdata->service_description)
            {
              std::map<std::pair<std::string, std::string>, int>::const_iterator it;

              it = gl_services.find(std::make_pair(scdata->host_name,
                                                   scdata->service_description));
              if (it != gl_services.end())
                service_check->id = it->second;
            }

          service_check->AddReader();
          gl_publisher.Event(service_check.get());
          service_check.release();
        }
    }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  \brief Function that process service status data.
 *
 *  This function is called by Nagios when some service status data are
 *  available.
 *
 *  \param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_SERVICE_STATUS_DATA).
 *  \param[in] data          A pointer to a nebstruct_service_status_data
 *                           containing the service status data.
 *
 *  \return 0 on success.
 */
int CallbackServiceStatus(int callback_type, void* data)
{
  LOGDEBUG("Processing service status ...");
  (void)callback_type;
  try
    {
      service* s;
      std::auto_ptr<Events::ServiceStatus> service_status(
        new Events::ServiceStatus);

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
      // service_status->last_update = XXX;
      service_status->latency = s->latency;
      if (s->long_plugin_output)
        service_status->long_output = s->long_plugin_output;
      service_status->max_check_attempts = s->max_attempts;
      service_status->modified_attributes = s->modified_attributes;
      service_status->next_check = s->next_check;
      service_status->next_notification = s->next_notification;
      service_status->no_more_notifications = s->no_more_notifications;
      service_status->notifications_enabled = s->notifications_enabled;
      service_status->obsess_over = s->obsess_over_service;
      if (s->plugin_output)
        service_status->output = s->plugin_output;
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
      if (s->host_name && s->description)
        {
          std::map<std::pair<std::string,
                             std::string>, int>::const_iterator it;

          it = gl_services.find(std::make_pair(s->host_name,
                                               s->description));
          if (it != gl_services.end())
            service_status->id = it->second;
        }
      service_status->should_be_scheduled = s->should_be_scheduled;
      service_status->state_type = s->state_type;

      service_status->AddReader();
      gl_publisher.Event(service_status.get());
      service_status.release();
    }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}
