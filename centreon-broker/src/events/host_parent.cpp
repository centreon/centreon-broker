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

#include "events/host_parent.h"

using namespace Events;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
HostParent::HostParent() {}

/**
 *  Copy constructor.
 *
 *  \param[in] hp Object to copy from.
 */
HostParent::HostParent(const HostParent& hp)
  : Event(hp), host(hp.host), parent(hp.parent) {}

/**
 *  Destructor.
 */
HostParent::~HostParent() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] hp Object to copy from.
 *
 *  \return *this
 */
HostParent& HostParent::operator=(const HostParent& hp)
{
  this->host = hp.host;
  this->parent = hp.parent;
  return (*this);
}

/**
 *  Get the type of this event (Event::HOSTPARENT).
 *
 *  \return Event::HOSTPARENT
 */
int HostParent::GetType() const
{
  return (Event::HOSTPARENT);
}
