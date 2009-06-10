/*
** truncate.cpp for CentreonBroker in ./src/db/mysql
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/05/09 Matthieu Kermagoret
** Last update 06/10/09 Matthieu Kermagoret
*/

#include "db/mysql/truncate.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  MySQLTruncate copy constructor.
 */
MySQLTruncate::MySQLTruncate(const MySQLTruncate& mytruncate) throw ()
  : Query(), Truncate(), MySQLQuery(mytruncate.myconn_)
{
  (void)mytruncate;
}

/**
 *  MySQLTruncate operator= overload.
 */
MySQLTruncate& MySQLTruncate::operator=(const MySQLTruncate& mytruncate)
  throw ()
{
  (void)mytruncate;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  MySQLTruncate default constructor.
 */
MySQLTruncate::MySQLTruncate(MYSQL* myconn) : MySQLQuery(myconn)
{
}

/**
 *  MySQLTruncate destructor.
 */
MySQLTruncate::~MySQLTruncate()
{
}

/**
 *  Execute the query.
 */
void MySQLTruncate::Execute()
{
  this->MySQLQuery::Execute();
  return ;
}

/**
 *  Prepare the query.
 */
void MySQLTruncate::Prepare() throw (DBException)
{
  this->query_ = "TRUNCATE TABLE ";
  this->query_ += this->table_;
  this->MySQLQuery::Prepare();
  return ;
}
