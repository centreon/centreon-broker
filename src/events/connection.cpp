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

#include "events/connection.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief Copy members from the Connection object to the current instance.
 *
 *  Copy all members defined within the Connection class. This method is used
 *  by the copy constructor and the assignment operator.
 *
 *  \param[in] c Object to copy data from.
 */
void Connection::InternalCopy(const Connection& c)
{
  this->agent_name      = c.agent_name;
  this->agent_version   = c.agent_version;
  this->connect_source  = c.connect_source;
  this->connect_time    = c.connect_time;
  this->connect_type    = c.connect_type;
  this->data_start_time = c.data_start_time;
  return ;
}

/**
 *  \brief Initialize members to 0, NULL or equivalent.
 *
 *  All members defined within the Connection class will be initialized to 0,
 *  NULL or equivalent. This method is used by some constructors.
 */
void Connection::ZeroInitialize()
{
  this->connect_time = 0;
  this->data_start_time = 0;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Connection default constructor.
 *
 *  Initialize all members to 0, NULL or equivalent.
 */
Connection::Connection()
{
  this->ZeroInitialize();
}

/**
 *  \brief Build a Connection from a ConnectionStatus.
 *
 *  Copy all data from the ConnectionStatus object and zero-initialize
 *  remaining members.
 *
 *  \param[in] cs Object to copy data from.
 */
Connection::Connection(const ConnectionStatus& cs) : ConnectionStatus(cs)
{
  this->ZeroInitialize();
}

/**
 *  \brief Connection copy constructor.
 *
 *  Copy all data from the given object to the current instance.
 *
 *  \param[in] c Object to copy data from.
 */
Connection::Connection(const Connection& c) : ConnectionStatus(c)
{
  this->InternalCopy(c);
}

/**
 *  Connection destructor.
 */
Connection::~Connection() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all data from the given object to the current instance.
 *
 *  \param[in] c Object to copy data from.
 */
Connection& Connection::operator=(const Connection& c)
{
  this->ConnectionStatus::operator=(c);
  this->InternalCopy(c);
  return (*this);
}

/**
 *  \brief Get the type of this event (Event::CONNECTION).
 *
 *  This method can help identify the type of an event at runtime.
 *
 *  \return Event::CONNECTION
 */
int Connection::GetType() const throw ()
{
  return (Event::CONNECTION);
}
