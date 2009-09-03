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
#include "dbuffer.h"
#include "nagios/nebstructs.h"

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
int CentreonBroker::CallbackAcknowledgement(int callback_type,
                                            void* data) throw ()
{
  (void)callback_type;
  try
    {
      nebstruct_acknowledgement_data* ack_data;
      std::auto_ptr<DBuffer> buffer(new DBuffer);

      ack_data = static_cast<nebstruct_acknowledgement_data*>(data);
      buffer->Append(ack_data->host_name);
      buffer->Append(ack_data->service_description);
      buffer->Append((short)ack_data->type);
      buffer->Append(ack_data->author_name);
      buffer->Append(ack_data->comment_data);
      // XXX : entry_time
      buffer->Append((bool)ack_data->is_sticky);
      buffer->Append((bool)ack_data->notify_contacts);
      buffer->Append((bool)ack_data->persistent_comment);
      buffer->Append((short)ack_data->state);

      // XXX : send buffer
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
int CentreonBroker::CallbackComment(int callback_type, void* data) throw ()
{
  (void)callback_type;
  try
    {
      std::auto_ptr<DBuffer> buffer(new DBuffer);
      nebstruct_comment_data* comment_data;

      comment_data = static_cast<nebstruct_comment_data*>(data);
      buffer->Append(comment_data->author_name);
      buffer->Append(comment_data->comment_data);
      // XXX : comment_time
      buffer->Append((short)comment_data->type);
      // XXX : deletion_time
      // XXX : entry_time
      buffer->Append((short)comment_data->entry_type);
      buffer->Append((int)comment_data->expire_time);
      buffer->Append((bool)comment_data->expires);
      buffer->Append(comment_data->host_name);
      buffer->Append((int)comment_data->comment_id);
      buffer->Append((bool)comment_data->persistent);
      buffer->Append(comment_data->service_description);
      buffer->Append((short)comment_data->source);

      // XXX : send buffer
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
int CentreonBroker::CallbackDowntime(int callback_type, void* data) throw ()
{
  (void)callback_type;
  try
    {
      std::auto_ptr<DBuffer> buffer(new DBuffer);
      nebstruct_downtime_data* downtime_data;

      downtime_data = static_cast<nebstruct_downtime_data*>(data);
      buffer->Append(downtime_data->author_name);
      buffer->Append(downtime_data->comment_data);
      buffer->Append((int)downtime_data->downtime_id);
      buffer->Append((short)downtime_data->type);
      buffer->Append((int)downtime_data->duration);
      buffer->Append((int)downtime_data->end_time);
      buffer->Append((bool)downtime_data->fixed);
      buffer->Append(downtime_data->host_name);
      buffer->Append(downtime_data->service_description);
      buffer->Append((int)downtime_data->start_time);
      buffer->Append((int)downtime_data->triggered_by);
      // XXX : was_cancelled
      // XXX : was_started

      // XXX : send buffer
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
int CentreonBroker::CallbackHostStatus(int callback_type, void* data) throw ()
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
int CentreonBroker::CallbackLog(int callback_type, void* data) throw ()
{
  (void)callback_type;
  try
    {
      std::auto_ptr<DBuffer> buffer(new DBuffer);
      nebstruct_log_data* log_data;

      log_data = static_cast<nebstruct_log_data*>(data);
      buffer->Append((int)log_data->entry_time);
      // XXX : host_name
      buffer->Append(log_data->data_type);
      // XXX : notification_cmd
      // XXX : notification_contact
      buffer->Append(log_data->data);
      // XXX : retry
      // XXX : service_description
      // XXX : status
      // XXX : type

      // XXX : send buffer
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
int CentreonBroker::CallbackProgramStatus(int callback_type,
                                          void* data) throw ()
{
  (void)callback_type;
  try
    {
      std::auto_ptr<DBuffer> buffer(new DBuffer);
      nebstruct_program_status_data* prog_status_data;

      prog_status_data = static_cast<nebstruct_program_status_data*>(data);
      buffer->Append((bool)prog_status_data->active_host_checks_enabled);
      buffer->Append((bool)prog_status_data->active_service_checks_enabled);
      buffer->Append((bool)prog_status_data->daemon_mode);
      buffer->Append((bool)prog_status_data->event_handlers_enabled);
      buffer->Append((bool)prog_status_data->failure_prediction_enabled);
      buffer->Append((bool)prog_status_data->flap_detection_enabled);
      buffer->Append(prog_status_data->global_host_event_handler);
      // XXX : instance_address
      // XXX : instance_description
      buffer->Append(prog_status_data->global_service_event_handler);
      // XXX : is_running
      // XXX : last_alive
      buffer->Append((int)prog_status_data->last_command_check);
      buffer->Append((int)prog_status_data->last_log_rotation);
      buffer->Append((int)prog_status_data->modified_host_attributes);
      buffer->Append((int)prog_status_data->modified_service_attributes);
      buffer->Append((bool)prog_status_data->notifications_enabled);
      buffer->Append((bool)prog_status_data->obsess_over_hosts);
      buffer->Append((bool)prog_status_data->obsess_over_services);
      buffer->Append((bool)prog_status_data->passive_host_checks_enabled);
      buffer->Append((bool)prog_status_data->passive_service_checks_enabled);
      buffer->Append(prog_status_data->pid);
      buffer->Append((bool)prog_status_data->process_performance_data);
      // XXX : program_end_time
      buffer->Append((int)prog_status_data->program_start);

      // XXX : send buffer
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
int CentreonBroker::CallbackServiceStatus(int callback_type,
                                          void* data) throw ()
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
