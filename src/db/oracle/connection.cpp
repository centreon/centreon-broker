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
#include <ocilib.h>
#include "db/db_exception.h"
#include "db/oracle/connection.h"
#include "logging.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief OracleConnection copy constructor.
 *
 *  Build a new object by copying data from the given object.
 *
 *  \param[in] oconn Object to copy data from.
 */
OracleConnection::OracleConnection(const OracleConnection& oconn)
  : Connection(oconn)
{
  // XXX
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data of the given object to the current instance.
 *
 *  \param[in] oconn Object to copy data from.
 *
 *  \return *this
 */
OracleConnection& OracleConnection::operator=(const OracleConnection& oconn)
{
  // XXX
  return (*this);
}

/**************************************
*                                     *
*          Public Methods             *
*                                     *
**************************************/

/**
 *  \brief OracleConnection default constructor.
 *
 *  Initialize object to its default state.
 */
OracleConnection::OracleConnection() throw (DBException)
  : Connection(Connection::ORACLE), oconn_(NULL) {}

/**
 *  \brief OracleConnection destructor.
 *
 *  Release acquired ressources.
 */
OracleConnection::~OracleConnection() throw ()
{
  this->Disconnect();
}

/**
 *  \brief Toggle the auto-commit mode of the database.
 *
 *  Set whether or not the database engine should commit queries automatically.
 *
 *  \param[in] activate true if data should be commited automatically, false
 *                      otherwise.
 */
void OracleConnection::AutoCommit(bool activate) throw (DBException)
{
#ifndef NDEBUG
  if (activate)
    CentreonBroker::logging.LogDebug("Activating Oracle auto-commit mode...");
  else
    CentreonBroker::logging.LogDebug("Deactivating Oracle auto-commit mode...");
#endif /* !NDEBUG */
  assert(this->oconn_);
  if (!OCI_SetAutoCommit(this->oconn_, activate))
    {
      OCI_Error* err;

      err = OCI_GetLastError();
      throw (DBException(OCI_ErrorGetOCICode(err),
                         DBException::INITIALIZATION,
                         OCI_ErrorGetString(err)));
    }
  return ;
}

/**
 *  Commit current transaction to the DB.
 */
void OracleConnection::Commit() throw (DBException)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Committing data to DB...");
#endif /* !NDEBUG */
  assert(this->oconn_);
  if (!OCI_Commit(this->oconn_))
    {
      OCI_Error* err;

      err = OCI_GetLastError();
      throw (DBException(OCI_ErrorGetOCICode(err),
                         DBException::COMMIT,
                         OCI_ErrorGetString(err)));
    }
  return ;
}

/**
 *  Connect to the Oracle server.
 *
 *  \param[in] host     Host to connect to.
 *  \param[in] user     User name to use for authentication.
 *  \param[in] password Password to use for authentication.
 *  \param[in] db       Database to use.
 */
void OracleConnection::Connect(const std::string& host,
                               const std::string& user,
                               const std::string& password,
                               const std::string& db)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Connecting to Oracle server...");
  CentreonBroker::logging.LogDebug((std::string("Host: ") + host).c_str());
  CentreonBroker::logging.LogDebug((std::string("User: ") + user).c_str());
  CentreonBroker::logging.LogDebug((std::string("DB: ") + db).c_str());
#endif /* !NDEBUG */
  // Generate the connection string
  std::string connect_str;

  connect_str = host + "/" + db;

  // Connect
  this->oconn_ = OCI_CreateConnection(connect_str.c_str(),
                                      user.c_str(),
                                      password.c_str(),
                                      OCI_SESSION_DEFAULT);
  if (!this->oconn_)
    {
      OCI_Error* err;

      err = OCI_GetLastError();
      throw (DBException(OCI_ErrorGetOCICode(err),
                         DBException::CONNECTION,
                         OCI_ErrorGetString(err)));
    }
  return ;
}

/**
 *  Disconnect of the Oracle server.
 */
void OracleConnection::Disconnect() throw ()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Disconnecting from Oracle server...");
#endif /* !NDEBUG */
  if (this->oconn_)
    {
      OCI_ConnectionFree(this->oconn_);
      this->oconn_ = NULL;
    }
  return ;
}

/**
 *  Get a DELETE query.
 *
 *  \return A new Delete query object.
 */
Delete* OracleConnection::GetDelete()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating Oracle DELETE query...");
#endif /* !NDEBUG */
  return (new OracleDelete(this->oconn_));
}

/**
 *  Get an INSERT query.
 *
 *  \return A new Insert query object.
 */
Insert* OracleConnection::GetInsert()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating Oracle INSERT query...");
#endif /* !NDEBUG */
  return (new OracleInsert(this->oconn_));
}

/**
 *  Get a SELECT query.
 *
 *  \return A new Select query object.
 */
Select* OracleConnection::GetSelect()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating Oracle SELECT query...");
#endif /* !NDEBUG */
  return (new OracleSelect(this->oconn_));
}

/**
 *  Get a TRUNCATE query.
 *
 *  \return A new Truncate query object.
 */
Truncate* OracleConnection::GetTruncate()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating Oracle TRUNCATE query...");
#endif /* !NDEBUG */
  return (new OracleTruncate(this->oconn_));
}

/**
 *  Get an UPDATE query.
 *
 *  \return A new Update query object.
 */
Update* OracleConnection::GetUpdate()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating Oracle UPDATE query...");
#endif /* !NDEBUG */
  return (new OracleUpdate(this->oconn_));
}
