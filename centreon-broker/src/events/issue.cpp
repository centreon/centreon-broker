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
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
Issue::Issue() : end_time(0) {}

/**
 *  Copy constructor.
 *
 *  \param[in] issue Object to build from.
 */
Issue::Issue(const Issue& issue) : IssueStatus(issue)
{
  this->end_time = issue.end_time;
}

/**
 *  Destructor.
 */
Issue::~Issue() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] issue Object to copy from.
 *
 *  \return *this.
 */
Issue& Issue::operator=(const Issue& issue)
{
  this->IssueStatus::operator=(issue);
  this->end_time = issue.end_time;
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
