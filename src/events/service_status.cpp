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

#include "events/service_status.h"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy internal members of the given object to the current instance.
 *
 *  Make a copy of all internal members defined within ServiceStatus to the
 *  current instance. This method is used by the copy constructor and the
 *  assignment operator.
 *
 *  \param[in] ss Object to copy data from.
 */
void ServiceStatus::InternalCopy(const ServiceStatus& ss)
{
  this->last_time_critical = ss.last_time_critical;
  this->last_time_ok       = ss.last_time_ok;
  this->last_time_unknown  = ss.last_time_unknown;
  this->last_time_warning  = ss.last_time_warning;
  this->service_id         = ss.service_id;
  return ;
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  \brief ServiceStatus default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
ServiceStatus::ServiceStatus()
  : last_time_critical(0),
    last_time_ok(0),
    last_time_unknown(0),
    last_time_warning(0),
    service_id(0) {}

/**
 *  \brief ServiceStatus copy constructor.
 *
 *  Copy all members of the given ServiceStatus to the current instance.
 *
 *  \param[in] ss Object to copy data from.
 */
ServiceStatus::ServiceStatus(const ServiceStatus& ss)
  : HostServiceStatus(ss)
{
  this->InternalCopy(ss);
}

/**
 *  ServiceStatus destructor.
 */
ServiceStatus::~ServiceStatus() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all members of the given ServiceStatus to the current instance.
 *
 *  \param[in] ss Object to copy data from.
 *
 *  \return *this
 */
ServiceStatus& ServiceStatus::operator=(const ServiceStatus& ss)
{
  this->HostServiceStatus::operator=(ss);
  this->InternalCopy(ss);
  return (*this);
}

/**
 *  \brief Returns the type of the event (Event::SERVICESTATUS).
 *
 *  This method is used to determine at runtime the type of event.
 *
 *  \return Event::SERVICESTATUS
 */
int ServiceStatus::GetType() const
{
  return (Event::SERVICESTATUS);
}
