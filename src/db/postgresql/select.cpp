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
#include "db/postgresql/select.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief PgSQLSelect copy constructor.
 *
 *  As this query is not copiable, the copy constructor is declared private.
 *  Any attempt to use this method will result in a call to abort().
 *
 *  \param[in] pgs Unused.
 */
PgSQLSelect::PgSQLSelect(const PgSQLSelect& pgs)
  : HaveArgs(pgs),
    HavePredicate(pgs),
    Query(pgs),
    Select(pgs),
    PgSQLHaveArgs(pgs),
    PgSQLHavePredicate(pgs)
{
  assert(false);
  abort();
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  As this query is not copiable, the assignment operator is declared private.
 *  Any attempt to use this method will result in a call to abort().
 *
 *  \param[in] pgs Unused.
 *
 *  \return *this
 */
PgSQLSelect& PgSQLSelect::operator=(const PgSQLSelect& pgs)
{
  (void)pgs;
  assert(false);
  abort();
  return (*this);
}

/**
 *  \brief Generate the beginning of the SELECT query.
 *
 *  This method build the first part of the query, the one with the SELECT
 *  keyword, the fields and the table. This method is here to avoid code
 *  redundancy and is used by Execute() and Prepare().
 *
 *  \see Execute
 *  \see Prepare
 */
void PgSQLSelect::GenerateQueryStart()
{
  // Start the query string
  this->query = "SELECT ";

  // List all fields to fetch
  for (std::list<std::string>::iterator it = this->fields.begin();
       it != this->fields.end();
       it++)
    {
      this->query.append(*it);
      this->query.append(", ");
    }
  query.resize(query.size() - 2);

  // Set the table
  query.append(" FROM \"");
  query.append(this->table);
  query.append("\"");

  return ;
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  \brief PgSQLSelect default constructor.
 *
 *  Initialize members to their default values.
 */
PgSQLSelect::PgSQLSelect(PGconn* pgconn) : PgSQLHaveArgs(pgconn) {}

/**
 *  \brief PgSQLSelect destructor.
 *
 *  Release all acquired ressources.
 */
PgSQLSelect::~PgSQLSelect()
{
  // XXX
}

/**
 *  \brief Execute a SELECT query.
 *
 *  Execute the SELECT query as it has been configured (prepared, with or
 *  without predicate, ...).
 */
void PgSQLSelect::Execute()
{
  // If the query has not been prepared, generate it.
  if (this->stmt_name.empty())
    {
      // Generate the first part of the query (SELECT fields from table)
      this->GenerateQueryStart();

      // Generate the predicate string (if any).
      this->PgSQLHavePredicate::ProcessPredicate(this->query);
    }

  // Execute the query (prepared or not).
  this->PgSQLHaveArgs::Execute();

  // Reset result set counters
  this->current_col_ = 0;
  this->current_row_ = -1;

  return ;
}

/**
 *  \brief Get the next argument as a bool.
 */
bool PgSQLSelect::GetBool()
{
  bool ret;

  if (PQfformat(this->result, this->current_col_))
    ret = *(bool*)(PQgetvalue(this->result,
                             this->current_row_,
			      this->current_col_));
  else
    ret = strtoul(PQgetvalue(this->result,
                             this->current_row_,
                             this->current_col_),
                  NULL,
                  0);
  this->current_col_++;
  return (ret);
}

/**
 *  \brief Get the next argument as a double.
 */
double PgSQLSelect::GetDouble()
{
  double ret;

  if (PQfformat(this->result, this->current_col_))
    ret = *(double*)(PQgetvalue(this->result,
                             this->current_row_,
			      this->current_col_));
  else
    ret = strtod(PQgetvalue(this->result,
                            this->current_row_,
                            this->current_col_),
                 NULL);
  ++this->current_col_;
  return (ret);
}

/**
 *  \brief Get the next argument as an int.
 */
int PgSQLSelect::GetInt()
{
  int ret;

  if (PQfformat(this->result, this->current_col_))
    ret = *(int*)(PQgetvalue(this->result,
                             this->current_row_,
			      this->current_col_));
  else
    ret = strtol(PQgetvalue(this->result,
                            this->current_row_,
                            this->current_col_),
                 NULL,
                 0);
  ++this->current_col_;
  return (ret);
}

/**
 *  \brief Get the next argument as a short.
 */
short PgSQLSelect::GetShort()
{
  short ret;

  if (PQfformat(this->result, this->current_col_))
    ret = *(short*)(PQgetvalue(this->result,
                               this->current_row_,
			       this->current_col_));
  else
    ret = strtol(PQgetvalue(this->result,
                            this->current_row_,
                            this->current_col_),
                 NULL,
                 0);
  ++this->current_col_;
  return (ret);
}

/**
 *  \brief Get the next argument as a string.
 *
 *  \param[out] The string argument will be set in this string.
 */
void PgSQLSelect::GetString(std::string& str)
{
  str = PQgetvalue(this->result, this->current_row_, this->current_col_);
  ++this->current_col_;
  return ;
}

/**
 *  \brief Move to the next row.
 *
 *  Fetch the next row of the result set. Returns true while there's a row.
 *
 *  \return True if there is an available row.
 */
bool PgSQLSelect::Next()
{
  // Reset counters
  ++this->current_row_;
  this->current_col_ = 0;

  return (PQntuples(this->result) > (int)this->current_row_);
}

/**
 *  \brief Prepare the SELECT query.
 *
 *  Prepare the SELECT query for a later execution. Prepared statements are
 *  useful for multiple execution.
 */
void PgSQLSelect::Prepare()
{
  // Generate the first part of the query (SELECT fields FROM table)
  this->GenerateQueryStart();

  // Generate the predicate string (if any).
  this->PgSQLHavePredicate::PreparePredicate(this->query, this->fields.size());

  // Prepare the query against the DB server
  this->PgSQLHaveArgs::Prepare();

  return ;
}
