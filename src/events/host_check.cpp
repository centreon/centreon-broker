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

#include "events/host_check.h"

using namespace Events;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
HostCheck::HostCheck() {}

/**
 *  Copy constructor.
 *
 *  \param[in] host_check Object to copy from.
 */
HostCheck::HostCheck(const HostCheck& host_check) : Check(host_check) {}

/**
 *  Destructor.
 */
HostCheck::~HostCheck() {}

/**
 *  Overload of the assignment operator.
 *
 *  \param[in] host_check Object to copy from.
 *
 *  \return *this
 */
HostCheck& HostCheck::operator=(const HostCheck& host_check)
{
  this->Events::Check::operator=(host_check);
  return (*this);
}

/**
 *  Get the type of this event (Event::HOSTCHECK).
 *
 *  \return Event::HOSTCHECK.
 */
int HostCheck::GetType() const
{
  return (Event::HOSTCHECK);
}
