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

#include <cassert>
#include <cstdlib>
#include "db/db_exception.h"
#include "db/postgresql/query.h"
#include "logging.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Static Members            *
*                                     *
**************************************/

int PgSQLQuery::query_nb = 0;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief PgSQLQuery copy constructor.
 *
 *  Because queries are not copyable, this method is declared private. Any
 *  attempt to use it will result in a call to abort().
 *
 *  \param[in] pgquery Unused.
 */
PgSQLQuery::PgSQLQuery(const PgSQLQuery& pgquery) : Query(pgquery)
{
  (void)pgquery;
  assert(false);
  abort();
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Because queries are not copyable, this method is declared private. Any
 *  attempt to use it will result in a call to abort().
 *
 *  \param[in] pgquery Unused.
 *
 *  \return *this
 */
PgSQLQuery& PgSQLQuery::operator=(const PgSQLQuery& pgquery)
{
  (void)pgquery;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief PgSQLQuery constructor.
 *
 *  Build a PostgreSQL query using the PostgreSQL connection object from which
 *  it depends.
 *
 *  \param[in] pgconn PostgreSQL connection object.
 */
PgSQLQuery::PgSQLQuery(PGconn* pgconn)
  : pgconn_(pgconn),
    result(NULL) {}

/**
 *  \brief PgSQLQuery destructor.
 *
 *  Delete all ressources acquired by this query.
 */
PgSQLQuery::~PgSQLQuery()
{
  // XXX : DEALLOCATE statement
  if (this->result)
    PQclear(this->result);
}

/**
 *  Execute the query.
 */
void PgSQLQuery::Execute()
{
  if (this->result)
    PQclear(this->result);
  if (!this->stmt_name.empty())
    {
#ifndef NDEBUG
      logging.LogDebug("Executing PostgreSQL prepared statement...");
#endif /* !NDEBUG */
      this->result = PQexecPrepared(this->pgconn_,
                                    this->stmt_name.c_str(),
                                    0,
                                    NULL,
                                    NULL,
                                    NULL,
                                    0);
    }
  else
    {
#ifndef NDEBUG
      logging.LogDebug("Executing PostgreSQL standard query ...");
      logging.LogDebug(this->query.c_str());
#endif /* !NDEBUG */
      this->result = PQexec(this->pgconn_, this->query.c_str());
    }
  if (!this->result)
    throw (DBException(0,
                       DBException::QUERY_EXECUTION,
                       "Could not allocate memory."));
  else if (PQresultStatus(this->result) != PGRES_COMMAND_OK
           && PQresultStatus(this->result) != PGRES_TUPLES_OK)
    {
      DBException dbe(0,
                      DBException::QUERY_EXECUTION,
                      PQresultErrorMessage(this->result));

      PQclear(this->result);
      this->result = NULL;
      throw (dbe);
    }
  return ;
}

/**
 *  Execute the query.
 */
void PgSQLQuery::Execute(unsigned int arg_count,
                         char** values,
                         int* lengths,
                         int* format)
{
  if (this->result)
    PQclear(this->result);
#ifndef NDEBUG
  logging.LogDebug("Executing PostgreSQL prepared statement...");
#endif /* !NDEBUG */
  this->result = PQexecPrepared(this->pgconn_,
                                this->stmt_name.c_str(),
                                arg_count,
                                values,
                                lengths,
                                format,
                                0);
  if (!this->result)
    throw (DBException(0,
                       DBException::QUERY_EXECUTION,
                       "Could not allocate memory."));
  else if (PQresultStatus(this->result) != PGRES_COMMAND_OK
           && PQresultStatus(this->result) != PGRES_TUPLES_OK)
    {
      DBException dbe(0,
                      DBException::QUERY_EXECUTION,
                      PQresultErrorMessage(this->result));

      PQclear(this->result);
      this->result = NULL;
      throw (dbe);
    }
  return ;
}

/**
 *  \brief Prepare the query.
 *
 *  Prepare the query on the PostgreSQL server for later execution.
 */
void PgSQLQuery::Prepare()
{
#ifndef NDEBUG
  logging.LogDebug("Preparing PostgreSQL statement...");
  logging.LogDebug(this->query.c_str());
#endif /* !NDEBUG */

  // Generate the statement name
  {
    std::stringstream ss;

    // XXX : query_nb should be locked
    ss << "stmt" << ++query_nb;
    this->stmt_name = ss.str();
  }

  // Prepare the query
  PGresult* res;

  res = PQprepare(this->pgconn_,
                  this->stmt_name.c_str(),
                  this->query.c_str(),
                  0,
                  NULL);
  if (!res)
    {
      this->stmt_name.clear();
      throw (DBException(0,
                         DBException::QUERY_PREPARATION,
                         "Could not allocate memory."));
    }
  else if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      DBException dbe(0,
                      DBException::QUERY_PREPARATION,
                      PQresultErrorMessage(res));

      this->stmt_name.clear();
      PQclear(res);
      throw (dbe);
    }
  return ;
}
