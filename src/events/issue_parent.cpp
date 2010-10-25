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

#include "events/issue_parent.h"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members to this object.
 *
 *  @param[in] issue_parent Object to copy.
 */
void IssueParent::InternalCopy(const IssueParent& issue_parent)
{
  this->child_host_id     = issue_parent.child_host_id;
  this->child_service_id  = issue_parent.child_service_id;
  this->child_start_time  = issue_parent.child_start_time;
  this->end_time          = issue_parent.end_time;
  this->parent_host_id    = issue_parent.parent_host_id;
  this->parent_service_id = issue_parent.parent_service_id;
  this->parent_start_time = issue_parent.parent_start_time;
  this->start_time        = issue_parent.start_time;
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
IssueParent::IssueParent()
  : child_host_id(0),
    child_service_id(0),
    child_start_time(0),
    end_time(0),
    parent_host_id(0),
    parent_service_id(0),
    parent_start_time(0),
    start_time(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] issue_parent Object to copy.
 */
IssueParent::IssueParent(const IssueParent& issue_parent)
  : Event(issue_parent)
{
  this->InternalCopy(issue_parent);
}

/**
 *  Destructor.
 */
IssueParent::~IssueParent() {}

/**
 *  Assignment operator.
 *
 *  @param[in] issue_parent Object to copy.
 *
 *  @return This object.
 */
IssueParent& IssueParent::operator=(const IssueParent& issue_parent)
{
  Event::operator=(issue_parent);
  this->InternalCopy(issue_parent);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return ISSUEPARENT.
 */
int IssueParent::GetType() const
{
  return (ISSUEPARENT);
}
