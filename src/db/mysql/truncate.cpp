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
 *  \brief MySQLTruncate copy constructor.
 *
 *  Construct the current instance by copying data from the given object.
 *  At least it should.
 *
 *  \param[in] mytruncate Object to copy data from.
 */
MySQLTruncate::MySQLTruncate(const MySQLTruncate& mytruncate)
  : Query(mytruncate), Truncate(mytruncate), MySQLQuery(mytruncate) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance. At least it
 *  should.
 *
 *  \param[in] mytruncate Object to copy data from.
 *
 *  \return *this
 */
MySQLTruncate& MySQLTruncate::operator=(const MySQLTruncate& mytruncate)
{
  this->Truncate::operator=(mytruncate);
  this->MySQLQuery::operator=(mytruncate);
  return (*this);
}

/**
 *  \brief Build the query string.
 *
 *  Build the literal TRUNCATE query.
 */
void MySQLTruncate::BuildQuery()
{
  this->query = "TRUNCATE TABLE `";
  this->query.append(this->table);
  this->query.append("`");
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief MySQLTruncate default constructor.
 *
 *  Initialize the MySQL TRUNCATE query.
 *
 *  \param[in] myconn MySQL connection object.
 */
MySQLTruncate::MySQLTruncate(MYSQL* myconn) : MySQLQuery(myconn) {}

/**
 *  \brief MySQLTruncate destructor.
 *
 *  Release all acquired ressources.
 */
MySQLTruncate::~MySQLTruncate() {}

/**
 *  \brief Execute the query.
 *
 *  Execute the TRUNCATE query on the specified table.
 */
void MySQLTruncate::Execute()
{
  // Build the query string if necessary
  if (!this->stmt)
    this->BuildQuery();

  // Really execute the query
  this->MySQLQuery::Execute();

  return ;
}

/**
 *  \brief Prepare the query.
 *
 *  Prepare the TRUNCATE query on the MySQL server.
 */
void MySQLTruncate::Prepare()
{
  // Build the query string
  this->BuildQuery();

  // Prepare the statement
  this->MySQLQuery::Prepare();
  return ;
}
