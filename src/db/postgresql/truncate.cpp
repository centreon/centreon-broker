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

#include "db/postgresql/truncate.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  PgSQLTruncate copy constructor.
 */
PgSQLTruncate::PgSQLTruncate(const PgSQLTruncate& pgtruncate) throw ()
  : Query(), Truncate(), PgSQLQuery(pgtruncate.pgconn_)
{
  (void)pgtruncate;
}

/**
 *  PgSQLTruncate operator= overload.
 */
PgSQLTruncate& PgSQLTruncate::operator=(const PgSQLTruncate& pgtruncate)
  throw ()
{
  (void)pgtruncate;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  PgSQLTruncate default constructor.
 */
PgSQLTruncate::PgSQLTruncate(PGconn* pgconn) : PgSQLQuery(pgconn)
{
}

/**
 *  PgSQLTruncate destructor.
 */
PgSQLTruncate::~PgSQLTruncate()
{
}

/**
 *  Execute the query.
 */
void PgSQLTruncate::Execute()
{
  this->PgSQLQuery::Execute();
  return ;
}

/**
 *  Prepare the query.
 */
void PgSQLTruncate::Prepare() throw (DBException)
{
  this->query_ = "TRUNCATE TABLE `";
  this->query_ += this->table_;
  this->query_ += '`';
  this->PgSQLQuery::Prepare();
  return ;
}
