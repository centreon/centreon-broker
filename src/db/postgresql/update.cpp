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

#include <cstring>
#include <sstream>
#include "db/db_exception.h"
#include "db/postgresql/update.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Check if it's the first time an argument is set.
 *
 *  If it is the first time, the query beginning will be generated. This method
 *  is called only when using plain text query.
 */
void PgSQLUpdate::CheckArg()
{
  if (this->query.empty())
    {
      this->GenerateQueryBeginning();
      this->field_ = this->fields.begin();
    }
  this->query.append(*(this->field_));
  this->field_++;
  this->query.append("=");
  return ;
}

/**
 *  \brief Generate the beginning of the UPDATE query.
 *
 *  Generate the part common to prepared statements and normal queries :
 *  "UPDATE table SET ".
 */
void PgSQLUpdate::GenerateQueryBeginning()
{
  this->query = "UPDATE \"";
  this->query.append(this->table);
  this->query.append("\" SET ");
  return ;
}

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief PgSQLUpdate copy constructor.
 *
 *  Build the new instance by copying data from the given object.
 *
 *  \param[in] pgupdate Object to copy data from.
 */
PgSQLUpdate::PgSQLUpdate(const PgSQLUpdate& pgupdate)
  : HaveArgs(pgupdate),
    Query(pgupdate),
    HavePredicate(pgupdate),
    HaveFields(pgupdate),
    Update(pgupdate),
    PgSQLHaveArgs(pgupdate),
    PgSQLHavePredicate(pgupdate) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] pgupdate Object to copy data from.
 *
 *  \return *this
 */
PgSQLUpdate& PgSQLUpdate::operator=(const PgSQLUpdate& pgupdate)
{
  this->PgSQLHaveArgs::operator=(pgupdate);
  this->PgSQLHavePredicate::operator=(pgupdate);
  this->Update::operator=(pgupdate);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief PgSQLUpdate constructor.
 *
 *  Build the UPDATE query using the PostgreSQL connection object on which the
 *  query will operate.
 *
 *  \param[in] pgconn PostgreSQL connection object.
 */
PgSQLUpdate::PgSQLUpdate(PGconn* pgconn) : PgSQLHaveArgs(pgconn) {}

/**
 *  \brief PgSQLUpdate destructor.
 *
 *  Release acquired ressources.
 */
PgSQLUpdate::~PgSQLUpdate() {}

/**
 *  \brief Execute the update.
 *
 *  Effectively execute the UPDATE query on the PostgreSQL server.
 */
void PgSQLUpdate::Execute()
{
  // XXX : does not support multiple execution
  if (this->stmt_name.empty())
    this->ProcessPredicate(this->query);
  this->PgSQLHaveArgs::Execute();
  return ;
}

/**
 *  \brief Return the number of row affected by the last query.
 *
 *  Ask the PostgreSQL server how many rows were affected by the last query executed
 *  by this object.
 *
 *  \return Number of rows affected by the last query.
 */
unsigned int PgSQLUpdate::GetUpdateCount()
{
  std::string query;
  PGresult* result;
  unsigned int ret;

  query = "SELECT currval(pg_get_serial_sequence('";
  query.append(this->table);
  query.append("', 'id'))");
  result = PQexec(this->pgconn_, query.c_str());
  if (!result)
    throw (DBException(0,
                       DBException::QUERY_EXECUTION,
                       "Unsufficient memory to retrieve last inserted id."));
  else if (PQresultStatus(result) != PGRES_COMMAND_OK
           && PQresultStatus(result) != PGRES_TUPLES_OK)
    {
      DBException dbe(0,
                      DBException::QUERY_EXECUTION,
                      PQresultErrorMessage(result));

      PQclear(result);
      throw (dbe);
    }
  ret = strtoul(PQgetvalue(result, 0, 0), NULL, 0);
  PQclear(result);
  return (ret);
}

/**
 *  \brief Prepare the query for execution.
 *
 *  Prepare the UPDATE statement on the PostgreSQL server for later execution.
 */
void PgSQLUpdate::Prepare()
{
  // "UPDATE table SET "
  this->GenerateQueryBeginning();

  // Append "field1=?, field2=?, ..., fieldN=?"
  unsigned int i = 0;
  for (std::list<std::string>::iterator it = this->fields.begin();
       it != this->fields.end();
       ++it)
    {
      std::ostringstream ss;

      ss << *it << "=$" << ++i << ", ";
      this->query.append(ss.str());
    }
  this->query.resize(this->query.size() - 2);

  // Append predicate (if any)
  this->PreparePredicate(this->query, this->fields.size());

  // Prepare the query against the server
  this->PgSQLHaveArgs::Prepare();

  return ;
}

/**
 *  Set the next argument as a bool.
 *
 *  \param[in] arg Next argument value.
 */
void PgSQLUpdate::SetArg(bool arg)
{
  if (this->stmt_name.empty())
    this->CheckArg();
  this->PgSQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a double.
 *
 *  \param[in] arg Next argument value.
 */
void PgSQLUpdate::SetArg(double arg)
{
  if (this->stmt_name.empty())
    this->CheckArg();
  this->PgSQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as an int.
 *
 *  \param[in] arg Next argument value.
 */
void PgSQLUpdate::SetArg(int arg)
{
  if (this->stmt_name.empty())
    this->CheckArg();
  this->PgSQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a short.
 *
 *  \param[in] arg Next argument value.
 */
void PgSQLUpdate::SetArg(short arg)
{
  if (this->stmt_name.empty())
    this->CheckArg();
  this->PgSQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a string.
 *
 *  \param[in] arg Next argument value.
 */
void PgSQLUpdate::SetArg(const std::string& arg)
{
  if (this->stmt_name.empty())
    this->CheckArg();
  this->PgSQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a time_t.
 *
 *  \param[in] arg Next argument value.
 */
void PgSQLUpdate::SetArg(time_t arg)
{
  this->PgSQLUpdate::SetArg((int)arg);
  return ;
}
