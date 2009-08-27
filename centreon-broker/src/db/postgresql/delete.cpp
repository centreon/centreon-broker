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

#include "db/postgresql/delete.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Generate the first part of the query string.
 *
 *  Expand the query object to a literal query string comprehensible by
 *  PostgreSQL.
 */
void PgSQLDelete::GenerateQuery()
{
  this->query = "DELETE FROM \"";
  this->query.append(this->table);
  this->query.append("\"");
  return ;
}

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief PgSQLDelete copy constructor.
 *
 *  Build the new instance by copying data from the given object.
 *
 *  \param[in] pgdelete Object to copy data from.
 */
PgSQLDelete::PgSQLDelete(const PgSQLDelete& pgdelete)
  : HaveArgs(pgdelete),
    HavePredicate(pgdelete),
    Query(pgdelete),
    Delete(pgdelete),
    PgSQLHaveArgs(pgdelete),
    PgSQLHavePredicate(pgdelete) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] pgdelete Object to copy data from.
 *
 *  \return *this
 */
PgSQLDelete& PgSQLDelete::operator=(const PgSQLDelete& pgdelete)
{
  this->Delete::operator=(pgdelete);
  this->PgSQLHaveArgs::operator=(pgdelete);
  this->PgSQLHavePredicate::operator=(pgdelete);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief PgSQLDelete constructor. Needs the PostgreSQL connection object.
 *
 *  Build a new PostgreSQL DELETE query.
 *
 *  \param[in] pgconn The PostgreSQL connection object on which the query will
 *                    be executed.
 */
PgSQLDelete::PgSQLDelete(PGconn* pgconn) : PgSQLHaveArgs(pgconn) {}

/**
 *  \brief PgSQLDelete destructor.
 *
 *  Release all acquired ressources.
 */
PgSQLDelete::~PgSQLDelete() {}

/**
 *  \brief Execute the DELETE query.
 *
 *  Execute the DELETE query on the PostgreSQL server as it has been configured.
 */
void PgSQLDelete::Execute()
{
  // If the query has not been prepared, generate the query string.
  if (!this->stmt)
    {
      // Generate the first part of the query.
      this->GenerateQueryBeginning();

      // Generate the predicate string (if any).
      this->PgSQLHavePredicate::ProcessPredicate(this->query);
    }

  // Execute the query (prepared or not).
  this->PgSQLHaveArgs::Execute();

  return ;
}

/**
 *  \brief Prepare the DELETE query.
 *
 *  Prepare the DELETE query on the PostgreSQL server.
 */
void PgSQLDelete::Prepare()
{
  // Generate the query string.
  this->GenerateQueryBeginning();

  // Append the predicate (if any).
  this->PgSQLHavePredicate::PreparePredicate(this->query);

  // Prepare the query against the DB server.
  this->PgSQLHaveArgs::Prepare();

  return ;
}
