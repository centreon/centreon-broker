/*
** host_service_status_event.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/15/09 Matthieu Kermagoret
** Last update 05/15/09 Matthieu Kermagoret
*/

#include <cstring>
#include "host_service_status_event.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Make a copy of all internal members of HostServiceStatusEvent to the
 *  current instance.
 */
void HostServiceStatusEvent::InternalCopy(const HostServiceStatusEvent& hsse)
{
  for (unsigned int i = 0; i < DOUBLE_NB; i++)
    this->doubles_[i] = hsse.doubles_[i];
  memcpy(this->ints_, hsse.ints_, sizeof(this->ints_));
  memcpy(this->shorts_, hsse.shorts_, sizeof(this->shorts_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = hsse.strings_[i];
  memcpy(this->timets_, hsse.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  HostServiceStatusEvent default constructor.
 */
HostServiceStatusEvent::HostServiceStatusEvent()
{
  for (unsigned int i = 0; i < DOUBLE_NB; i++)
    this->doubles_[i] = 0.0;
  memset(this->ints_, 0, sizeof(this->ints_));
  memset(this->shorts_, 0, sizeof(this->shorts_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  HostServiceStatusEvent copy constructor.
 */
HostServiceStatusEvent::HostServiceStatusEvent(const HostServiceStatusEvent& h)
  : StatusEvent(h)
{
  this->InternalCopy(h);
}

/**
 *  HostServiceStatusEvent destructor.
 */
HostServiceStatusEvent::~HostServiceStatusEvent()
{
}

/**
 *  HostServiceStatusEvent operator= overload.
 */
HostServiceStatusEvent& HostServiceStatusEvent::operator=(const
  HostServiceStatusEvent& hsse)
{
  this->StatusEvent::operator=(hsse);
  this->InternalCopy(hsse);
  return (*this);
}
