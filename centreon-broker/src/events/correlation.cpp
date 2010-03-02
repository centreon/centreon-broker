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

#include "events/correlation.h"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy data members to the current instance.
 *
 *  This method is used by the copy constructor and the assignment operator.
 *
 *  \param[in] correlation Object to copy from.
 */
void Correlation::InternalCopy(const Correlation& correlation)
{
  this->ack_time   = correlation.ack_time;
  this->end_time   = correlation.end_time;
  this->host_id    = correlation.host_id;
  this->output     = correlation.output;
  this->service_id = correlation.service_id;
  this->start_time = correlation.start_time;
  this->state      = correlation.state;
  this->status     = correlation.status;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
Correlation::Correlation()
  : ack_time(0),
    end_time(0),
    host_id(0),
    service_id(0),
    state(0),
    status(0) {}

/**
 *  Copy constructor.
 *
 *  \param[in] correlation Object to build from.
 */
Correlation::Correlation(const Correlation& correlation) : Event(correlation)
{
  this->InternalCopy(correlation);
}

/**
 *  Destructor.
 */
Correlation::~Correlation() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] correlation Object to copy from.
 *
 *  \return *this.
 */
Correlation& Correlation::operator=(const Correlation& correlation)
{
  this->Event::operator=(correlation);
  this->InternalCopy(correlation);
  return (*this);
}

/**
 *  Get the type of this event (Event::CORRELATION).
 *
 *  \return Event::CORRELATION.
 */
int Correlation::GetType() const
{
  return (Event::CORRELATION);
}
