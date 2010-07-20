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

#include "events/host.h"

using namespace Events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief Copy all internal data of the given object to the current instance.
 *
 *  This method copy all data defined directly in the Host class. This is used
 *  by the copy constructor and the assignment operator.
 *
 *  \param[in] h Object to copy data from.
 */
void Host::InternalCopy(const Host& h)
{
  this->address                       = h.address;
  this->alias                         = h.alias;
  this->flap_detection_on_down        = h.flap_detection_on_down;
  this->flap_detection_on_unreachable = h.flap_detection_on_unreachable;
  this->flap_detection_on_up          = h.flap_detection_on_up;
  this->host_name                     = h.host_name;
  this->instance_id                   = h.instance_id;
  this->notify_on_down                = h.notify_on_down;
  this->notify_on_unreachable         = h.notify_on_unreachable;
  this->stalk_on_down                 = h.stalk_on_down;
  this->stalk_on_unreachable          = h.stalk_on_unreachable;
  this->stalk_on_up                   = h.stalk_on_up;
  this->statusmap_image               = h.statusmap_image;
  return ;
}

/**
 *  \brief Zero-initialize internal data.
 *
 *  This method is used by constructors.
 */
void Host::ZeroInitialize()
{
  this->flap_detection_on_down        = 0;
  this->flap_detection_on_unreachable = 0;
  this->flap_detection_on_up          = 0;
  this->instance_id                   = 0;
  this->notify_on_down                = 0;
  this->notify_on_unreachable         = 0;
  this->stalk_on_down                 = 0;
  this->stalk_on_unreachable          = 0;
  this->stalk_on_up                   = 0;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Host default constructor.
 *
 *  Initialize internal data to 0, NULL or equivalent.
 */
Host::Host()
{
  this->ZeroInitialize();
}

/**
 *  \brief Build a Host from a HostStatus.
 *
 *  Copy HostStatus data to the current instance and zero-initialize other
 *  members.
 *
 *  \param[in] hs HostStatus object to initialize part of the Host instance.
 */
Host::Host(const HostStatus& hs) : HostStatus(hs)
{
  this->ZeroInitialize();
}

/**
 *  \brief Host copy constructor.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] h Object to copy data from.
 */
Host::Host(const Host& h) : HostService(h), HostStatus(h)
{
  this->InternalCopy(h);
}

/**
 *  Host destructor.
 */
Host::~Host() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] h Object to copy data from.
 *
 *  \return *this
 */
Host& Host::operator=(const Host& h)
{
  this->HostService::operator=(h);
  this->HostStatus::operator=(h);
  this->InternalCopy(h);
  return (*this);
}

/**
 *  \brief Get the type of this event (Event::HOST).
 *
 *  This will help for runtime event type identification.
 *
 *  \return Event::HOST
 */
int Host::GetType() const
{
  return (Event::HOST);
}
