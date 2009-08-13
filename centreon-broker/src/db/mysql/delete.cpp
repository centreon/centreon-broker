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

#include "db/mysql/delete.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Generate the first part of the query string.
 *
 *  Expand the query object to a literal query string comprehensible by MySQL.
 */
void MySQLDelete::GenerateQuery()
{
  this->query = "DELETE FROM `";
  this->query.append(this->table);
  this->query.append("`");
  return ;
}

/**
 *  \brief Returns the number of argument this query accepts.
 *
 *  This overriden method is used by MySQLHaveArgs to allocate its argument
 *  buffer.
 *
 *  \return Number of arguments to be set in this query.
 */
unsigned int MySQLDelete::GetArgCount() throw ()
{
  return (this->placeholders);
}

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief MySQLDelete copy constructor.
 *
 *  Build the new instance by copying data from the given object.
 *
 *  \param[in] mydelete Object to copy data from.
 */
MySQLDelete::MySQLDelete(const MySQLDelete& mydelete)
  : HaveArgs(mydelete),
    HavePredicate(mydelete),
    Query(mydelete),
    Delete(mydelete),
    MySQLHaveArgs(mydelete), MySQLHavePredicate(mydelete) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] mydelete Object to copy data from.
 *
 *  \return *this
 */
MySQLDelete& MySQLDelete::operator=(const MySQLDelete& mydelete)
{
  this->Delete::operator=(mydelete);
  this->MySQLHaveArgs::operator=(mydelete);
  this->MySQLHavePredicate::operator=(mydelete);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief MySQLDelete constructor. Needs the MySQL connection object.
 *
 *  Build a new MySQL DELETE query.
 *
 *  \param[in] myconn The MySQL connection object on which the query will be
 *                    executed.
 */
MySQLDelete::MySQLDelete(MYSQL* myconn) : MySQLHaveArgs(myconn) {}

/**
 *  \brief MySQLDelete destructor.
 *
 *  Release all acquired ressources.
 */
MySQLDelete::~MySQLDelete() {}

/**
 *  \brief Execute the DELETE query.
 *
 *  Execute the DELETE query on the MySQL server as it has been configured.
 */
void MySQLDelete::Execute()
{
  // If the query has not been prepared, generate the query string
  if (!this->stmt)
    {
      // Generate the first part of the query.
      this->GenerateQuery();

      // Generate the predicate string (if any).
      this->MySQLHavePredicate::ProcessPredicate(this->query);
    }

  // Execute the query (prepared or not).
  this->MySQLHaveArgs::Execute();

  return ;
}

/**
 *  \brief Prepare the DELETE query.
 *
 *  Prepare the DELETE query on the MySQL server.
 */
void MySQLDelete::Prepare()
{
  // Generate the query string.
  this->GenerateQuery();

  // Append the predicate (if any).
  this->MySQLHavePredicate::PreparePredicate(this->query);

  // Prepare the query against the DB server.
  this->MySQLHaveArgs::Prepare();

  return ;
}
