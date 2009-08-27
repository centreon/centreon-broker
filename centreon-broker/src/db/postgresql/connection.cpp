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
#include <libpq-fe.h>
#include "db/db_exception.h"
#include "db/postgresql/connection.h"
#include "logging.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief PgSQLConnection copy constructor.
 *
 *  Build a new object by copying data from the given object.
 *
 *  \param[in] pgconn Object to copy data from.
 */
PgSQLConnection::PgSQLConnection(const PgSQLConnection& pgconn)
  : Connection(pgconn)
{
  // XXX
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data of the given object to the current instance.
 *
 *  \param[in] pgconn Object to copy data from.
 *
 *  \return *this
 */
PgSQLConnection& PgSQLConnection::operator=(const PgSQLConnection& pgconn)
{
  // XXX
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief PgSQLConnection default constructor.
 *
 *  Initialize object to its default state.
 */
PgSQLConnection::PgSQLConnection() throw ()
  : Connection(Connection::POSTGRESQL), pgconn_(NULL) {}

/**
 *  \brief PgSQLConnection destructor.
 *
 *  Release acquired ressources.
 */
PgSQLConnection::~PgSQLConnection() throw ()
{
  this->Disconnect();
}

/**
 *  \brief Toggle the auto-commit mode of the connection.
 *
 *  Set whether or not the database engine should commit queries automatically.
 *
 *  \param[in] activate true if data should be committed automatically, false
 *                      otherwise.
 */
void PgSQLConnection::AutoCommit(bool activate)
{
  PGresult* res;

  (void)activate;
  assert(this->pgconn_);
  assert(!activate);
#ifndef NDEBUG
  if (activate)
    CentreonBroker::logging.LogDebug("Activating PostgreSQL " \
				     "auto-commit mode...");
  else
    CentreonBroker::logging.LogDebug("Deactivating PostgreSQL " \
				     "auto-commit mode...");
#endif /* !NDEBUG */
  // XXX : for now we only support autocommit activation, not deactivation
  res = PQexec(this->pgconn_, "BEGIN;");
  if (!res)
    {
      throw (DBException(0,
                         DBException::INITIALIZATION,
                         "Could not allocate memory."));
    }
  else if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      DBException dbe(0,
		      DBException::INITIALIZATION,
                      PQresultErrorMessage(res));
      PQclear(res);
      throw (dbe);
    }
  PQclear(res);
  return ;
}

/**
 *  Commit data of the current transaction.
 */
void PgSQLConnection::Commit()
{
  PGresult* res;

  assert(this->pgconn_);
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Committing data to DB...");
#endif /* !NDEBUG */
  res = PQexec(this->pgconn_, "COMMIT; BEGIN;");
  if (!res)
    {
      throw (DBException(0,
			 DBException::COMMIT,
			 "Could not allocate memory."));
    }
  else if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      DBException dbe(0,
		      DBException::COMMIT,
		      PQresultErrorMessage(res));
      PQclear(res);
      throw (dbe);
    }
  PQclear(res);
  return ;
}

/**
 *  Connect to the PostgreSQL server.
 *
 *  \param[in] host     Host to connect to.
 *  \param[in] user     User name to use for authentication.
 *  \param[in] password Password to use for authentication.
 *  \param[in] db       Database to use.
 */
void PgSQLConnection::Connect(const std::string& host,
			      const std::string& user,
			      const std::string& password,
			      const std::string& db)
{
  std::string conninfo;

#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Connecting to PostgreSQL server...", true);
  CentreonBroker::logging.LogDebug((std::string("Host: ") + host).c_str());
  CentreonBroker::logging.LogDebug((std::string("User: ") + user).c_str());
  CentreonBroker::logging.LogDebug((std::string("DB: ") + db).c_str());
  CentreonBroker::logging.Deindent();
#endif /* !NDEBUG */
  conninfo = "host=";
  conninfo += host;
  conninfo += " user=";
  conninfo += user;
  conninfo += " password=";
  conninfo += password;
  conninfo += " dbname=";
  conninfo += db;
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
void PgSQLConnection::Disconnect() throw ()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Disconnecting from PostgreSQL server...");
#endif /* !NDEBUG */
  if (this->pgconn_)
    {
      PQfinish(this->pgconn_);
      this->pgconn_ = NULL;
    }
  return ;
}

/**
 *  Get a DELETE query.
 *
 *  \return A new Delete query object.
 */
Delete* PgSQLConnection::GetDelete()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating PostgreSQL DELETE query...");
#endif /* !NDEBUG */
  return (new PgSQLDelete(this->pgconn_));
}

/**
 *  Get an INSERT query.
 *
 *  \return A new Insert query object.
 */
Insert* PgSQLConnection::GetInsert()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating PostgreSQL INSERT query...");
#endif /* !NDEBUG */
  return (new PgSQLInsert(this->pgconn_));
}

/**
 *  Get a SELECT query.
 *
 *  \return A new Select query object.
 */
Select* PgSQLConnection::GetSelect()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating PostgreSQL SELECT query...");
#endif /* !NDEBUG */
  return (new PgSQLSelect(this->pgconn_));
}

/**
 *  Get a TRUNCATE query.
 *
 *  \return A new Truncate query object.
 */
Truncate* PgSQLConnection::GetTruncate()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating PostgreSQL TRUNCATE query...");
#endif /* !NDEBUG */
  return (new PgSQLTruncate(this->pgconn_));
}

/**
 *  Get an UPDATE query.
 *
 *  \return A new Update query object.
 */
Update* PgSQLConnection::GetUpdate()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating PostgreSQL UPDATE query...");
#endif /* !NDEBUG */
  return (new PgSQLUpdate(this->pgconn_));
}
