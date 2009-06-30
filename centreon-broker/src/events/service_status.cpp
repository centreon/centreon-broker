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

#include <cstring>
#include <string>
#include "events/service_status.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Make a copy of all internal members of ServiceStatus to the current
 *  instance.
 */
void ServiceStatus::InternalCopy(const ServiceStatus& sse)
{
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = sse.strings_[i];
  memcpy(this->timets_, sse.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  ServiceStatus default constructor.
 */
ServiceStatus::ServiceStatus()
{
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  ServiceStatus copy constructor.
 */
ServiceStatus::ServiceStatus(const ServiceStatus& sse)
  : HostServiceStatus(sse)
{
  this->InternalCopy(sse);
}

/**
 *  ServiceStatus destructor.
 */
ServiceStatus::~ServiceStatus()
{
}

/**
 *  ServiceStatus operator= overload.
 */
ServiceStatus& ServiceStatus::operator=(const ServiceStatus& s)
{
  this->HostServiceStatus::operator=(s);
  this->InternalCopy(s);
  return (*this);
}

/**
 *  Returns the type of the event.
 */
int ServiceStatus::GetType() const throw ()
{
  return (Event::SERVICESTATUS);
}

/**
 *  Get the last_time_critical member.
 */
time_t ServiceStatus::GetLastTimeCritical() const throw ()
{
  return (this->timets_[LAST_TIME_CRITICAL]);
}

/**
 *  Get the last_time_ok member.
 */
time_t ServiceStatus::GetLastTimeOk() const throw ()
{
  return (this->timets_[LAST_TIME_OK]);
}

/**
 *  Get the last_time_unknown member.
 */
time_t ServiceStatus::GetLastTimeUnknown() const throw ()
{
  return (this->timets_[LAST_TIME_UNKNOWN]);
}

/**
 *  Get the last_time_warning member.
 */
time_t ServiceStatus::GetLastTimeWarning() const throw ()
{
  return (this->timets_[LAST_TIME_WARNING]);
}

/**
 *  Returns the service_description member.
 */
const std::string& ServiceStatus::GetServiceDescription() const throw ()
{
  return (this->strings_[SERVICE_DESCRIPTION]);
}

/**
 *  Set the last_time_critical member.
 */
void ServiceStatus::SetLastTimeCritical(time_t ltc) throw ()
{
  this->timets_[LAST_TIME_CRITICAL] = ltc;
  return ;
}

/**
 *  Set the last_time_ok member.
 */
void ServiceStatus::SetLastTimeOk(time_t lto) throw ()
{
  this->timets_[LAST_TIME_OK] = lto;
  return ;
}

/**
 *  Set the last_time_unknown member.
 */
void ServiceStatus::SetLastTimeUnknown(time_t ltu) throw ()
{
  this->timets_[LAST_TIME_UNKNOWN] = ltu;
  return ;
}

/**
 *  Set the last_time_warning member.
 */
void ServiceStatus::SetLastTimeWarning(time_t ltw) throw ()
{
  this->timets_[LAST_TIME_WARNING] = ltw;
  return ;
}

/**
 *  Sets the service_description member.
 */
void ServiceStatus::SetServiceDescription(const std::string& sd)
{
  this->strings_[SERVICE_DESCRIPTION] = sd;
  return ;
}
