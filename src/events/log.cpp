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

#include "events/log.h"

using namespace Events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief Copy all internal data of the given object to the current instance.
 *
 *  Make a copy of all data defined within the Log class. This method is used
 *  by the copy constructor and the assignment operator.
 *
 *  \param[in] log Object to copy data from.
 */
void Log::InternalCopy(const Log& log)
{
  this->c_time               = log.c_time;
  this->host_name            = log.host_name;
  this->instance_name        = log.instance_name;
  this->msg_type             = log.msg_type;
  this->notification_cmd     = log.notification_cmd;
  this->notification_contact = log.notification_contact;
  this->output               = log.output;
  this->retry                = log.retry;
  this->service_description  = log.service_description;
  this->status               = log.status;
  this->type                 = log.type;
  return ;
}

/**************************************
*                                     *
*          Public Methods             *
*                                     *
**************************************/

/**
 *  \brief Log default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
Log::Log() : c_time(0), msg_type(0), retry(0), status(0), type(0) {}

/**
 *  \brief Log copy constructor.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  \param[in] log Object to copy data from.
 */
Log::Log(const Log& log) : Event(log)
{
  this->InternalCopy(log);
}

/**
 *  Log destructor.
 */
Log::~Log() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  \param[in] log Object to copy data from.
 *
 *  \return *this
 */
Log& Log::operator=(const Log& log)
{
  this->Event::operator=(log);
  this->InternalCopy(log);
  return (*this);
}

/**
 *  \brief Returns the type of the event (Event::LOG).
 *
 *  This method can be useful for runtime event type identification.
 *
 *  \return Event::LOG
 */
int Log::GetType() const
{
  return (Event::LOG);
}
