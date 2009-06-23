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
#include "db/db_exception.h"
#include "db/postgresql/connection.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  PgSQLConnection copy constructor.
 */
PgSQLConnection::PgSQLConnection(const PgSQLConnection& pgconn)
  : Connection(Connection::POSTGRESQL)
{
  (void)pgconn;
  assert(false);
  return ;
}

/**
 *  PgSQL operator= overload.
 */
PgSQLConnection& PgSQLConnection::operator=(const PgSQLConnection& pgconn)
{
  (void)pgconn;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  PgSQLConnection default constructor.
 */
PgSQLConnection::PgSQLConnection() : Connection(Connection::POSTGRESQL)
{
}

/**
 *  PgSQLConnection destructor.
 */
PgSQLConnection::~PgSQLConnection()
{

}

/**
 *  Toggle the auto-commit mode of the connection.
 */
void PgSQLConnection::AutoCommit(bool activate)
{
}

/**
 *  Commit data of the current transaction.
 */
void PgSQLConnection::Commit()
{
}

/**
 *  Connect to the specified PostgreSQL server.
 */
void PgSQLConnection::Connect(const std::string& host,
			      const std::string& user,
			      const std::string& password,
			      const std::string& db)
{
  std::string conninfo;

  this->pgconn_ = PQconnectdb(conninfo.c_str());
  if (!this->pgconn_ || PQstatus(this->pgconn_) != CONNECTION_OK)
    {
      if (this->pgconn_)
	this->Disconnect();
      throw (DBException(0,
			 DB::DBException::INITIALIZATION,
			 "Connection to PostgreSQL server failed"));
    }
  return ;
}

/**
 *  Disconnect from the PostgreSQL server.
 */
void PgSQLConnection::Disconnect()
{
  assert(this->pgconn_);
  PQfinish(this->pgconn_);
  this->pgconn_ = NULL;
  return ;
}
