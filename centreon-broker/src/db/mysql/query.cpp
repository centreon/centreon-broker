/*
** query.cpp for CentreonBroker in ./src/db/mysql
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/09/09 Matthieu Kermagoret
** Last update 06/12/09 Matthieu Kermagoret
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
void MySQLQuery::Execute() throw (DBException)
{
  logging.AddDebug("Executing MySQL prepared statement...");
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
  logging.AddDebug("Preparing MySQL statement...");
  logging.Indent();
  logging.AddDebug(this->query_.c_str());
  logging.Deindent();
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
