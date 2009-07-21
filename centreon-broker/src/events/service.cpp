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
#include "events/service.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy all internal data of the Service structure to the current instance.
 */
void Service::InternalCopy(const Service& s)
{
  memcpy(this->bools_, s.bools_, sizeof(this->bools_));
  memcpy(this->shorts_, s.shorts_, sizeof(this->shorts_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = s.strings_[i];
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Service default constructor.
 */
Service::Service() throw ()
{
  memset(this->bools_, 0, sizeof(this->bools_));
  memset(this->shorts_, 0, sizeof(this->shorts_));
}

/**
 *  Build a Service from a ServiceStatus.
 */
Service::Service(const ServiceStatus& ss) : ServiceStatus(ss)
{
  memset(this->bools_, 0, sizeof(this->bools_));
  memset(this->shorts_, 0, sizeof(this->shorts_));
}

/**
 *  Service copy constructor.
 */
Service::Service(const Service& s) : HostService(s), ServiceStatus(s)
{
  this->InternalCopy(s);
}

/**
 *  Service destructor.
 */
Service::~Service() throw ()
{
}

/**
 *  Service operator= overload.
 */
Service& Service::operator=(const Service& s)
{
  this->HostService::operator=(s);
  this->ServiceStatus::operator=(s);
  this->InternalCopy(s);
  return (*this);
}

/**
 *  Get the failure_prediction_options member.
 */
const std::string& Service::GetFailurePredictionOptions() const throw ()
{
  return (this->strings_[FAILURE_PREDICTION_OPTIONS]);
}

/**
 *  Get the flap_detection_on_critical member.
 */
short Service::GetFlapDetectionOnCritical() const throw ()
{
  return (this->shorts_[FLAP_DETECTION_ON_CRITICAL]);
}

/**
 *  Get the flap_detection_on_ok member.
 */
short Service::GetFlapDetectionOnOk() const throw ()
{
  return (this->shorts_[FLAP_DETECTION_ON_OK]);
}

/**
 *  Get the flap_detection_on_unknown member.
 */
short Service::GetFlapDetectionOnUnknown() const throw ()
{
  return (this->shorts_[FLAP_DETECTION_ON_UNKNOWN]);
}

/**
 *  Get the flap_detection_on_warning member.
 */
short Service::GetFlapDetectionOnWarning() const throw ()
{
  return (this->shorts_[FLAP_DETECTION_ON_WARNING]);
}

/**
 *  Get the is_volatile member.
 */
bool Service::GetIsVolatile() const throw ()
{
  return (this->bools_[IS_VOLATILE]);
}

/**
 *  Get the notified_on_critical member.
 */
bool Service::GetNotifiedOnCritical() const throw ()
{
  return (this->bools_[NOTIFIED_ON_CRITICAL]);
}

/**
 *  Get the notified_on_unknown member.
 */
bool Service::GetNotifiedOnUnknown() const throw ()
{
  return (this->bools_[NOTIFIED_ON_UNKNOWN]);
}

/**
 *  Get the notified_on_warning member.
 */
bool Service::GetNotifiedOnWarning() const throw ()
{
  return (this->bools_[NOTIFIED_ON_WARNING]);
}

/**
 *  Get the stalk_on_critical member.
 */
short Service::GetStalkOnCritical() const throw ()
{
  return (this->shorts_[STALK_ON_CRITICAL]);
}

/**
 *  Get the stalk_on_ok member.
 */
short Service::GetStalkOnOk() const throw ()
{
  return (this->shorts_[STALK_ON_OK]);
}

/**
 *  Get the stalk_on_unknown member.
 */
short Service::GetStalkOnUnknown() const throw ()
{
  return (this->shorts_[STALK_ON_UNKNOWN]);
}

/**
 *  Get the stalk_on_warning member.
 */
short Service::GetStalkOnWarning() const throw ()
{
  return (this->shorts_[STALK_ON_WARNING]);
}

/**
 *  Get the type of the event.
 */
int Service::GetType() const throw ()
{
  return (Event::SERVICE);
}

/**
 *  Set the failure_prediction_options member.
 */
void Service::SetFailurePredictionOptions(const std::string& fpo)
{
  this->strings_[FAILURE_PREDICTION_OPTIONS] = fpo;
  return ;
}

/**
 *  Set the flap_detection_on_critical member.
 */
void Service::SetFlapDetectionOnCritical(short fdoc) throw ()
{
  this->shorts_[FLAP_DETECTION_ON_CRITICAL] = fdoc;
  return ;
}

/**
 *  Set the flap_detection_on_ok member.
 */
void Service::SetFlapDetectionOnOk(short fdoo) throw ()
{
  this->shorts_[FLAP_DETECTION_ON_OK] = fdoo;
  return ;
}

/**
 *  Set the flap_detection_on_unknown member.
 */
void Service::SetFlapDetectionOnUnknown(short fdou) throw ()
{
  this->shorts_[FLAP_DETECTION_ON_UNKNOWN] = fdou;
  return ;
}

/**
 *  Set the flap_detection_on_warning member.
 */
void Service::SetFlapDetectionOnWarning(short fdow) throw ()
{
  this->shorts_[FLAP_DETECTION_ON_WARNING] = fdow;
  return ;
}

/**
 *  Set the is_volatile member.
 */
void Service::SetIsVolatile(bool iv) throw ()
{
  this->bools_[IS_VOLATILE] = iv;
  return ;
}

/**
 *  Set the notified_on_critical member.
 */
void Service::SetNotifiedOnCritical(bool noc) throw ()
{
  this->bools_[NOTIFIED_ON_CRITICAL] = noc;
  return ;
}

/**
 *  Set the notified_on_unknown member.
 */
void Service::SetNotifiedOnUnknown(bool nou) throw ()
{
  this->bools_[NOTIFIED_ON_UNKNOWN] = nou;
  return ;
}

/**
 *  Set the notified_on_warning member.
 */
void Service::SetNotifiedOnWarning(bool now) throw ()
{
  this->bools_[NOTIFIED_ON_WARNING] = now;
  return ;
}

/**
 *  Set the stalk_on_critical member.
 */
void Service::SetStalkOnCritical(short soc) throw ()
{
  this->shorts_[STALK_ON_CRITICAL] = soc;
  return ;
}

/**
 *  Set the stalk_on_ok member.
 */
void Service::SetStalkOnOk(short soo) throw ()
{
  this->shorts_[STALK_ON_OK] = soo;
  return ;
}

/**
 *  Set the stalk_on_unknown member.
 */
void Service::SetStalkOnUnknown(short sou) throw ()
{
  this->shorts_[STALK_ON_UNKNOWN] = sou;
  return ;
}

/**
 *  Set the stalk_on_warning member.
 */
void Service::SetStalkOnWarning(short sow) throw ()
{
  this->shorts_[STALK_ON_WARNING] = sow;
  return ;
}
