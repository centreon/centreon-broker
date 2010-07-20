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

#include "events/downtime.h"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy internal data of the given object to the current instance.
 *
 *  This internal method is used to copy data defined inside the Downtime class
 *  from an object to the current instance. This means that no superclass data
 *  are copied. This method is used in Downtime copy constructor and in the =
 *  operator overload.
 *
 *  \param[in] downtime Object to copy from.
 *
 *  \see Downtime(const Downtime&)
 *  \see operator=(const Downtime&)
 */
void Downtime::InternalCopy(const Downtime& downtime)
{
  this->author              = downtime.author;
  this->comment             = downtime.comment;
  this->downtime_type       = downtime.downtime_type;
  this->duration            = downtime.duration;
  this->end_time            = downtime.end_time;
  this->entry_time          = downtime.entry_time;
  this->fixed               = downtime.fixed;
  this->host_name           = downtime.host_name;
  this->internal_id         = downtime.internal_id;
  this->instance_name       = downtime.instance_name;
  this->service_description = downtime.service_description;
  this->start_time          = downtime.start_time;
  this->triggered_by        = downtime.triggered_by;
  this->was_cancelled       = downtime.was_cancelled;
  this->was_started         = downtime.was_started;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Downtime default constructor.
 *
 *  Set all members to their default value (0, NULL or equivalent).
 */
Downtime::Downtime()
  : downtime_type(0),
    duration(0),
    end_time(0),
    entry_time(0),
    fixed(false),
    internal_id(0),
    start_time(0),
    triggered_by(0),
    was_cancelled(false),
    was_started(false) {}

/**
 *  \brief Downtime copy constructor.
 *
 *  Copy internal data of the downtime object to the current instance.
 *
 *  \param[in] downtime Object to copy from.
 */
Downtime::Downtime(const Downtime& downtime) : Event(downtime)
{
  this->InternalCopy(downtime);
}

/**
 *  Downtime destructor.
 */
Downtime::~Downtime() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy internal data of the downtime object to the current instance.
 *
 *  \param[in] downtime Object to copy from.
 *
 *  \return *this
 */
Downtime& Downtime::operator=(const Downtime& downtime)
{
  this->Event::operator=(downtime);
  this->InternalCopy(downtime);
  return (*this);
}

/**
 *  \brief Get the event's type.
 *
 *  Returns the type of this event (Event::DOWNTIME). This can be useful for
 *  runtime event type identification.
 *
 *  \return Event::DOWNTIME
 */
int Downtime::GetType() const
{
  return (Event::DOWNTIME);
}
