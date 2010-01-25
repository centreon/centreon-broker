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
#include "callbacks.h"
#include "events/events.h"
#include "multiplexing/publisher.h"
#include "nagios/nebstructs.h"

// Extern global sender.
extern Multiplexing::Publisher gl_publisher;

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
  (void)callback_type;
  try
    {
      nebstruct_acknowledgement_data* ack_data;
      std::auto_ptr<Events::Acknowledgement> ack(new Events::Acknowledgement);

      ack_data = static_cast<nebstruct_acknowledgement_data*>(data);
      ack->acknowledgement_type = ack_data->type; // XXX : duplicate with type
      ack->author = ack_data->author_name;
      ack->comment = ack_data->comment_data;
      //ack->entry_time = XXX;
      ack->host = ack_data->host_name;
      ack->is_sticky = ack_data->is_sticky;
      ack->notify_contacts = ack_data->notify_contacts;
      ack->persistent_comment = ack_data->persistent_comment;
      ack->service = ack_data->service_description;
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
  (void)callback_type;
  try
    {
      nebstruct_comment_data* comment_data;
      std::auto_ptr<Events::Comment> comment(new Events::Comment);

      comment_data = static_cast<nebstruct_comment_data*>(data);
      comment->author = comment_data->author_name;
      comment->comment = comment_data->comment_data;
      // comment->comment_time = XXX;
      comment->comment_type = comment_data->type;
      // comment->deletion_time = XXX;
      // comment->entry_time = XXX;
      comment->entry_type = comment_data->entry_type;
      comment->expire_time = comment_data->expire_time;
      comment->expires = comment_data->expires;
      comment->host = comment_data->host_name;
      comment->internal_id = comment_data->comment_id;
      comment->persistent = comment_data->persistent;
      comment->service = comment_data->service_description;
      comment->source = comment_data->source;
      // comment->type = XXX;

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
  (void)callback_type;
  try
    {
      nebstruct_downtime_data* downtime_data;
      std::auto_ptr<Events::Downtime> downtime(new Events::Downtime);

      downtime_data = static_cast<nebstruct_downtime_data*>(data);
      downtime->author = downtime_data->author_name;
      downtime->comment = downtime_data->comment_data;
      downtime->downtime_type = downtime_data->type; // XXX : duplicate with type
      downtime->duration = downtime_data->duration;
      downtime->end_time = downtime_data->end_time;
      //downtime->entry_time = XXX;
      downtime->fixed = downtime_data->fixed;
      downtime->host = downtime_data->host_name;
      downtime->id = downtime_data->downtime_id;
      downtime->service = downtime_data->service_description;
      downtime->start_time = downtime_data->start_time;
      downtime->triggered_by = downtime_data->triggered_by;
      downtime->type = downtime_data->type; // XXX : duplicate with downtime_type
      // downtime->was_cancelled = XXX;
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
  (void)callback_type;
  try
    {
      nebstruct_host_status_data* host_status_data;

      host_status_data = static_cast<nebstruct_host_status_data*>(data);
      // XXX : processing
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
  (void)callback_type;
  try
    {
      nebstruct_log_data* log_data;
      std::auto_ptr<Events::Log> log(new Events::Log);

      log_data = static_cast<nebstruct_log_data*>(data);
      log->c_time = log_data->entry_time;
      // log->host = XXX;
      log->msg_type = log_data->data_type;
      // log->notification_cmd = XXX;
      // log->notification_contact = XXX;
      log->output = log_data->data;
      // log->retry = XXX;
      // log->service = XXX;
      // log->status = XXX;
      // log->type = XXX;

      log->AddReader();
      gl_publisher.Event(log.get());
      log.release();
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
      program_status->daemon_mode = program_status_data->daemon_mode;
      program_status->event_handler_enabled
	= program_status_data->event_handlers_enabled;
      program_status->failure_prediction_enabled
	= program_status_data->failure_prediction_enabled;
      program_status->flap_detection_enabled
	= program_status_data->flap_detection_enabled;
      program_status->global_host_event_handler
	= program_status_data->global_host_event_handler;
      program_status->global_service_event_handler
	= program_status_data->global_service_event_handler;
      // program_status->is_running = XXX;
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
      program_status->pid = program_status_data->pid;
      program_status->process_performance_data
	= program_status_data->process_performance_data;
      // program_status->program_end = XXX;
      program_status->program_start = program_status_data->program_start;

      program_status->AddReader();
      gl_publisher.Event(program_status.get());
      program_status.release();
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
  (void)callback_type;
  try
    {
      nebstruct_service_status_data* service_status_data;

      service_status_data = static_cast<nebstruct_service_status_data*>(data);

      // XXX : processing
    }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}
