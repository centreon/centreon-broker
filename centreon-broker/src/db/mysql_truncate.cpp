/*
** mysql_truncate.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/05/09 Matthieu Kermagoret
** Last update 06/05/09 Matthieu Kermagoret
*/

#include <cassert>
#include <mysql.h>
#include "db/mysql_truncate.h"
#include "exception.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  MySQLTruncate default constructor.
 */
MySQLTruncate::MySQLTruncate(MYSQL* myconn) : myconn_(myconn)
{
}

/**
 *  Copy all internal data of the given object to the current instance.
 */
void MySQLTruncate::InternalCopy(const MySQLTruncate& truncate)
{
  this->myconn_ = truncate.myconn_;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  MySQLTruncate copy constructor.
 */
MySQLTruncate::MySQLTruncate(const MySQLTruncate& truncate)
  : TruncateQuery(truncate)
{
  this->InternalCopy(truncate);
}

/**
 *  MySQLTruncate destructor.
 */
MySQLTruncate::~MySQLTruncate()
{
}

/**
 *  MySQLTruncate operator= overload.
 */
MySQLTruncate& MySQLTruncate::operator=(const MySQLTruncate& truncate)
{
  this->TruncateQuery::operator=(truncate);
  this->InternalCopy(truncate);
  return (*this);
}

/**
 *  Execute the TRUNCATE query.
 */
void MySQLTruncate::Execute()
{
  std::string query;

  assert(this->myconn_);
  assert(!this->table_.empty());
  query = "TRUNCATE TABLE ";
  query += this->table_;
  query += ";";
  if (mysql_query(this->myconn_, query.c_str()))
    throw (Exception(mysql_errno(this->myconn_),
                     mysql_error(this->myconn_)));
  return ;
}
