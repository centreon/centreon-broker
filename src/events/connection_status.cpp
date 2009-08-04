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

#include "events/connection_status.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief Copy members of the ConnectionStatus object to the current instance.
 *
 *  Copy all members defined within the ConnectionStatus class. This method is
 *  used by the copy constructor and the assignment operator.
 *
 *  \param[in] cs Object to copy data from.
 */
void ConnectionStatus::InternalCopy(const ConnectionStatus& cs) throw ()
{
  this->bytes_processed   = cs.bytes_processed;
  this->data_end_time     = cs.data_end_time;
  this->disconnect_time   = cs.disconnect_time;
  this->entries_processed = cs.entries_processed;
  this->last_checkin_time = cs.last_checkin_time;
  this->lines_processed   = cs.lines_processed;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief ConnectionStatus default constructor.
 *
 *  Initialize all members to 0, NULL or equivalent.
 */
ConnectionStatus::ConnectionStatus()
  : bytes_processed(0),
    data_end_time(0),
    disconnect_time(0),
    entries_processed(0),
    last_checkin_time(0),
    lines_processed(0) {}

/**
 *  \brief ConnectionStatus copy constructor.
 *
 *  Copy all data of the given object to the current instance.
 *
 *  \param[in] cs Object to copy data from.
 */
ConnectionStatus::ConnectionStatus(const ConnectionStatus& cs)
  : Event(cs)
{
  this->InternalCopy(cs);
}

/**
 *  ConnectionStatus destructor.
 */
ConnectionStatus::~ConnectionStatus() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all data of the given object to the current instance.
 *
 *  \param[in] cs Object to copy data from.
 */
ConnectionStatus& ConnectionStatus::operator=(const ConnectionStatus& cs)
{
  this->Event::operator=(cs);
  this->InternalCopy(cs);
  return (*this);
}

/**
 *  \brief Get the type of the event (Event::CONNECTIONSTATUS).
 *
 *  This method is used to determine the type of the event at runtime.
 *
 *  \return Event::CONNECTIONSTATUS
 */
int ConnectionStatus::GetType() const throw ()
{
  return (Event::CONNECTIONSTATUS);
}
