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

#include "events/service_check.h"

using namespace Events;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
ServiceCheck::ServiceCheck() {}

/**
 *  Copy constructor.
 *
 *  \param[in] service_check Object to copy from.
 */
ServiceCheck::ServiceCheck(const ServiceCheck& service_check)
  : Check(service_check) {}

/**
 *  Destructor.
 */
ServiceCheck::~ServiceCheck() {}

/**
 *  Overload of the assignment operator.
 *
 *  \param[in] service_check Object to copy from.
 *
 *  \return *this
 */
ServiceCheck& ServiceCheck::operator=(const ServiceCheck& service_check)
{
  this->Check::operator=(service_check);
  return (*this);
}

/**
 *  Get the type of this event (Event::SERVICECHECK).
 *
 *  \return Event::SERVICECHECK.
 */
int ServiceCheck::GetType() const
{
  return (Event::SERVICECHECK);
}
