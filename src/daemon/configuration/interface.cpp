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

#include "configuration/interface.h"

using namespace Configuration;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy data members.
 *
 *  Copy all data members from the given object to the current instance. This
 *  method is used by the copy constructor and the assignment operator.
 *  \par Safety Basic exception safety.
 *
 *  \param[in] interface Object to copy data from.
 */
void Interface::InternalCopy(const Interface& interface)
{
  this->db        = interface.db;
  this->filename  = interface.filename;
  this->host      = interface.host;
  this->interface = interface.interface;
  this->name      = interface.name;
  this->password  = interface.password;
  this->port      = interface.port;
  this->protocol  = interface.protocol;
  this->socket    = interface.socket;
  this->tls       = interface.tls;
  this->type      = interface.type;
  this->user      = interface.user;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Interface default constructor.
 */
Interface::Interface()
{
  this->db       = "cb";
  this->host     = "localhost";
  this->port     = 5668;
  this->protocol = UNKNOWN_PROTO;
  this->tls      = false;
  this->type     = UNKNOWN_TYPE;
  this->user     = "root";
}

/**
 *  Interface copy constructor.
 *
 *  \param[in] interface Object to copy data from.
 */
Interface::Interface(const Interface& interface)
{
  this->InternalCopy(interface);
}

/**
 *  Interface destructor.
 */
Interface::~Interface() {}

/**
 *  Assignment operator overload.
 *  \par Safety Basic exception safety.
 *
 *  \param[in] interface Object to copy data from.
 *
 *  \return *this
 */
Interface& Interface::operator=(const Interface& interface)
{
  this->InternalCopy(interface);
  return (*this);
}
