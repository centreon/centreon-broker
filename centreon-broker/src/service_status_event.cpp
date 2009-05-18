/*
** service_status_event.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 05/18/09 Matthieu Kermagoret
*/

#include <cstring>
#include <string>
#include "service_status_event.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Make a copy of all internal members of ServiceStatusEvent to the current
 *  instance.
 */
void ServiceStatusEvent::InternalCopy(const ServiceStatusEvent& sse)
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
 *  ServiceStatusEvent default constructor.
 */
ServiceStatusEvent::ServiceStatusEvent()
{
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  ServiceStatusEvent copy constructor.
 */
ServiceStatusEvent::ServiceStatusEvent(const ServiceStatusEvent& sse)
  : HostServiceStatusEvent(sse)
{
  this->InternalCopy(sse);
}

/**
 *  ServiceStatusEvent destructor.
 */
ServiceStatusEvent::~ServiceStatusEvent()
{
}

/**
 *  ServiceStatusEvent operator= overload.
 */
ServiceStatusEvent& ServiceStatusEvent::operator=(const ServiceStatusEvent& s)
{
  this->HostServiceStatusEvent::operator=(s);
  this->InternalCopy(s);
  return (*this);
}

/**
 *  Returns the type of the event.
 */
int ServiceStatusEvent::GetType() const throw ()
{
  // XXX : hardcoded value
  return (1);
}

/**
 *  Returns the host.
 */
const std::string& ServiceStatusEvent::GetHost() const throw ()
{
  return (this->strings_[HOST]);
}

/**
 *  Get the last_time_critical member.
 */
time_t ServiceStatusEvent::GetLastTimeCritical() const throw ()
{
  return (this->timets_[LAST_TIME_CRITICAL]);
}

/**
 *  Get the last_time_ok member.
 */
time_t ServiceStatusEvent::GetLastTimeOk() const throw ()
{
  return (this->timets_[LAST_TIME_OK]);
}

/**
 *  Get the last_time_unknown member.
 */
time_t ServiceStatusEvent::GetLastTimeUnknown() const throw ()
{
  return (this->timets_[LAST_TIME_UNKNOWN]);
}

/**
 *  Get the last_time_warning member.
 */
time_t ServiceStatusEvent::GetLastTimeWarning() const throw ()
{
  return (this->timets_[LAST_TIME_WARNING]);
}

/**
 *  Returns the service.
 */
const std::string& ServiceStatusEvent::GetService() const throw ()
{
  return (this->strings_[SERVICE]);
}

/**
 *  Sets the host on which the event appeared.
 */
void ServiceStatusEvent::SetHost(const std::string& h)
{
  this->strings_[HOST] = h;
  return ;
}

/**
 *  Set the last_time_critical member.
 */
void ServiceStatusEvent::SetLastTimeCritical(time_t ltc) throw ()
{
  this->timets_[LAST_TIME_CRITICAL] = ltc;
  return ;
}

/**
 *  Set the last_time_ok member.
 */
void ServiceStatusEvent::SetLastTimeOk(time_t lto) throw ()
{
  this->timets_[LAST_TIME_OK] = lto;
  return ;
}

/**
 *  Set the last_time_unknown member.
 */
void ServiceStatusEvent::SetLastTimeUnknown(time_t ltu) throw ()
{
  this->timets_[LAST_TIME_UNKNOWN] = ltu;
  return ;
}

/**
 *  Set the last_time_warning member.
 */
void ServiceStatusEvent::SetLastTimeWarning(time_t ltw) throw ()
{
  this->timets_[LAST_TIME_WARNING] = ltw;
  return ;
}

/**
 *  Sets the service to which this event refers.
 */
void ServiceStatusEvent::SetService(const std::string& s)
{
  this->strings_[SERVICE] = s;
  return ;
}
