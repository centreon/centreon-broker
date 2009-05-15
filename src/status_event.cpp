/*
** status_event.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/15/09 Matthieu Kermagoret
** Last update 05/15/09 Matthieu Kermagoret
*/

#include <cstring>
#include "status_event.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Make a copy of all internal members of a StatusEvent to the current
 *  instance.
 */
void StatusEvent::InternalCopy(const StatusEvent& se) throw ()
{
  memcpy(this->shorts_, se.shorts_, sizeof(*this->shorts_));
  this->status_update_time_ = se.status_update_time_;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  StatusEvent default constructor.
 */
StatusEvent::StatusEvent()
{
  memset(this->shorts_, 0, sizeof(*this->shorts_));
  this->status_update_time_ = 0;
}

/**
 *  StatusEvent copy constructor.
 */
StatusEvent::StatusEvent(const StatusEvent& se) : Event(se)
{
  this->InternalCopy(se);
}

/**
 *  StatusEvent destructor.
 */
StatusEvent::~StatusEvent()
{
}

/**
 *  StatusEvent operator= overload.
 */
StatusEvent& StatusEvent::operator=(const StatusEvent& se)
{
  this->Event::operator=(se);
  this->InternalCopy(se);
  return (*this);
}

/**
 *  Get the event_handler_enabled member.
 */
short StatusEvent::GetEventHandlerEnabled() const throw ()
{
  return (this->shorts_[EVENT_HANDLER_ENABLED]);
}

/**
 *  Get the failure_prediction_enabled member.
 */
short StatusEvent::GetFailurePredictionEnabled() const throw ()
{
  return (this->shorts_[FAILURE_PREDICTION_ENABLED]);
}

/**
 *  Get the flap_detection_enabled member.
 */
short StatusEvent::GetFlapDetectionEnabled() const throw ()
{
  return (this->shorts_[FLAP_DETECTION_ENABLED]);
}

/**
 *  Get the notifications_enabled member.
 */
short StatusEvent::GetNotificationsEnabled() const throw ()
{
  return (this->shorts_[NOTIFICATIONS_ENABLED]);
}

/**
 *  Get the process_performance_data member.
 */
short StatusEvent::GetProcessPerformanceData() const throw ()
{
  return (this->shorts_[PROCESS_PERFORMANCE_DATA]);
}

/**
 *  Get the status_update_time member.
 */
time_t StatusEvent::GetStatusUpdateTime() const throw ()
{
  return (this->status_update_time_);
}

/**
 *  Set the event_handler_enabled member.
 */
void StatusEvent::SetEventHandlerEnabled(short ehe) throw ()
{
  this->shorts_[EVENT_HANDLER_ENABLED] = ehe;
  return ;
}

/**
 *  Set the failure_prediction_enabled member.
 */
void StatusEvent::SetFailurePredictionEnabled(short fpe) throw ()
{
  this->shorts_[FAILURE_PREDICTION_ENABLED] = fpe;
  return ;
}

/**
 *  Set the flap_detection_enabled member.
 */
void StatusEvent::SetFlapDetectionEnabled(short fde) throw ()
{
  this->shorts_[FLAP_DETECTION_ENABLED] = fde;
  return ;
}

/**
 *  Set the notifications_enabled member.
 */
void StatusEvent::SetNotificationsEnabled(short ne) throw ()
{
  this->shorts_[NOTIFICATIONS_ENABLED] = ne;
  return ;
}

/**
 *  Set the process_performance_data member.
 */
void StatusEvent::SetProcessPerformanceData(short ppd) throw ()
{
  this->shorts_[PROCESS_PERFORMANCE_DATA] = ppd;
  return ;
}

/**
 *  Set the status_update_time member.
 */
void StatusEvent::SetStatusUpdateTime(time_t sut) throw ()
{
  this->status_update_time_ = sut;
  return ;
}
