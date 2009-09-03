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

#include <sstream>
#include "db/postgresql/insert.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Generate the beginning of the query string.
 *
 *  Generate the "INSERT INTO table(fields) VALUES(" part of the query. This
 *  method is used internaly for prepared statement and plain text query.
 */
void PgSQLInsert::GenerateQueryBeginning()
{
  // First part, table on which the query will operate
  this->query = "INSERT INTO \"";
  this->query.append(this->table);
  this->query.append("\"(");

  // Extract fields names
  for (std::list<std::string>::iterator it = this->fields.begin();
       it != this->fields.end();
       it++)
    {
      this->query.append("\"");
      this->query.append(*it);
      this->query.append("\", ");
    }
  this->query.resize(this->query.size() - 2);

  // Prepare for values insertions
  this->query.append(") VALUES(");

  return ;
}

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief PgSQLInsert copy constructor.
 *
 *  Build the new PgSQLInsert object by copying data from the given object.
 *
 *  \param[in] pginsert Object to copy data from.
 */
PgSQLInsert::PgSQLInsert(const PgSQLInsert& pginsert)
  : HaveArgs(pginsert),
    HaveFields(pginsert),
    Query(pginsert),
    Insert(pginsert),
    PgSQLHaveArgs(pginsert) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] pginsert Object to copy data from.
 *
 *  \return *this
 */
PgSQLInsert& PgSQLInsert::operator=(const PgSQLInsert& pginsert)
{
  this->Insert::operator=(pginsert);
  this->PgSQLHaveArgs::operator=(pginsert);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief PgSQLInsert constructor.
 *
 *  Build the PostgreSQL INSERT query using the connection object on which the
 *  query will operate.
 *
 *  \param[in] pgconn PgSQL connection object.
 */
PgSQLInsert::PgSQLInsert(PGconn* pgconn) : PgSQLHaveArgs(pgconn) {}

/**
 *  \brief PgSQLInsert destructor.
 *
 *  Release all acquired ressources.
 */
PgSQLInsert::~PgSQLInsert() {}

/**
 *  \brief Execute the INSERT query.
 *
 *  Send the query on the PostgreSQL server to execute it.
 */
void PgSQLInsert::Execute()
{
  // If the query has not been prepared, the current state of the query is
  // 'INSERT INTO "table"(field1, ..., fieldN) VALUES(value1, ..., valueN, ' so
  // finish it
  if (this->stmt_name.empty())
    {
      // XXX : fix because the query object cannot handle a second execution
      this->query.resize(this->query.size() - 2);
      this->query.append(")");
    }

  // Execute the query
  this->PgSQLHaveArgs::Execute();

  return ;
}

/**
 *  Get the primary key of the last inserted element.
 *
 *  \return Primary key of the last inserted element.
 */
unsigned int PgSQLInsert::InsertId()
{
  // XXX : select currval(id) from table;
}

/**
 *  \brief Prepare the INSERT query on the PostgreSQL server.
 *
 *  Prepare the INSERT statement on the server for a later execution.
 */
void PgSQLInsert::Prepare()
{
  int fields_nb;

  // Generate the first part of the query
  this->GenerateQueryBeginning();

  // Append as many ? as fields
  fields_nb = this->fields.size();
  for (int i = 0; i < fields_nb; i++)
    {
      std::stringstream ss;

      ss << '$' << i + 1;
      this->query.append(ss.str());
      this->query.append(", ");
    }
  this->query.resize(this->query.size() - 2);
  this->query.append(")");

  // Prepare the query against the DB server
  this->PgSQLHaveArgs::Prepare();

  return ;
}

/**
 *  Set the next argument as a bool.
 *
 *  \param[in] arg Next argument value.
 */
void PgSQLInsert::SetArg(bool arg)
{
  if (this->stmt_name.empty() && this->query.empty())
    this->GenerateQueryBeginning();
  this->PgSQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a double.
 *
 *  \param[in] arg Next argument value.
 */
void PgSQLInsert::SetArg(double arg)
{
  if (this->stmt_name.empty() && this->query.empty())
    this->GenerateQueryBeginning();
  this->PgSQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as an int.
 *
 *  \param[in] arg Next argument value.
 */
void PgSQLInsert::SetArg(int arg)
{
  if (this->stmt_name.empty() && this->query.empty())
    this->GenerateQueryBeginning();
  this->PgSQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a short.
 *
 *  \param[in] arg Next argument value.
 */
void PgSQLInsert::SetArg(short arg)
{
  if (this->stmt_name.empty() && this->query.empty())
    this->GenerateQueryBeginning();
  this->PgSQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a string.
 *
 *  \param[in] arg Next argument value.
 */
void PgSQLInsert::SetArg(const std::string& arg)
{
  if (this->stmt_name.empty() && this->query.empty())
    this->GenerateQueryBeginning();
  this->PgSQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a time_t.
 *
 *  \param[in] arg Next argument value.
 */
void PgSQLInsert::SetArg(time_t arg)
{
  this->PgSQLInsert::SetArg((int)arg);
  return ;
}
