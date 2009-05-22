/*
** host.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/22/09 Matthieu Kermagoret
** Last update 05/22/09 Matthieu Kermagoret
*/

#include <cstring>
#include "host.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Copy all internal data of the Host object to the current instance.
 */
void Host::InternalCopy(const Host& h)
{
  memcpy(this->shorts_, h.shorts_, sizeof(this->shorts_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = h.strings_[i];
  memcpy(this->timets_, h.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Host default constructor.
 */
Host::Host() throw ()
{
  memset(this->shorts_, 0, sizeof(this->shorts_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  Host copy constructor.
 */
Host::Host(const Host& h) : HostService(h), HostServiceStatus(h)
{
  this->InternalCopy(h);
}

/**
 *  Host destructor.
 */
Host::~Host() throw ()
{
}

/**
 *  Host operator= overload.
 */
Host& Host::operator=(const Host& h)
{
  this->HostService::operator=(h);
  this->HostServiceStatus::operator=(h);
  this->InternalCopy(h);
  return (*this);
}
