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
 *  \brief Clean arguments structures.
 *
 *  Release memory associated with arguments structures for prepared
 *  statements.
 */
void MySQLSelect::CleanArgs()
{
  if (this->result_.stmt.res)
    {
      for (unsigned int i = 0; i < this->result_.stmt.count; ++i)
	{
	  MYSQL_BIND* bind;

	  bind = this->result_.stmt.res + i;
	  switch (bind->buffer_type)
	    {
             case MYSQL_TYPE_TINY:
	      delete (static_cast<char*>(bind->buffer));
	      delete (static_cast<unsigned long*>(bind->length));
	      break ;
             case MYSQL_TYPE_DOUBLE:
	      delete (static_cast<double*>(bind->buffer));
	      delete (static_cast<unsigned long*>(bind->length));
	      break ;
             case MYSQL_TYPE_LONG:
	      delete (static_cast<int*>(bind->buffer));
	      delete (static_cast<unsigned long*>(bind->length));
	      break ;
             case MYSQL_TYPE_SHORT:
	      delete (static_cast<short*>(bind->buffer));
	      delete (static_cast<unsigned long*>(bind->length));
	      break ;
             case MYSQL_TYPE_BLOB:
             case MYSQL_TYPE_STRING:
             case MYSQL_TYPE_VAR_STRING:
	      delete [] (static_cast<char*>(bind->buffer));
	      delete (static_cast<unsigned long*>(bind->length));
	      break ;
	    };
	}
      delete [] this->result_.stmt.res;
    }
  return ;
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
MySQLSelect::MySQLSelect(MYSQL* mysql) : MySQLHaveArgs(mysql)
{
  this->result_.std.res = NULL;
}

/**
 *  \brief MySQLSelect destructor.
 *
 *  Release all acquired ressources.
 */
MySQLSelect::~MySQLSelect()
{
  if (this->stmt)
    this->CleanArgs();
  else if (this->result_.std.res)
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
      // Generate the first part of the query (SELECT fields from table).
      this->GenerateQueryStart();

      // Generate the predicate string (if any).
      this->MySQLHavePredicate::ProcessPredicate(this->query);
    }

  // Execute the query (prepared or not).
  this->MySQLHaveArgs::Execute();

  // Extract the result set.
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
      if (this->result_.stmt.res[this->current_].buffer_type
          != MYSQL_TYPE_TINY)
        throw (DBException(0,
                           DBException::QUERY_EXECUTION,
                           "Tried to fetch bool column"));
      ret = *(bool*)(this->result_.stmt.res[this->current_].buffer);
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
      if (this->result_.stmt.res[this->current_].buffer_type
          != MYSQL_TYPE_DOUBLE)
        throw (DBException(0,
                           DBException::QUERY_EXECUTION,
                           "Tried to fetch double column"));
      ret = *(double*)(this->result_.stmt.res[this->current_].buffer);
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
      if (this->result_.stmt.res[this->current_].buffer_type
          != MYSQL_TYPE_LONG)
        throw (DBException(0,
                           DBException::QUERY_EXECUTION,
                           "Tried to fetch int column"));
      ret = *(int*)(this->result_.stmt.res[this->current_].buffer);
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
      if (this->result_.stmt.res[this->current_].buffer_type
          != MYSQL_TYPE_SHORT)
        throw (DBException(0,
                           DBException::QUERY_EXECUTION,
                           "Tried to fetch short column"));
      ret = *(short*)(this->result_.stmt.res[this->current_].buffer);
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
      if (this->result_.stmt.res[this->current_].buffer_type != MYSQL_TYPE_STRING)
        throw (DBException(0,
                           DBException::QUERY_EXECUTION,
                           "Tried to fetch string column"));
      str = (char*)(this->result_.stmt.res[this->current_].buffer);
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

  // Create and bind arguments buffer.
  this->result_.stmt.res = NULL;
  {
    MYSQL_RES* metadata;

    metadata = mysql_stmt_result_metadata(this->stmt);
    if (!metadata)
      throw (DBException(mysql_stmt_errno(this->stmt),
                         DBException::QUERY_PREPARATION,
                         mysql_stmt_error(this->stmt)));

    try
      {
	// Create result table.
	this->result_.stmt.count = mysql_num_fields(metadata);
	this->result_.stmt.res = new MYSQL_BIND[this->result_.stmt.count];
	memset(this->result_.stmt.res,
               0,
               sizeof(*this->result_.stmt.res) * this->result_.stmt.count);

	// Initialize each result structure.
	MYSQL_BIND* bind;
	MYSQL_FIELD* field;
	for (bind = this->result_.stmt.res, field = mysql_fetch_field(metadata);
             field;
             ++bind, field = mysql_fetch_field(metadata))
	  {
	    switch (field->type)
	      {
               case MYSQL_TYPE_TINY:
		bind->buffer = static_cast<void*>(new char);
		bind->buffer_length = sizeof(char);
		bind->length = new unsigned long;
		break ;
               case MYSQL_TYPE_DOUBLE:
		bind->buffer = static_cast<void*>(new double);
		bind->buffer_length = sizeof(double);
		bind->length = new unsigned long;
		break ;
               case MYSQL_TYPE_LONG:
		bind->buffer = static_cast<void*>(new int);
		bind->buffer_length = sizeof(int);
		bind->length = new unsigned long;
		break ;
               case MYSQL_TYPE_BLOB:
               case MYSQL_TYPE_STRING:
               case MYSQL_TYPE_VAR_STRING:
		bind->buffer = static_cast<void*>(new char[1024]);
		bind->buffer_length = 1024;
		bind->length = new unsigned long;
		break ;
	      };
	  }
      }
    // Avoid memory leak by releasing memory.
    catch (...)
      {
	mysql_free_result(metadata);
	this->CleanArgs();
	throw ;
      }
  }

  return ;
}
