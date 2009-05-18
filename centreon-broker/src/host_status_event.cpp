/*
** host_status_event.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/07/09 Matthieu Kermagoret
** Last update 05/18/09 Matthieu Kermagoret
*/

#include <cstring>
#include <string>
#include "host_status_event.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Make a copy of all internal members of HostStatusEvent to the current
 *  instance.
 */
void HostStatusEvent::InternalCopy(const HostStatusEvent& hse)
{
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = hse.strings_[i];
  memcpy(this->timets_, hse.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  HostStatusEvent constructor.
 */
HostStatusEvent::HostStatusEvent()
{
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  HostStatusEvent copy constructor.
 */
HostStatusEvent::HostStatusEvent(const HostStatusEvent& hse)
  : HostServiceStatusEvent(hse)
{
  this->InternalCopy(hse);
}

/**
 *  HostStatusEvent destructor.
 */
HostStatusEvent::~HostStatusEvent()
{
}

/**
 *  HostStatusEvent operator= overload.
 */
HostStatusEvent& HostStatusEvent::operator=(const HostStatusEvent& hse)
{
  this->HostServiceStatusEvent::operator=(hse);
  this->InternalCopy(hse);
  return (*this);
}

/**
 *  Returns the type of the event.
 */
int HostStatusEvent::GetType() const throw ()
{
  // XXX : hardcoded value
  return (0);
}

/**
 *  Get the host member.
 */
const std::string& HostStatusEvent::GetHost() const throw ()
{
  return (this->strings_[HOST]);
}

/**
 *  Get the last_time_up member.
 */
time_t HostStatusEvent::GetLastTimeUp() const throw ()
{
  return (this->timets_[LAST_TIME_UP]);
}

/**
 *  Get the last_time_down member.
 */
time_t HostStatusEvent::GetLastTimeDown() const throw ()
{
  return (this->timets_[LAST_TIME_DOWN]);
}

/**
 *  Get the last_time_unreachable member.
 */
time_t HostStatusEvent::GetLastTimeUnreachable() const throw ()
{
  return (this->timets_[LAST_TIME_UNREACHABLE]);
}

/**
 *  Set the host member.
 */
void HostStatusEvent::SetHost(const std::string& h)
{
  this->strings_[HOST] = h;
  return ;
}

/**
 *  Set the last_time_up member.
 */
void HostStatusEvent::SetLastTimeUp(time_t ltu) throw ()
{
  this->timets_[LAST_TIME_UP] = ltu;
  return ;
}

/**
 *  Set the last_time_down member.
 */
void HostStatusEvent::SetLastTimeDown(time_t ltd) throw ()
{
  this->timets_[LAST_TIME_DOWN] = ltd;
  return ;
}

/**
 *  Set the last_time_unreachable member.
 */
void HostStatusEvent::SetLastTimeUnreachable(time_t ltu) throw ()
{
  this->timets_[LAST_TIME_UNREACHABLE] = ltu;
  return ;
}
