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

#include <cstring>
#include "connection.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Copy all internal data from the Connection object to the current instance.
 */
void Connection::InternalCopy(const Connection& c)
{
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = c.strings_[i];
  memcpy(this->timets_, c.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Connection default constructor.
 */
Connection::Connection() throw ()
{
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  Connection copy constructor.
 */
Connection::Connection(const Connection& c) : ConnectionStatus(c)
{
  this->InternalCopy(c);
}

/**
 *  Build a Connection from a ConnectionStatus.
 */
Connection::Connection(const ConnectionStatus& cs) : ConnectionStatus(cs)
{
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  Connection destructor.
 */
Connection::~Connection() throw ()
{
}

/**
 *  Connection operator= overload.
 */
Connection& Connection::operator=(const Connection& c)
{
  this->ConnectionStatus::operator=(c);
  this->InternalCopy(c);
  return (*this);
}

/**
 *  Get the agent_name member.
 */
const std::string& Connection::GetAgentName() const throw ()
{
  return (this->strings_[AGENT_NAME]);
}

/**
 *  Get the agent_version member.
 */
const std::string& Connection::GetAgentVersion() const throw ()
{
  return (this->strings_[AGENT_VERSION]);
}

/**
 *  Get the connect_source member.
 */
const std::string& Connection::GetConnectSource() const throw ()
{
  return (this->strings_[CONNECT_SOURCE]);
}

/**
 *  Get the connect_time member.
 */
time_t Connection::GetConnectTime() const throw ()
{
  return (this->timets_[CONNECT_TIME]);
}

/**
 *  Get the connect_type member.
 */
const std::string& Connection::GetConnectType() const throw ()
{
  return (this->strings_[CONNECT_TYPE]);
}

/**
 *  Get the data_start_time member.
 */
time_t Connection::GetDataStartTime() const throw ()
{
  return (this->timets_[DATA_START_TIME]);
}

/**
 *  Get the type of the event.
 */
int Connection::GetType() const throw ()
{
  return (Event::CONNECTION);
}

/**
 *  Set the agent_name member.
 */
void Connection::SetAgentName(const std::string& an)
{
  this->strings_[AGENT_NAME] = an;
  return ;
}

/**
 *  Set the agent_version member.
 */
void Connection::SetAgentVersion(const std::string& av)
{
  this->strings_[AGENT_VERSION] = av;
  return ;
}

/**
 *  Set the connect_source member.
 */
void Connection::SetConnectSource(const std::string& cs)
{
  this->strings_[CONNECT_SOURCE] = cs;
  return ;
}

/**
 *  Set the connect_time member.
 */
void Connection::SetConnectTime(time_t ct) throw ()
{
  this->timets_[CONNECT_TIME] = ct;
  return ;
}

/**
 *  Set the connect_type member.
 */
void Connection::SetConnectType(const std::string& ct)
{
  this->strings_[CONNECT_TYPE] = ct;
  return ;
}

/**
 *  Set the data_start_time member.
 */
void Connection::SetDataStartTime(time_t dst) throw ()
{
  this->timets_[DATA_START_TIME] = dst;
  return ;
}
