/*
** service_status.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 06/04/09 Matthieu Kermagoret
*/

#include <cstring>
#include <string>
#include "service_status.h"

using namespace CentreonBroker;

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
  // XXX : hardcoded value
  return (1);
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
