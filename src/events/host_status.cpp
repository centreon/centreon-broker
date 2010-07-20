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

#include "events/host_status.h"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy all internal members of the given object to the current
 *         instance.
 *
 *  Make a copy of all internal members of HostStatus to the current instance.
 *  This method is use by the copy constructor and the assignment operator.
 *
 *  \param[in] hs Object to copy data from.
 */
void HostStatus::InternalCopy(const HostStatus& hs)
{
  this->last_time_down        = hs.last_time_down;
  this->last_time_unreachable = hs.last_time_unreachable;
  this->last_time_up          = hs.last_time_up;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief HostStatus constructor.
 *
 *  Initialize all members to 0, NULL or equivalent.
 */
HostStatus::HostStatus()
  : last_time_down(0),
    last_time_unreachable(0),
    last_time_up(0) {}

/**
 *  \brief HostStatus copy constructor.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  \param[in] hs Object to copy data from.
 */
HostStatus::HostStatus(const HostStatus& hs)
  : HostServiceStatus(hs)
{
  this->InternalCopy(hs);
}

/**
 *  HostStatus destructor.
 */
HostStatus::~HostStatus() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  \param[in] hs Object to copy data from.
 *
 *  \return *this
 */
HostStatus& HostStatus::operator=(const HostStatus& hs)
{
  this->HostServiceStatus::operator=(hs);
  this->InternalCopy(hs);
  return (*this);
}

/**
 *  \brief Returns the type of the event (Event::HOSTSTATUS).
 *
 *  The type of the event can be useful for runtime event type identification.
 *
 *  \return Event::HOSTSTATUS
 */
int HostStatus::GetType() const
{
  return (Event::HOSTSTATUS);
}
