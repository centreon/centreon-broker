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
  this->query_ = "TRUNCATE TABLE `";
  this->query_ += this->table_;
  this->query_ += '`';
  this->MySQLQuery::Prepare();
  return ;
}
