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

#include <cassert>
#include "db/connection.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Connection copy constructor.
 */
Connection::Connection(const Connection& conn) throw () : dbms_(UNKNOWN)
{
  (void)conn;
  assert(false);
}

/**
 *  Connection operator= overload.
 */
Connection& Connection::operator=(const Connection& conn) throw ()
{
  (void)conn;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Connection default constructor.
 */
Connection::Connection(Connection::DBMS dbms) throw () : dbms_(dbms)
{
}

/**
 *  Connection destructor.
 */
Connection::~Connection()
{
}

/**
 *  Returns the type of the current DBMS.
 */
Connection::DBMS Connection::GetDbms() const throw ()
{
  return (this->dbms_);
}
