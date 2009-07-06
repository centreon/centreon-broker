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
  memcpy(this->bools_, se.bools_, sizeof(this->bools_));
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
  memset(this->bools_, 0, sizeof(this->bools_));
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
bool Status::GetEventHandlerEnabled() const throw ()
{
  return (this->bools_[EVENT_HANDLER_ENABLED]);
}

/**
 *  Get the failure_prediction_enabled member.
 */
bool Status::GetFailurePredictionEnabled() const throw ()
{
  return (this->bools_[FAILURE_PREDICTION_ENABLED]);
}

/**
 *  Get the flap_detection_enabled member.
 */
bool Status::GetFlapDetectionEnabled() const throw ()
{
  return (this->bools_[FLAP_DETECTION_ENABLED]);
}

/**
 *  Get the notifications_enabled member.
 */
bool Status::GetNotificationsEnabled() const throw ()
{
  return (this->bools_[NOTIFICATIONS_ENABLED]);
}

/**
 *  Get the process_performance_data member.
 */
bool Status::GetProcessPerformanceData() const throw ()
{
  return (this->bools_[PROCESS_PERFORMANCE_DATA]);
}

/**
 *  Set the event_handler_enabled member.
 */
void Status::SetEventHandlerEnabled(bool ehe) throw ()
{
  this->bools_[EVENT_HANDLER_ENABLED] = ehe;
  return ;
}

/**
 *  Set the failure_prediction_enabled member.
 */
void Status::SetFailurePredictionEnabled(bool fpe) throw ()
{
  this->bools_[FAILURE_PREDICTION_ENABLED] = fpe;
  return ;
}

/**
 *  Set the flap_detection_enabled member.
 */
void Status::SetFlapDetectionEnabled(bool fde) throw ()
{
  this->bools_[FLAP_DETECTION_ENABLED] = fde;
  return ;
}

/**
 *  Set the notifications_enabled member.
 */
void Status::SetNotificationsEnabled(bool ne) throw ()
{
  this->bools_[NOTIFICATIONS_ENABLED] = ne;
  return ;
}

/**
 *  Set the process_performance_data member.
 */
void Status::SetProcessPerformanceData(bool ppd) throw ()
{
  this->bools_[PROCESS_PERFORMANCE_DATA] = ppd;
  return ;
}
