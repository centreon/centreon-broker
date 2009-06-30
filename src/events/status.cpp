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

#include <cstring>
#include "events/status.h"

using namespace CentreonBroker::Events;

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
