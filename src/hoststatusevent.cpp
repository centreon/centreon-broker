/*
** hoststatusevent.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/07/09 Matthieu Kermagoret
** Last update 05/07/09 Matthieu Kermagoret
*/

#include <cstring>
#include "eventsubscriber.h"
#include "hoststatusevent.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  HostStatusEvent constructor.
 */
HostStatusEvent::HostStatusEvent()
{
}

/**
 *  HostStatusEvent copy constructor.
 */
HostStatusEvent::HostStatusEvent(const HostStatusEvent& hse) : Event()
{
  operator=(hse);
}

/**
 *  HostStatusEvent destructor.
 */
HostStatusEvent::~HostStatusEvent()
{
}

/**
 *  HostStatusEvent operator= overload.
 */
HostStatusEvent& HostStatusEvent::operator=(const HostStatusEvent& hse)
{
  memcpy(this, &hse, sizeof(*this));
  return (*this);
}

/**
 *  Take the visitor and show him the inside.
 */
void HostStatusEvent::AcceptVisitor(EventSubscriber& ev)
{
  ev.Visit(this->hoststatus_id);
  ev.Visit(this->instance_id);
  ev.Visit(this->host_object_id);
  ev.Visit(this->status_update_time);
  ev.Visit(this->output);
  ev.Visit(this->perfdata);
  ev.Visit(this->current_state);
  ev.Visit(this->has_been_checked);
  ev.Visit(this->should_be_scheduled);
  ev.Visit(this->current_check_attempt);
  ev.Visit(this->max_check_attempts);
  ev.Visit(this->last_check);
  ev.Visit(this->next_check);
  ev.Visit(this->check_type);
  ev.Visit(this->last_state_change);
  ev.Visit(this->last_hard_state_change);
  ev.Visit(this->last_time_up);
  ev.Visit(this->last_time_down);
  ev.Visit(this->last_time_unreachable);
  ev.Visit(this->state_type);
  ev.Visit(this->last_notification);
  ev.Visit(this->next_notification);
  ev.Visit(this->no_more_notifications);
  ev.Visit(this->notifications_enabled);
  ev.Visit(this->problem_has_been_acknowledged);
  ev.Visit(this->acknowledgement_type);
  ev.Visit(this->current_notification_number);
  ev.Visit(this->passive_checks_enabled);
  ev.Visit(this->active_checks_enabled);
  ev.Visit(this->event_handler_enabled);
  ev.Visit(this->flap_detection_enabled);
  ev.Visit(this->is_flapping);
  ev.Visit(this->percent_state_change);
  ev.Visit(this->latency);
  ev.Visit(this->execution_time);
  ev.Visit(this->scheduled_downtime_depth);
  ev.Visit(this->failure_prediction_enabled);
  ev.Visit(this->process_performance_data);
  ev.Visit(this->obsess_over_host);
  ev.Visit(this->modified_host_attributes);
  ev.Visit(this->event_handler);
  ev.Visit(this->check_command);
  ev.Visit(this->normal_check_interval);
  ev.Visit(this->retry_check_interval);
  ev.Visit(this->check_timeperiod_object_id);
  return ;
}

/**
 *  Returns the type of the event.
 */
int HostStatusEvent::GetType()
{
  return (0);
}

/**
 *  Sets the hoststatus_id.
 */
void HostStatusEvent::SetHostStatusId(int hsi)
{
  this->hoststatus_id = hsi;
  return ;
}

/**
 *  Sets the host_object_id.
 */
void HostStatusEvent::SetHostObjectId(int hoi)
{
  this->host_object_id = hoi;
  return ;
}
