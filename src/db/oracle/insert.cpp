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

#include "db/oracle/insert.h"

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
void OracleInsert::GenerateQueryBeginning()
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

/**
 *  \brief Get the number of arguments this query accepts.
 *
 *  The number of arguments in this query is equal to the number of fields.
 *  Each field can be set after the query has been prepared. This method is
 *  used by OracleHaveArgs to allocate memory for those arguments.
 *
 *  \return Number of arguments the query accepts.
 */
unsigned int OracleInsert::GetArgCount() throw ()
{
  // XXX : not really exception proof
  return (this->fields.size());
}

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief OracleInsert copy constructor.
 *
 *  Build the new OracleInsert object by copying data from the given object.
 *
 *  \param[in] oinsert Object to copy data from.
 */
OracleInsert::OracleInsert(const OracleInsert& oinsert)
  : HaveArgs(oinsert),
    HaveFields(oinsert),
    Query(oinsert),
    Insert(oinsert),
    OracleHaveArgs(oinsert) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] oinsert Object to copy data from.
 *
 *  \return *this
 */
OracleInsert& OracleInsert::operator=(const OracleInsert& oinsert)
{
  this->Insert::operator=(oinsert);
  this->OracleHaveArgs::operator=(oinsert);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief OracleInsert constructor.
 *
 *  Build the Oracle INSERT query using the connection object on which the
 *  query will operate.
 *
 *  \param[in] oconn Oracle connection object.
 */
OracleInsert::OracleInsert(OCI_Connection* oconn) : OracleHaveArgs(oconn) {}

/**
 *  \brief OracleInsert destructor.
 *
 *  Release all acquired ressources.
 */
OracleInsert::~OracleInsert() {}

/**
 *  \brief Execute the INSERT query.
 *
 *  Send the query on the Oracle server to execute it.
 */
void OracleInsert::Execute()
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
  this->OracleHaveArgs::Execute();

  return ;
}

/**
 *  Get the primary key of the last inserted element.
 *
 *  \return Primary key of the last inserted element.
 */
unsigned int OracleInsert::InsertId()
{
  // XXX
}

/**
 *  \brief Prepare the INSERT query on the Oracle server.
 *
 *  Prepare the Oracle statement on the server for a later execution.
 */
void OracleInsert::Prepare()
{
  unsigned int fields_nb;

  // Generate the first part of the query
  this->GenerateQueryBeginning();

  // Append as many :X as fields
  fields_nb = this->fields.size();
  for (unsigned int i = 1; i <= fields_nb; ++i)
    {
      std::ostringstream ss;

      ss << ':' << i << ", ";
      this->query.append(ss.str());
    }
  this->query.resize(this->query.size() - 2);
  this->query.append(")");

  // Prepare the query against the DB server
  this->OracleHaveArgs::Prepare();

  return ;
}

/**
 *  Set the next argument as a bool.
 *
 *  \param[in] arg Next argument value.
 */
void OracleInsert::SetArg(bool arg)
{
  if (!this->stmt && this->query.empty())
    this->GenerateQueryBeginning();
  this->OracleHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a double.
 *
 *  \param[in] arg Next argument value.
 */
void OracleInsert::SetArg(double arg)
{
  if (!this->stmt && this->query.empty())
    this->GenerateQueryBeginning();
  this->OracleHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as an int.
 *
 *  \param[in] arg Next argument value.
 */
void OracleInsert::SetArg(int arg)
{
  if (!this->stmt && this->query.empty())
    this->GenerateQueryBeginning();
  this->OracleHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a short.
 *
 *  \param[in] arg Next argument value.
 */
void OracleInsert::SetArg(short arg)
{
  if (!this->stmt && this->query.empty())
    this->GenerateQueryBeginning();
  this->OracleHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a string.
 *
 *  \param[in] arg Next argument value.
 */
void OracleInsert::SetArg(const std::string& arg)
{
  if (!this->stmt && this->query.empty())
    this->GenerateQueryBeginning();
  this->OracleHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a time_t.
 *
 *  \param[in] arg Next argument value.
 */
void OracleInsert::SetArg(time_t arg)
{
  this->OracleInsert::SetArg((int)arg);
  return ;
}
