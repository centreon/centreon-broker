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

#include "events/issue_update.h"

using namespace Events;

/**************************************
*                                     *
*            Private Methods          *
*                                     *
**************************************/

/**
 *  Copy internal data members from the given instance to this object.
 *
 *  \param[in] issue_update Object to copy.
 */
void IssueUpdate::InternalCopy(const IssueUpdate& issue_update)
{
  this->host_id1    = issue_update.host_id1;
  this->host_id2    = issue_update.host_id2;
  this->service_id1 = issue_update.service_id1;
  this->service_id2 = issue_update.service_id2;
  this->start_time1 = issue_update.start_time1;
  this->start_time2 = issue_update.start_time2;
  this->update      = issue_update.update;
  return ;
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
IssueUpdate::IssueUpdate()
  : host_id1(0),
    host_id2(0),
    service_id1(0),
    service_id2(0),
    start_time1(0),
    start_time2(0),
    update(UNKNOWN) {}

/**
 *  Copy constructor.
 *
 *  \param[in] issue_update Object to copy.
 */
IssueUpdate::IssueUpdate(const IssueUpdate& issue_update)
  : Events::Event(issue_update)
{
  this->InternalCopy(issue_update);
}

/**
 *  Destructor.
 */
IssueUpdate::~IssueUpdate() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] issue_update Object to copy.
 *
 *  \return *this
 */
IssueUpdate& IssueUpdate::operator=(const IssueUpdate& issue_update)
{
  this->Event::operator=(issue_update);
  this->InternalCopy(issue_update);
  return (*this);
}

/**
 *  Get the type of this event (Event::ISSUEUPDATE).
 *
 *  \return Event::ISSUEUPDATE
 */
int IssueUpdate::GetType() const
{
  return (Event::ISSUEUPDATE);
}
