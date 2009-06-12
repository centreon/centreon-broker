/*
** service.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/22/09 Matthieu Kermagoret
** Last update 06/12/09 Matthieu Kermagoret
*/

#include <cstring>
#include "service.h"

using namespace CentreonBroker;

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
  memset(this->shorts_, 0, sizeof(this->shorts_));
}

/**
 *  Build a Service from a ServiceStatus.
 */
Service::Service(const ServiceStatus& ss) : ServiceStatus(ss)
{
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
 *  Get the freshness_checks_enabled member.
 */
short Service::GetFreshnessChecksEnabled() const throw ()
{
  return (this->shorts_[FRESHNESS_CHECKS_ENABLED]);
}

/**
 *  Get the is_volatile member.
 */
short Service::GetIsVolatile() const throw ()
{
  return (this->shorts_[IS_VOLATILE]);
}

/**
 *  Get the notify_on_critical member.
 */
short Service::GetNotifyOnCritical() const throw ()
{
  return (this->shorts_[NOTIFY_ON_CRITICAL]);
}

/**
 *  Get the notify_on_unknown member.
 */
short Service::GetNotifyOnUnknown() const throw ()
{
  return (this->shorts_[NOTIFY_ON_UNKNOWN]);
}

/**
 *  Get the notify_on_warning member.
 */
short Service::GetNotifyOnWarning() const throw ()
{
  return (this->shorts_[NOTIFY_ON_WARNING]);
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
  // XXX : hardcoded value
  return (8);
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
 *  Set the freshness_checks_enabled member.
 */
void Service::SetFreshnessChecksEnabled(short fce) throw ()
{
  this->shorts_[FRESHNESS_CHECKS_ENABLED] = fce;
  return ;
}

/**
 *  Set the is_volatile member.
 */
void Service::SetIsVolatile(short iv) throw ()
{
  this->shorts_[IS_VOLATILE] = iv;
  return ;
}

/**
 *  Set the notify_on_critical member.
 */
void Service::SetNotifyOnCritical(short noc) throw ()
{
  this->shorts_[NOTIFY_ON_CRITICAL] = noc;
  return ;
}

/**
 *  Set the notify_on_unknown member.
 */
void Service::SetNotifyOnUnknown(short nou) throw ()
{
  this->shorts_[NOTIFY_ON_UNKNOWN] = nou;
  return ;
}

/**
 *  Set the notify_on_warning member.
 */
void Service::SetNotifyOnWarning(short now) throw ()
{
  this->shorts_[NOTIFY_ON_WARNING] = now;
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
