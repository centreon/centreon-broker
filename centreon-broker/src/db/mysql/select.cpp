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
#include "db/mysql/select.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief MySQLSelect copy constructor.
 *
 *  As this query is not copiable, the copy constructor is declared private.
 *  Any attempt to use this method will result in a call to abort().
 *
 *  \param[in] mys Unused.
 */
MySQLSelect::MySQLSelect(const MySQLSelect& mys)
  : HaveArgs(mys),
    HavePredicate(mys),
    Query(mys),
    Select(mys),
    MySQLHaveArgs(mys),
    MySQLHavePredicate(mys)
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
 *  \param[in] mys Unused.
 *
 *  \return *this
 */
MySQLSelect& MySQLSelect::operator=(const MySQLSelect& mys)
{
  (void)mys;
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
void MySQLSelect::GenerateQueryStart()
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
  query.append(" FROM ");
  query.append(this->table);

  return ;
}

/**
 *  \brief Get the number of arguments the query holds.
 *
 *  Returns the number of variables present in the query. This method is an
 *  override of the MySQLHaveArgs's method and is used by the latter class to
 *  allocate memory for prepared statements.
 *
 *  \return The number of settables variables in the query.
 *
 *  \see MySQLHaveArgs
 */
unsigned int MySQLSelect::GetArgCount() throw ()
{
  return (this->placeholders);
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  \brief MySQLSelect default constructor.
 *
 *  Initialize members to their default values.
 */
MySQLSelect::MySQLSelect(MYSQL* mysql) : MySQLHaveArgs(mysql) {}

/**
 *  \brief MySQLSelect destructor.
 *
 *  Release all acquired ressources.
 */
MySQLSelect::~MySQLSelect()
{
  if (this->stmt)
    ; // XXX : free result_.stmt
  else
    if (this->result_.std.res)
      mysql_free_result(this->result_.std.res);
}

/**
 *  \brief Execute a SELECT query.
 *
 *  Execute the SELECT query as it has been configured (prepared, with or
 *  without predicate, ...).
 */
void MySQLSelect::Execute()
{
  // If the query has not been prepared, generate it.
  if (!this->stmt)
    {
      // Generate the first part of the query (SELECT fields from table)
      this->GenerateQueryStart();

      // Generate the predicate string (if any).
      this->MySQLHavePredicate::ProcessPredicate(this->query);
    }

  // Execute the query (prepared or not).
  this->MySQLHaveArgs::Execute();

  // Extract the result set
  if (!this->stmt)
    {
      this->result_.std.res = mysql_use_result(this->mysql);
      if (!this->result_.std.res)
        throw (DBException(mysql_errno(this->mysql),
                           DBException::QUERY_EXECUTION,
                           mysql_error(this->mysql)));
    }

  return ;
}

/**
 *  \brief Get the next argument as a bool.
 */
bool MySQLSelect::GetBool()
{
  bool ret;

  if (this->stmt)
    {
      if (this->result_.stmt[this->current_].buffer_type != MYSQL_TYPE_TINY)
	throw (DBException(0,
                           DBException::QUERY_EXECUTION,
                           "Tried to fetch bool column"));
      ret = *(bool*)(this->result_.stmt[this->current_].buffer);
    }
  else
    ret = *(bool*)(this->result_.std.row[this->current_]);
  this->current_++;
  return (ret);
}

/**
 *  \brief Get the next argument as a double.
 */
double MySQLSelect::GetDouble()
{
  double ret;

  if (this->stmt)
    {
      if (this->result_.stmt[this->current_].buffer_type != MYSQL_TYPE_DOUBLE)
	throw (DBException(0,
                           DBException::QUERY_EXECUTION,
                           "Tried to fetch double column"));
      ret = *(double*)(this->result_.stmt[this->current_].buffer);
    }
  else
    ret = *(double*)(this->result_.std.row[this->current_]);
  this->current_++;
  return (ret);
}

/**
 *  \brief Get the next argument as an int.
 */
int MySQLSelect::GetInt()
{
  int ret;

  if (this->stmt)
    {
      if (this->result_.stmt[this->current_].buffer_type != MYSQL_TYPE_LONG)
	throw (DBException(0,
                           DBException::QUERY_EXECUTION,
                           "Tried to fetch int column"));
      ret = *(int*)(this->result_.stmt[this->current_].buffer);
    }
  else
    {
      unsigned long length;
      char* buffer;

      length = mysql_fetch_lengths(this->result_.std.res)[this->current_];
      buffer = new char[length + 1];
      memcpy(buffer, this->result_.std.row[this->current_], length);
      buffer[length] = '\0';
      ret = strtol(buffer, NULL, 0);
      delete [] buffer;
    }
  this->current_++;
  return (ret);
}

/**
 *  \brief Get the next argument as a short.
 */
short MySQLSelect::GetShort()
{
  short ret;

  if (this->stmt)
    {
      if (this->result_.stmt[this->current_].buffer_type != MYSQL_TYPE_SHORT)
	throw (DBException(0,
                           DBException::QUERY_EXECUTION,
                           "Tried to fetch short column"));
      ret = *(short*)(this->result_.stmt[this->current_].buffer);
    }
  else
    ret = *(short*)(this->result_.std.row[this->current_]);
  this->current_++;
  return (ret);
}

/**
 *  \brief Get the next argument as a string.
 *
 *  \param[out] The string argument will be set in this string.
 */
void MySQLSelect::GetString(std::string& str)
{
  if (this->stmt)
    {
      if (this->result_.stmt[this->current_].buffer_type != MYSQL_TYPE_STRING)
	throw (DBException(0,
                           DBException::QUERY_EXECUTION,
                           "Tried to fetch string column"));
      str = (char*)(this->result_.stmt[this->current_].buffer);
    }
  else
    str = (char*)(this->result_.std.row[this->current_]);
  this->current_++;
  return ;
}

/**
 *  \brief Move to the next row.
 *
 *  Fetch the next row of the result set. Returns true while there's a row.
 *
 *  \return True if there is an available row.
 */
bool MySQLSelect::Next()
{
  bool ret;

  // Prepared statement
  if (this->stmt)
    {
      int ec;

      ec = mysql_stmt_fetch(this->stmt);
      if (1 == ec)
	throw (DBException(mysql_stmt_errno(this->stmt),
                           DBException::QUERY_EXECUTION,
                           mysql_stmt_error(this->stmt)));
      else
	ret = !(ec && (ec != MYSQL_DATA_TRUNCATED));
    }

  // Standard query
  else
    {
      this->result_.std.row = mysql_fetch_row(this->result_.std.res);
      if (NULL == this->result_.std.row)
	{
	  mysql_free_result(this->result_.std.res);
	  this->result_.std.res = NULL;
	  ret = false;
	}
      else
	ret = true;
    }

  // Reset column counter
  this->current_ = 0;

  return (ret);
}

/**
 *  \brief Prepare the SELECT query.
 *
 *  Prepare the SELECT query for a later execution. Prepared statements are
 *  useful for multiple execution.
 */
void MySQLSelect::Prepare()
{
  // Generate the first part of the query (SELECT fields FROM table)
  this->GenerateQueryStart();

  // Generate the predicate string (if any).
  this->MySQLHavePredicate::PreparePredicate(this->query);

  // Prepare the query against the DB server
  this->MySQLHaveArgs::Prepare();

  return ;
}
