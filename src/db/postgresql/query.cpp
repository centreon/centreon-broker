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
 *  PgSQLQuery copy constructor. Because queries are not copyable, this is
 *  declared private.
 */
PgSQLQuery::PgSQLQuery(const PgSQLQuery& pgquery) throw () : Query()
{
  (void)pgquery;
}

/**
 *  PgSQLQuery operator= overload. Because queries are not copyable, this is
 *  declared private.
 */
PgSQLQuery& PgSQLQuery::operator=(const PgSQLQuery& pgquery) throw ()
{
  (void)pgquery;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  PgSQLQuery constructor. It needs the PgSQL connection object from which it
 *  depends.
 */
PgSQLQuery::PgSQLQuery(PGconn* pgconn) throw ()
  : nparams_(0),
    param_format_(NULL),
    param_length_(NULL),
    param_values_(NULL),
    pgconn_(pgconn)
{
}

/**
 *  PgSQLQuery destructor.
 */
PgSQLQuery::~PgSQLQuery()
{
  assert(this->pgconn_);
  // XXX : DEALLOCATE statement
}

/**
 *  Execute the query.
 */
void PgSQLQuery::Execute() throw (DBException)
{
  PGresult* res;

#ifndef NDEBUG
  logging.LogDebug("Executing PostgreSQL prepared statement...");
#endif /* !NDEBUG */
  assert(!this->stmt_name_.empty());
  res = PQexecPrepared(this->pgconn_,
		       this->stmt_name_.c_str(),
		       this->nparams_,
		       this->param_values_,
		       this->param_length_,
		       this->param_format_,
		       0);
  if (!res)
    throw (DBException(0,
		       DBException::QUERY_EXECUTION,
		       "Could not allocate memory."));
  else if (PQresultStatus(res) != PGRES_COMMAND_OK
	   && PQresultStatus(res) != PGRES_TUPLES_OK)
    throw (DBException(0,
                       DBException::QUERY_EXECUTION,
                       PQresultErrorMessage(res)));
  return ;
}

/**
 *  Prepare the query.
 */
void PgSQLQuery::Prepare()
{
#ifndef NDEBUG
  logging.LogDebug("Preparing PostgreSQL statement...", true);
  logging.LogDebug(this->query_.c_str());
  logging.Deindent();
#endif /* !NDEBUG */
  // Generate the statement name
  {
    std::stringstream ss;

    // XXX : query_nb should be locked
    ss << "stmt" << ++query_nb;
    this->stmt_name_ = ss.str();
  }
  // Prepare the query
  {
    PGresult* res;

    res = PQprepare(this->pgconn_,
		    this->stmt_name_.c_str(),
		    this->query_.c_str(),
		    0,
		    NULL);
    if (!res)
      {
	this->stmt_name_.clear();
	throw (DBException(0,
			   DBException::QUERY_PREPARATION,
			   "Could not allocate memory."));
      }
    else if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
	DBException dbe(0,
			DBException::QUERY_PREPARATION,
			PQresultErrorMessage(res));

	this->stmt_name_.clear();
	PQclear(res);
	throw (dbe);
      }
  }
  return ;
}
