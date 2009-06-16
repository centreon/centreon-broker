/*
** host_status.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/07/09 Matthieu Kermagoret
** Last update 06/16/09 Matthieu Kermagoret
*/

#include <cstring>
#include "host_status.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Make a copy of all internal members of HostStatus to the current
 *  instance.
 */
void HostStatus::InternalCopy(const HostStatus& hse)
{
  memcpy(this->timets_, hse.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  HostStatus constructor.
 */
HostStatus::HostStatus()
{
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  HostStatus copy constructor.
 */
HostStatus::HostStatus(const HostStatus& hse)
  : HostServiceStatus(hse)
{
  this->InternalCopy(hse);
}

/**
 *  HostStatus destructor.
 */
HostStatus::~HostStatus()
{
}

/**
 *  HostStatus operator= overload.
 */
HostStatus& HostStatus::operator=(const HostStatus& hse)
{
  this->HostServiceStatus::operator=(hse);
  this->InternalCopy(hse);
  return (*this);
}

/**
 *  Get the last_time_up member.
 */
time_t HostStatus::GetLastTimeUp() const throw ()
{
  return (this->timets_[LAST_TIME_UP]);
}

/**
 *  Get the last_time_down member.
 */
time_t HostStatus::GetLastTimeDown() const throw ()
{
  return (this->timets_[LAST_TIME_DOWN]);
}

/**
 *  Get the last_time_unreachable member.
 */
time_t HostStatus::GetLastTimeUnreachable() const throw ()
{
  return (this->timets_[LAST_TIME_UNREACHABLE]);
}

/**
 *  Returns the type of the event.
 */
int HostStatus::GetType() const throw ()
{
  return (Event::HOSTSTATUS);
}

/**
 *  Set the last_time_up member.
 */
void HostStatus::SetLastTimeUp(time_t ltu) throw ()
{
  this->timets_[LAST_TIME_UP] = ltu;
  return ;
}

/**
 *  Set the last_time_down member.
 */
void HostStatus::SetLastTimeDown(time_t ltd) throw ()
{
  this->timets_[LAST_TIME_DOWN] = ltd;
  return ;
}

/**
 *  Set the last_time_unreachable member.
 */
void HostStatus::SetLastTimeUnreachable(time_t ltu) throw ()
{
  this->timets_[LAST_TIME_UNREACHABLE] = ltu;
  return ;
}
