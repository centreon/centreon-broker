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
#include <mysql.h>
#include "db/db_exception.h"
#include "db/mysql/connection.h"
#include "logging.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief MySQLConnection copy constructor.
 *
 *  Build a new object by copying data from the given object.
 *
 *  \param[in] myconn Object to copy data from.
 */
MySQLConnection::MySQLConnection(const MySQLConnection& myconn)
  : Connection(myconn)
{
  // XXX
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data of the given object to the current instance.
 *
 *  \param[in] myconn Object to copy data from.
 *
 *  \return *this
 */
MySQLConnection& MySQLConnection::operator=(const MySQLConnection& myconn)
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
 *  \brief MySQLConnection default constructor.
 *
 *  Initialize object to its default state.
 */
MySQLConnection::MySQLConnection() throw (DBException)
  : Connection(Connection::MYSQL), myconn_(NULL)
{
  // Ensure that mysql_thread_init has been properly called even if the object
  // wasn't used.
  if (mysql_thread_init())
    throw (DBException(0,
                       DBException::INITIALIZATION,
                       "MySQL library initialization failed for this thread"));
}

/**
 *  \brief MySQLConnection destructor.
 *
 *  Release acquired ressources.
 */
MySQLConnection::~MySQLConnection() throw ()
{
  this->Disconnect();
  mysql_thread_end();
}

/**
 *  \brief Toggle the auto-commit mode of the database.
 *
 *  Set whether or not the database engine should commit queries automatically.
 *
 *  \param[in] activate true if data should be commited automatically, false
 *                      otherwise.
 */
void MySQLConnection::AutoCommit(bool activate) throw (DBException)
{
#ifndef NDEBUG
  if (activate)
    CentreonBroker::logging.LogDebug("Activating MySQL auto-commit mode...");
  else
    CentreonBroker::logging.LogDebug("Deactivating MySQL auto-commit mode...");
#endif /* !NDEBUG */
  assert(this->myconn_);
  if (mysql_autocommit(this->myconn_, activate))
    throw (DBException(mysql_errno(this->myconn_),
                       DBException::INITIALIZATION,
                       mysql_error(this->myconn_)));
  return ;
}

/**
 *  Commit current transaction to the DB.
 */
void MySQLConnection::Commit() throw (DBException)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Committing data to DB...");
#endif /* !NDEBUG */
  assert(this->myconn_);
  if (mysql_commit(this->myconn_))
    throw (DBException(mysql_errno(this->myconn_),
                       DBException::COMMIT,
                       mysql_error(this->myconn_)));
  return ;
}

/**
 *  Connect to the MySQL server.
 *
 *  \param[in] host     Host to connect to.
 *  \param[in] user     User name to use for authentication.
 *  \param[in] password Password to use for authentication.
 *  \param[in] db       Database to use.
 */
void MySQLConnection::Connect(const std::string& host,
			      const std::string& user,
			      const std::string& password,
			      const std::string& db)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Connecting to MySQL server...");
  CentreonBroker::logging.LogDebug((std::string("Host: ") + host).c_str());
  CentreonBroker::logging.LogDebug((std::string("User: ") + user).c_str());
  CentreonBroker::logging.LogDebug((std::string("DB: ") + db).c_str());
#endif /* !NDEBUG */
  // Initialize the MYSQL structure.
  this->myconn_ = mysql_init(NULL);
  if (!this->myconn_)
    throw (DBException(mysql_errno(this->myconn_),
                       DBException::INITIALIZATION,
                       mysql_error(this->myconn_)));

  // Connect to the DB server.
  if (!mysql_real_connect(this->myconn_,
			  host.c_str(),
			  user.c_str(),
			  password.c_str(),
			  db.c_str(),
			  0,
			  NULL,
			  CLIENT_FOUND_ROWS))
    throw (DBException(mysql_errno(this->myconn_),
                       DBException::INITIALIZATION,
                       mysql_error(this->myconn_)));
  return ;
}

/**
 *  Disconnect of the MySQL server.
 */
void MySQLConnection::Disconnect() throw ()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Disconnecting from MySQL server...");
#endif /* !NDEBUG */
  if (this->myconn_)
    {
      mysql_close(this->myconn_);
      this->myconn_ = NULL;
    }
  return ;
}

/**
 *  Get a DELETE query.
 *
 *  \return A new Delete query object.
 */
Delete* MySQLConnection::GetDelete()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating MySQL DELETE query...");
#endif /* !NDEBUG */
  return (new MySQLDelete(this->myconn_));
}

/**
 *  Get an INSERT query.
 *
 *  \return A new Insert query object.
 */
Insert* MySQLConnection::GetInsert()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating MySQL INSERT query...");
#endif /* !NDEBUG */
  return (new MySQLInsert(this->myconn_));
}

/**
 *  Get a SELECT query.
 *
 *  \return A new Select query object.
 */
Select* MySQLConnection::GetSelect()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating MySQL SELECT query...");
#endif /* !NDEBUG */
  return (new MySQLSelect(this->myconn_));
}

/**
 *  Get a TRUNCATE query.
 *
 *  \return A new Truncate query object.
 */
Truncate* MySQLConnection::GetTruncate()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating MySQL TRUNCATE query...");
#endif /* !NDEBUG */
  return (new MySQLTruncate(this->myconn_));
}

/**
 *  Get an UPDATE query.
 *
 *  \return A new Update query object.
 */
Update* MySQLConnection::GetUpdate()
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Creating MySQL UPDATE query...");
#endif /* !NDEBUG */
  return (new MySQLUpdate(this->myconn_));
}
