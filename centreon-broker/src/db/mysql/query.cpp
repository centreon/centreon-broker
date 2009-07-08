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

#include "db/mysql/query.h"
#include "logging.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  MySQLQuery copy constructor. Because queries are not copyable, this is
 *  declared private.
 */
MySQLQuery::MySQLQuery(const MySQLQuery& myquery) throw () : Query()
{
  (void)myquery;
}

/**
 *  MySQLQuery operator= overload. Because queries are not copyable, this is
 *  declared private.
 */
MySQLQuery& MySQLQuery::operator=(const MySQLQuery& myquery) throw ()
{
  (void)myquery;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  MySQLQuery constructor. It needs the MySQL connection object from which it
 *  depends.
 */
MySQLQuery::MySQLQuery(MYSQL* myconn) throw () : myconn_(myconn), mystmt_(NULL)
{
}

/**
 *  MySQLQuery destructor.
 */
MySQLQuery::~MySQLQuery()
{
  if (this->mystmt_)
    mysql_stmt_close(this->mystmt_);
}

/**
 *  Execute the query.
 */
void MySQLQuery::Execute()
{
#ifndef NDEBUG
  logging.LogDebug("Executing MySQL prepared statement...");
#endif /* !NDEBUG */
  assert(this->mystmt_);
  if (mysql_stmt_execute(this->mystmt_))
    throw (DBException(mysql_stmt_errno(this->mystmt_),
                       DBException::QUERY_EXECUTION,
                       mysql_stmt_error(this->mystmt_)));
  return ;
}

/**
 *  Prepare the query.
 */
void MySQLQuery::Prepare()
{
#ifndef NDEBUG
  logging.LogDebug("Preparing MySQL statement...", true);
  logging.LogDebug(this->query_.c_str());
  logging.Deindent();
#endif /* !NDEBUG */
  this->mystmt_ = mysql_stmt_init(this->myconn_);
  if (!this->mystmt_)
    throw (DBException(mysql_stmt_errno(this->mystmt_),
                       DBException::QUERY_PREPARATION,
                       mysql_stmt_error(this->mystmt_)));
  if (mysql_stmt_prepare(this->mystmt_,
                         this->query_.c_str(),
                         this->query_.size()))
    throw (DBException(mysql_stmt_errno(this->mystmt_),
                       DBException::QUERY_PREPARATION,
                       mysql_stmt_error(this->mystmt_)));
  return ;
}
