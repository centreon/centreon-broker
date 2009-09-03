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
 *  \brief PgSQLTruncate copy constructor.
 *
 *  Construct the current instance by copying data from the given object.
 *
 *  \param[in] pgtruncate Object to copy data from.
 */
PgSQLTruncate::PgSQLTruncate(const PgSQLTruncate& pgtruncate)
  : HaveTable(pgtruncate),
    Query(pgtruncate),
    Truncate(pgtruncate),
    PgSQLQuery(NULL) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] pgtruncate Object to copy data from.
 *
 *  \return *this
 */
PgSQLTruncate& PgSQLTruncate::operator=(const PgSQLTruncate& pgtruncate)
{
  this->Truncate::operator=(pgtruncate);
  return (*this);
}

/**
 *  \brief Build the query string.
 *
 *  Build the literal TRUNCATE query.
 */
void PgSQLTruncate::BuildQuery()
{
  this->query = "TRUNCATE TABLE \"";
  this->query.append(this->table);
  this->query.append("\"");
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief PgSQLTruncate constructor.
 *
 *  Initialize the PgSQL TRUNCATE query.
 *
 *  \param[in] pgconn PostgreSQL connection object.
 */
PgSQLTruncate::PgSQLTruncate(PGconn* pgconn) : PgSQLQuery(pgconn) {}

/**
 *  \brief PgSQLTruncate destructor.
 *
 *  Release all acquired ressources.
 */
PgSQLTruncate::~PgSQLTruncate() {}

/**
 *  \brief Execute the query.
 *
 *  Execute the TRUNCATE query on the specified table.
 */
void PgSQLTruncate::Execute()
{
  // Build the query string if necessary
  if (this->stmt_name.empty())
    this->BuildQuery();

  // Really execute the query
  this->PgSQLQuery::Execute();

  return ;
}

/**
 *  \brief Prepare the query.
 *
 *  Prepare the TRUNCATE query on the PostgreSQL server.
 */
void PgSQLTruncate::Prepare()
{
  // Build the query string
  this->BuildQuery();

  // Prepare the statement
  this->PgSQLQuery::Prepare();
  return ;
}
