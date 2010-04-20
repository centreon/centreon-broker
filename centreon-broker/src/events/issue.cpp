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

#include "events/issue.h"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy internal members.
 *
 *  This method is used by the copy constructor and the assignment operator.
 *
 *  \param[in] issue Object to copy.
 */
void Issue::InternalCopy(const Issue& issue)
{
  this->ack_time   = issue.ack_time;
  this->end_time   = issue.end_time;
  this->host_id    = issue.host_id;
  this->output     = issue.output;
  this->service_id = issue.service_id;
  this->start_time = issue.start_time;
  this->state      = issue.state;
  this->status     = issue.status;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
Issue::Issue()
  : ack_time(0),
    end_time(0),
    host_id(0),
    service_id(0),
    start_time(0),
    state(0),
    status(0) {}

/**
 *  Copy constructor.
 *
 *  \param[in] issue Object to copy.
 */
Issue::Issue(const Issue& issue) : Events::Event(issue)
{
  this->InternalCopy(issue);
}

/**
 *  Destructor.
 */
Issue::~Issue() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] issue Object to copy.
 *
 *  \return *this
 */
Issue& Issue::operator=(const Issue& issue)
{
  this->Event::operator=(issue);
  this->InternalCopy(issue);
  return (*this);
}

/**
 *  Get the type of this event (Event::ISSUE).
 *
 *  \return Event::ISSUE.
 */
int Issue::GetType() const
{
  return (Event::ISSUE);
}
