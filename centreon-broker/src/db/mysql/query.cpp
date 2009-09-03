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

#include <errmsg.h>
#include "db/db_exception.h"
#include "db/mysql/query.h"
#include "logging.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief MySQLQuery constructor.
 *
 *  Initialize the new instance with the MySQL connection object from which it
 *  depends.
 *
 *  \param[in] myconn MySQL connection on which the query will be executed.
 */
MySQLQuery::MySQLQuery(MYSQL* myconn) : mysql(myconn), stmt(NULL) {}

/**
 *  \brief MySQLQuery copy constructor.
 *
 *  Build the MySQL query by copying data from the given query.
 *
 *  \param[in] myquery Object to copy data from.
 */
MySQLQuery::MySQLQuery(const MySQLQuery& myquery) : Query(myquery)
{
  // XXX : copy data
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given query object to the current instance.
 *
 *  \param[in] myquery Object to copy data from.
 *
 *  \return *this
 */
MySQLQuery& MySQLQuery::operator=(const MySQLQuery& myquery)
{
  this->Query::operator=(myquery);
  // XXX : copy data
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief MySQLQuery destructor.
 *
 *  Close the statement if the Prepare() method has been called.
 */
MySQLQuery::~MySQLQuery()
{
  if (this->stmt)
    mysql_stmt_close(this->stmt);
}

/**
 *  \brief Execute the query.
 *
 *  Execute the underlying query on the MySQL server.
 */
void MySQLQuery::Execute()
{
  if (this->stmt)
    {
#ifndef NDEBUG
      logging.LogDebug("Executing MySQL prepared statement ...");
#endif /* !NDEBUG */
      if (mysql_stmt_execute(this->stmt))
	{
	  int ec;

	  ec = mysql_stmt_errno(this->stmt);
	  if ((CR_SERVER_GONE_ERROR == ec)
              || (CR_SERVER_LOST == ec))
            throw (DBException(ec,
                               DBException::CONNECTION,
                               mysql_stmt_error(this->stmt)));
          else
            throw (DBException(ec,
                               DBException::QUERY_EXECUTION,
                               mysql_stmt_error(this->stmt)));
	}
    }
  else
    {
#ifndef NDEBUG
      logging.LogDebug("Executing MySQL standard query ...");
      logging.LogDebug(this->query.c_str());
#endif /* !NDEBUG */
      if (mysql_query(this->mysql, this->query.c_str()))
        {
	  int ec;

	  ec = mysql_errno(this->mysql);
          if ((CR_SERVER_GONE_ERROR == ec)
              || (CR_SERVER_LOST == ec))
            throw (DBException(ec,
                               DBException::CONNECTION,
                               mysql_error(this->mysql)));
          else
            throw (DBException(ec,
                               DBException::QUERY_EXECUTION,
                               mysql_error(this->mysql)));
        }
    }
  return ;
}

/**
 *  \brief Prepare the query.
 *
 *  Prepare the query on the MySQL server for later execution.
 */
void MySQLQuery::Prepare()
{
#ifndef NDEBUG
  logging.LogDebug("Preparing MySQL statement...");
  logging.LogDebug(this->query.c_str());
#endif /* !NDEBUG */
  this->stmt = mysql_stmt_init(this->mysql);
  if (!this->stmt)
    throw (DBException(mysql_errno(this->mysql),
                       DBException::QUERY_PREPARATION,
                       mysql_error(this->mysql)));
  if (mysql_stmt_prepare(this->stmt,
                         this->query.c_str(),
                         this->query.size()))
    throw (DBException(mysql_stmt_errno(this->stmt),
                       DBException::QUERY_PREPARATION,
                       mysql_stmt_error(this->stmt)));
  return ;
}
