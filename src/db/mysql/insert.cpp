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

#include "db/mysql/insert.h"

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
void MySQLInsert::GenerateQueryBeginning()
{
  // First part, table on which the query will operate
  this->query = "INSERT INTO `";
  this->query.append(this->table);
  this->query.append("`(");

  // Extract fields names
  for (std::list<std::string>::iterator it = this->fields.begin();
       it != this->fields.end();
       it++)
    {
      this->query.append(*it);
      this->query.append(", ");
    }
  this->query.resize(this->query.size() - 2);

  // Prepare for values insertions
  this->query.append(") VALUES(");

  return ;
}

/**
 *  \brief Get the number of arguments this query accepts.
 *
 *  The number of arguments in this query is equal to the number of fields.
 *  Each field can be set after the query has been prepared. This method is
 *  used by MySQLHaveArgs to allocate memory for those arguments.
 *
 *  \return Number of arguments the query accepts.
 */
unsigned int MySQLInsert::GetArgCount() throw ()
{
  size_t size;

  try
    {
      size = this->fields.size();
    }
  catch (...)
    {
      size = 0;
    }
  return (size);
}

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief MySQLInsert copy constructor.
 *
 *  Build the new MySQLInsert object by copying data from the given object.
 *
 *  \param[in] myinsert Object to copy data from.
 */
MySQLInsert::MySQLInsert(const MySQLInsert& myinsert)
  : HaveArgs(myinsert),
    HaveFields(myinsert),
    Query(myinsert),
    Insert(myinsert),
    MySQLHaveArgs(myinsert) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] myinsert Object to copy data from.
 *
 *  \return *this
 */
MySQLInsert& MySQLInsert::operator=(const MySQLInsert& myinsert)
{
  this->Insert::operator=(myinsert);
  this->MySQLHaveArgs::operator=(myinsert);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief MySQLInsert constructor.
 *
 *  Build the MySQL INSERT query using the connection object on which the query
 *  will operate.
 *
 *  \param[in] myconn MySQL connection object.
 */
MySQLInsert::MySQLInsert(MYSQL* myconn) : MySQLHaveArgs(myconn) {}

/**
 *  \brief MySQLInsert destructor.
 *
 *  Release all acquired ressources.
 */
MySQLInsert::~MySQLInsert() {}

/**
 *  \brief Execute the INSERT query.
 *
 *  Send the query on the MySQL server to execute it.
 */
void MySQLInsert::Execute()
{
  // If the query has not been prepared, the current state of the query is
  // "INSERT INTO `table`(field1, ..., fieldN) VALUES(value1, ..., valueN, " so
  // finish it
  if (!this->stmt)
    {
      // XXX : fix because the query object cannot handle a second execution
      this->query.resize(this->query.size() - 2);
      this->query.append(")");
    }

  // Execute the query
  this->MySQLHaveArgs::Execute();

  return ;
}

/**
 *  Get the primary key of the last inserted element.
 *
 *  \return Primary key of the last inserted element.
 */
unsigned int MySQLInsert::InsertId()
{
  return (this->stmt ? mysql_stmt_insert_id(this->stmt)
                     : mysql_insert_id(this->mysql));
}

/**
 *  \brief Prepare the INSERT query on the MySQL server.
 *
 *  Prepare the INSERT statement on the server for a later execution.
 */
void MySQLInsert::Prepare()
{
  unsigned int fields_nb;

  // Generate the first part of the query
  this->GenerateQueryBeginning();

  // Append as many ? as fields
  fields_nb = this->fields.size();
  for (unsigned int i = 0; i < fields_nb; ++i)
    this->query.append("?, ");
  this->query.resize(this->query.size() - 2);
  this->query.append(")");

  // Prepare the query against the DB server
  this->MySQLHaveArgs::Prepare();

  return ;
}

/**
 *  Set the next argument as a bool.
 *
 *  \param[in] arg Next argument value.
 */
void MySQLInsert::SetArg(bool arg)
{
  if (!this->stmt && this->query.empty())
    this->GenerateQueryBeginning();
  this->MySQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a double.
 *
 *  \param[in] arg Next argument value.
 */
void MySQLInsert::SetArg(double arg)
{
  if (!this->stmt && this->query.empty())
    this->GenerateQueryBeginning();
  this->MySQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as an int.
 *
 *  \param[in] arg Next argument value.
 */
void MySQLInsert::SetArg(int arg)
{
  if (!this->stmt && this->query.empty())
    this->GenerateQueryBeginning();
  this->MySQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a short.
 *
 *  \param[in] arg Next argument value.
 */
void MySQLInsert::SetArg(short arg)
{
  if (!this->stmt && this->query.empty())
    this->GenerateQueryBeginning();
  this->MySQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a string.
 *
 *  \param[in] arg Next argument value.
 */
void MySQLInsert::SetArg(const std::string& arg)
{
  if (!this->stmt && this->query.empty())
    this->GenerateQueryBeginning();
  this->MySQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a time_t.
 *
 *  \param[in] arg Next argument value.
 */
void MySQLInsert::SetArg(time_t arg)
{
  this->MySQLInsert::SetArg((int)arg);
  return ;
}
