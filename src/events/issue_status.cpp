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

#include "events/issue_status.h"

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
 *  \param[in] issue_status Object to copy.
 */
void IssueStatus::InternalCopy(const IssueStatus& issue_status)
{
  this->ack_time   = issue_status.ack_time;
  this->host_id    = issue_status.host_id;
  this->output     = issue_status.output;
  this->service_id = issue_status.service_id;
  this->start_time = issue_status.start_time;
  this->state      = issue_status.state;
  this->status     = issue_status.status;
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Constructor.
 */
IssueStatus::IssueStatus()
  : ack_time(0),
    host_id(0),
    service_id(0),
    start_time(0),
    state(0),
    status(0) {}

/**
 *  Copy constructor.
 *
 *  \param[in] issue_status Object to copy.
 */
IssueStatus::IssueStatus(const IssueStatus& issue_status)
  : Events::Event(issue_status)
{
  this->InternalCopy(issue_status);
}

/**
 *  Destructor.
 */
IssueStatus::~IssueStatus() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] issue_status Object to copy.
 *
 *  \return *this
 */
IssueStatus& IssueStatus::operator=(const IssueStatus& issue_status)
{
  this->Event::operator=(issue_status);
  this->InternalCopy(issue_status);
  return (*this);
}

/**
 *  Get the type of this event (Event::ISSUESTATUS).
 *
 *  \return Event::ISSUESTATUS.
 */
int IssueStatus::GetType() const
{
  return (Event::ISSUESTATUS);
}
