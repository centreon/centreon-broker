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
#include "db/oracle/select.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief OracleSelect copy constructor.
 *
 *  As this query is not copiable, the copy constructor is declared private.
 *  Any attempt to use this method will result in a call to abort().
 *
 *  \param[in] os Unused.
 */
OracleSelect::OracleSelect(const OracleSelect& os)
  : HaveArgs(os),
    HavePredicate(os),
    Query(os),
    Select(os),
    OracleHaveArgs(os),
    OracleHavePredicate(os)
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
 *  \param[in] os Unused.
 *
 *  \return *this
 */
OracleSelect& OracleSelect::operator=(const OracleSelect& os)
{
  (void)os;
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
void OracleSelect::GenerateQueryStart()
{
  // Start the query string
  this->query = "SELECT ";

  // List all fields to fetch
  for (std::list<std::string>::iterator it = this->fields.begin();
       it != this->fields.end();
       it++)
    {
      this->query.append("\"");
      this->query.append(*it);
      this->query.append("\", ");
    }
  query.resize(query.size() - 2);

  // Set the table
  query.append(" FROM \"");
  query.append(this->table);
  query.append("\"");

  return ;
}

/**
 *  \brief Get the number of arguments the query holds.
 *
 *  Returns the number of variables present in the query. This method is an
 *  override of the OracleHaveArgs's method and is used by the latter class to
 *  allocate memory for prepared statements.
 *
 *  \return The number of settables variables in the query.
 *
 *  \see OracleHaveArgs
 */
unsigned int OracleSelect::GetArgCount() throw ()
{
  return (this->placeholders);
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  \brief OracleSelect default constructor.
 *
 *  Initialize members to their default values.
 */
OracleSelect::OracleSelect(OCI_Connection* oconn) : OracleHaveArgs(oconn) {}

/**
 *  \brief OracleSelect destructor.
 *
 *  Release all acquired ressources.
 */
OracleSelect::~OracleSelect()
{
  if (this->result_set_)
    {
      if (this->plain_stmt)
	OCI_ReleaseResultsets(this->plain_stmt);
      else
	OCI_ReleaseResultsets(this->stmt);
    }
}

/**
 *  \brief Execute a SELECT query.
 *
 *  Execute the SELECT query as it has been configured (prepared, with or
 *  without predicate, ...).
 */
void OracleSelect::Execute()
{
  // Free previous result set
  if (this->result_set_)
    {
      if (this->plain_stmt)
	OCI_ReleaseResultsets(this->plain_stmt);
      else
	OCI_ReleaseResultsets(this->stmt);
      this->result_set_ = NULL;
    }

  // If the query has not been prepared, generate it.
  if (!this->stmt)
    {
      // Generate the first part of the query (SELECT fields from table)
      this->GenerateQueryStart();

      // Generate the predicate string (if any).
      this->OracleHavePredicate::ProcessPredicate(this->query);
    }

  // Execute the query (prepared or not).
  this->OracleHaveArgs::Execute();

  // Extract result set
  if (this->stmt)
    this->result_set_ = OCI_GetResultSet(this->stmt);
  else
    this->result_set_ = OCI_GetResultSet(this->plain_stmt);

  return ;
}

/**
 *  \brief Get the next argument as a bool.
 */
bool OracleSelect::GetBool()
{
  return (OCI_GetInt(this->result_set_, ++this->current_));
}

/**
 *  \brief Get the next argument as a double.
 */
double OracleSelect::GetDouble()
{
  return (OCI_GetDouble(this->result_set_, ++this->current_));
}

/**
 *  \brief Get the next argument as an int.
 */
int OracleSelect::GetInt()
{
  return (OCI_GetInt(this->result_set_, ++this->current_));
}

/**
 *  \brief Get the next argument as a short.
 */
short OracleSelect::GetShort()
{
  return (OCI_GetShort(this->result_set_, ++this->current_));
}

/**
 *  \brief Get the next argument as a string.
 *
 *  \param[out] The string argument will be set in this string.
 */
void OracleSelect::GetString(std::string& str)
{
  str = OCI_GetString(this->result_set_, ++this->current_);
  return ;
}

/**
 *  \brief Move to the next row.
 *
 *  Fetch the next row of the result set. Returns true while there's a row.
 *
 *  \return True if there is an available row.
 */
bool OracleSelect::Next()
{
  this->current_ = 0;
  return (OCI_FetchNext(this->result_set_));
}

/**
 *  \brief Prepare the SELECT query.
 *
 *  Prepare the SELECT query for a later execution. Prepared statements are
 *  useful for multiple execution.
 */
void OracleSelect::Prepare()
{
  // Generate the first part of the query (SELECT fields FROM table)
  this->GenerateQueryStart();

  // Generate the predicate string (if any).
  this->OracleHavePredicate::PreparePredicate(this->query, this->fields.size());

  // Prepare the query against the DB server
  this->OracleHaveArgs::Prepare();

  return ;
}
