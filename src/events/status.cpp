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

#include "events/status.h"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy members of the given Status object to the current instance.
 *
 *  Copy all members defined within the Status class. This method is used by
 *  the copy constructor and the assignment operator.
 *
 *  \param[in] s Object to copy data from.
 */
void Status::InternalCopy(const Status& s)
{
  this->event_handler_enabled      = s.event_handler_enabled;
  this->failure_prediction_enabled = s.failure_prediction_enabled;
  this->flap_detection_enabled     = s.flap_detection_enabled;
  this->notifications_enabled      = s.notifications_enabled;
  this->process_performance_data   = s.process_performance_data;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Status default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
Status::Status()
  : event_handler_enabled(false),
    failure_prediction_enabled(false),
    flap_detection_enabled(false),
    notifications_enabled(false),
    process_performance_data(false) {}

/**
 *  \brief Status copy constructor.
 *
 *  Copy all members of the given Status object to the current instance.
 *
 *  \param[in] s Object to copy data from.
 */
Status::Status(const Status& s) : Event(s)
{
  this->InternalCopy(s);
}

/**
 *  Status destructor.
 */
Status::~Status() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all members of the given Status object to the current instance.
 *
 *  \param[in] s Object to copy data from.
 *
 *  \return *this
 */
Status& Status::operator=(const Status& s)
{
  this->Event::operator=(s);
  this->InternalCopy(s);
  return (*this);
}
