/*
** status.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/15/09 Matthieu Kermagoret
** Last update 05/20/09 Matthieu Kermagoret
*/

#include <cstring>
#include "status.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Make a copy of all internal members of a Status to the current
 *  instance.
 */
void Status::InternalCopy(const Status& se) throw ()
{
  memcpy(this->shorts_, se.shorts_, sizeof(this->shorts_));
  memcpy(this->timets_, se.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Status default constructor.
 */
Status::Status()
{
  memset(this->shorts_, 0, sizeof(this->shorts_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  Status copy constructor.
 */
Status::Status(const Status& se) : Event(se)
{
  this->InternalCopy(se);
}

/**
 *  Status destructor.
 */
Status::~Status()
{
}

/**
 *  Status operator= overload.
 */
Status& Status::operator=(const Status& se)
{
  this->Event::operator=(se);
  this->InternalCopy(se);
  return (*this);
}

/**
 *  Get the event_handler_enabled member.
 */
short Status::GetEventHandlerEnabled() const throw ()
{
  return (this->shorts_[EVENT_HANDLER_ENABLED]);
}

/**
 *  Get the failure_prediction_enabled member.
 */
short Status::GetFailurePredictionEnabled() const throw ()
{
  return (this->shorts_[FAILURE_PREDICTION_ENABLED]);
}

/**
 *  Get the flap_detection_enabled member.
 */
short Status::GetFlapDetectionEnabled() const throw ()
{
  return (this->shorts_[FLAP_DETECTION_ENABLED]);
}

/**
 *  Get the notifications_enabled member.
 */
short Status::GetNotificationsEnabled() const throw ()
{
  return (this->shorts_[NOTIFICATIONS_ENABLED]);
}

/**
 *  Get the process_performance_data member.
 */
short Status::GetProcessPerformanceData() const throw ()
{
  return (this->shorts_[PROCESS_PERFORMANCE_DATA]);
}

/**
 *  Get the status_update_time member.
 */
time_t Status::GetStatusUpdateTime() const throw ()
{
  return (this->timets_[STATUS_UPDATE_TIME]);
}

/**
 *  Set the event_handler_enabled member.
 */
void Status::SetEventHandlerEnabled(short ehe) throw ()
{
  this->shorts_[EVENT_HANDLER_ENABLED] = ehe;
  return ;
}

/**
 *  Set the failure_prediction_enabled member.
 */
void Status::SetFailurePredictionEnabled(short fpe) throw ()
{
  this->shorts_[FAILURE_PREDICTION_ENABLED] = fpe;
  return ;
}

/**
 *  Set the flap_detection_enabled member.
 */
void Status::SetFlapDetectionEnabled(short fde) throw ()
{
  this->shorts_[FLAP_DETECTION_ENABLED] = fde;
  return ;
}

/**
 *  Set the notifications_enabled member.
 */
void Status::SetNotificationsEnabled(short ne) throw ()
{
  this->shorts_[NOTIFICATIONS_ENABLED] = ne;
  return ;
}

/**
 *  Set the process_performance_data member.
 */
void Status::SetProcessPerformanceData(short ppd) throw ()
{
  this->shorts_[PROCESS_PERFORMANCE_DATA] = ppd;
  return ;
}

/**
 *  Set the status_update_time member.
 */
void Status::SetStatusUpdateTime(time_t sut) throw ()
{
  this->timets_[STATUS_UPDATE_TIME] = sut;
  return ;
}
