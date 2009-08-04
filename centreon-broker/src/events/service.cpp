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

#include "events/service.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy members of the Service object to the current instance.
 *
 *  Copy all members defined within the Service class. This method is used by
 *  the copy constructor and the assignment operator.
 *
 *  \param[in] s Object to copy data from.
 */
void Service::InternalCopy(const Service& s)
{
  this->failure_prediction_options = s.failure_prediction_options;
  this->flap_detection_on_critical = s.flap_detection_on_critical;
  this->flap_detection_on_ok       = s.flap_detection_on_ok;
  this->flap_detection_on_unknown  = s.flap_detection_on_unknown;
  this->is_volatile                = s.is_volatile;
  this->notified_on_critical       = s.notified_on_critical;
  this->notified_on_unknown        = s.notified_on_unknown;
  this->notified_on_warning        = s.notified_on_warning;
  this->stalk_on_critical          = s.stalk_on_critical;
  this->stalk_on_unknown           = s.stalk_on_unknown;
  this->stalk_on_warning           = s.stalk_on_warning;
  return ;
}

/**
 *  \brief Initialize members to 0, NULL or equivalent.
 *
 *  This method initializes members defined within the Service class to 0,
 *  NULL or equivalent. This method is used by some constructors.
 */
void Service::ZeroInitialize()
{
  this->flap_detection_on_critical = 0;
  this->flap_detection_on_ok = 0;
  this->flap_detection_on_unknown = 0;
  this->flap_detection_on_warning = 0;
  this->is_volatile = false;
  this->notified_on_critical = false;
  this->notified_on_unknown = false;
  this->notified_on_warning = false;
  this->stalk_on_critical = 0;
  this->stalk_on_ok = 0;
  this->stalk_on_unknown = 0;
  this->stalk_on_warning = 0;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Service default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
Service::Service()
{
  this->ZeroInitialize();
}

/**
 *  \brief Build a Service from a ServiceStatus.
 *
 *  Copy all members from ServiceStatus to the current instance and
 *  zero-initialize remaining members.
 *
 *  \param[in] ss ServiceStatus object to copy data from.
 */
Service::Service(const ServiceStatus& ss) : ServiceStatus(ss)
{
  this->ZeroInitialize();
}

/**
 *  \brief Service copy constructor.
 *
 *  Copy all members of the given Service object to the current instance.
 *
 *  \param[in] s Object to copy data from.
 */
Service::Service(const Service& s) : HostService(s), ServiceStatus(s)
{
  this->InternalCopy(s);
}

/**
 *  Service destructor.
 */
Service::~Service() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all members of the given Service object to the current instance.
 *
 *  \param[in] s Object to copy data from.
 *
 *  \return *this
 */
Service& Service::operator=(const Service& s)
{
  this->HostService::operator=(s);
  this->ServiceStatus::operator=(s);
  this->InternalCopy(s);
  return (*this);
}

/**
 *  \brief Get the type of the event (Event::SERVICE).
 *
 *  This method is used to determine the type of the event at runtime.
 *
 *  \return Event::SERVICE
 */
int Service::GetType() const throw ()
{
  return (Event::SERVICE);
}
