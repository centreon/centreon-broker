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
#include "db/mysql/have_args.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief Clean a MYSQL_BIND structure.
 *
 *  MYSQL_BIND structures might hold allocated memory. Release such ressources
 *  hold by the specified MYSQL_BIND structure.
 *
 *  \param[in,out] bind MYSQL_BIND structure to clean.
 */
void MySQLHaveArgs::CleanArg(MYSQL_BIND* bind)
{
  if (bind->buffer)
    switch (bind->buffer_type)
      {
       case MYSQL_TYPE_TINY:
        delete (static_cast<char*>(bind->buffer));
        break ;
       case MYSQL_TYPE_DOUBLE:
        delete (static_cast<double*>(bind->buffer));
        break ;
       case MYSQL_TYPE_LONG:
        delete (static_cast<int*>(bind->buffer));
        break ;
       case MYSQL_TYPE_SHORT:
        delete (static_cast<short*>(bind->buffer));
        break ;
       case MYSQL_TYPE_STRING:
        bind->buffer = NULL;
        break ;
       default:
	;
      }
  memset(bind, 0, sizeof(*bind));
  return ;
}

/**************************************
*                                     *
*         Protected Methods           *
*                                     *
**************************************/

/**
 *  \brief MySQLHaveArgs constructor.
 *
 *  Initialize the new object.
 *
 *  \param[in] mysql The MySQL connection object.
 */
MySQLHaveArgs::MySQLHaveArgs(MYSQL* mysql)
  : MySQLQuery(mysql),
    arg_(0),
    args_(NULL),
    args_count_(0) {}

/**
 *  \brief MySQLHaveArgs copy constructor.
 *
 *  Initialize the new object from the given object.
 *
 *  \param[in] mha Object to copy data from.
 */
MySQLHaveArgs::MySQLHaveArgs(const MySQLHaveArgs& mha)
  : HaveArgs(mha), Query(mha), MySQLQuery(mha)
{
  // XXX : copy arguments
}

/**
 *  \brief MySQLHaveArgs destructor.
 *
 *  Release all ressources previously acquired.
 */
MySQLHaveArgs::~MySQLHaveArgs()
{
  // XXX : arguments destruction
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all arguments of the given object to the current instance.
 *
 *  \param[in] mha Object to copy data from.
 *
 *  \return *this
 */
MySQLHaveArgs& MySQLHaveArgs::operator=(const MySQLHaveArgs& mha)
{
  // XXX : copy arguments
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Execute the query.
 *
 *  When the underlying query is executed, the job of MySQLHaveArgs is to bind
 *  arguments to the statement (in the case of a prepared statement). Then it
 *  transfers control to the MySQLQuery::Execute method.
 */
void MySQLHaveArgs::Execute()
{
  if (this->stmt && mysql_stmt_bind_param(this->stmt, this->args_))
    throw (DBException(mysql_stmt_errno(this->stmt),
                       DBException::QUERY_EXECUTION,
                       mysql_stmt_error(this->stmt)));
  this->arg_ = 0;
  this->MySQLQuery::Execute();
  return ;
}

/**
 *  \brief Prepare the query.
 *
 *  When the underlying query is being prepared, the MySQLHaveArgs object is
 *  responsible of allocating the MYSQL_BIND structure that will hold query
 *  arguments. For this purpose it will call the overriden GetArgCount()
 *  method. Control is then transfered to the MySQLQuery::Prepare method.
 */
void MySQLHaveArgs::Prepare()
{
  this->args_count_ = this->GetArgCount();
  if (this->args_count_)
    {
      this->args_ = new MYSQL_BIND[this->args_count_];
      memset(this->args_, 0, this->args_count_ * sizeof(*this->args_));
      this->arg_ = 0;
    }
  this->MySQLQuery::Prepare();
  return ;
}

/**
 *  Set the next argument as a bool.
 *
 *  \param[in] arg Next value.
 */
void MySQLHaveArgs::SetArg(bool arg)
{
  if (this->stmt)
    {
      MYSQL_BIND* mybind;

      mybind = this->args_ + this->arg_;
      if (mybind->buffer_type != MYSQL_TYPE_TINY)
	{
	  this->CleanArg(mybind);
	  memset(mybind, 0, sizeof(*mybind));
	  mybind->buffer_type = MYSQL_TYPE_TINY;
	  mybind->buffer = static_cast<void*>(new char);
	  mybind->buffer_length = sizeof(char);
	  mybind->length = &(mybind->buffer_length);
	}
      *static_cast<char*>(mybind->buffer) = (arg ? 1 : 0);
      this->arg_++;
    }
  else
    if (arg)
      this->query.append("true");
    else
      this->query.append("false");
  return ;
}

/**
 *  Set the next argument as a double.
 *
 *  \param[in] arg Next value.
 */
void MySQLHaveArgs::SetArg(double arg)
{
  if (this->stmt)
    {
      MYSQL_BIND* mybind;

      mybind = this->args_ + this->arg_;
      if (mybind->buffer_type != MYSQL_TYPE_DOUBLE)
	{
	  this->CleanArg(mybind);
	  memset(mybind, 0, sizeof(*mybind));
	  mybind->buffer_type = MYSQL_TYPE_DOUBLE;
	  mybind->buffer = static_cast<void*>(new double);
	  mybind->buffer_length = sizeof(double);
	  mybind->length = &(mybind->buffer_length);
	}
      *static_cast<double*>(mybind->buffer) = arg;
      this->arg_++;
    }
  else
    {
      std::stringstream ss;

      ss << arg;
      this->query.append(ss.str());
    }
  return ;
}

/**
 *  Set the next argument as an int.
 *
 *  \param[in] arg Next value.
 */
void MySQLHaveArgs::SetArg(int arg)
{
  if (this->stmt)
    {
      MYSQL_BIND* mybind;

      mybind = this->args_ + this->arg_;
      if (mybind->buffer_type != MYSQL_TYPE_LONG)
	{
	  this->CleanArg(mybind);
	  memset(mybind, 0, sizeof(*mybind));
	  mybind->buffer_type = MYSQL_TYPE_LONG;
	  mybind->buffer = static_cast<void*>(new int);
	  mybind->buffer_length = sizeof(int);
	  mybind->length = &(mybind->buffer_length);
	}
      *static_cast<int*>(mybind->buffer) = arg;
      this->arg_++;
    }
  else
    {
      std::stringstream ss;

      ss << arg;
      this->query.append(ss.str());
    }
  return ;
}

/**
 *  Set the next argument as a short.
 *
 *  \param[in] arg Next value.
 */
void MySQLHaveArgs::SetArg(short arg)
{
  if (this->stmt)
    {
      MYSQL_BIND* mybind;

      mybind = this->args_ + this->arg_;
      if (mybind->buffer_type != MYSQL_TYPE_SHORT)
	{
	  this->CleanArg(mybind);
	  memset(mybind, 0, sizeof(*mybind));
	  mybind->buffer_type = MYSQL_TYPE_SHORT;
	  mybind->buffer = static_cast<void*>(new short);
	  mybind->buffer_length = sizeof(short);
	  mybind->length = &(mybind->buffer_length);
	}
      *static_cast<short*>(mybind->buffer) = arg;
      this->arg_++;
    }
  else
    {
      std::stringstream ss;

      ss << arg;
      this->query.append(ss.str());
    }
  return ;
}

/**
 *  Set the next argument as a string. For performance reasons, strings are not
 *  copied. Therefore, provided strings should remains valid at least until
 *  query execution.
 *
 *  \param[in] arg Next value.
 */
void MySQLHaveArgs::SetArg(const std::string& arg)
{
  if (this->stmt)
    {
      MYSQL_BIND* mybind;

      mybind = this->args_ + this->arg_;
      if (mybind->buffer_type != MYSQL_TYPE_STRING)
	{
	  this->CleanArg(mybind);
	  memset(mybind, 0, sizeof(*mybind));
	  mybind->buffer_type = MYSQL_TYPE_STRING;
	  mybind->length = &(mybind->buffer_length);
	}
      mybind->buffer_length = arg.size() + 1;
      mybind->buffer = const_cast<char*>(arg.c_str());
      this->arg_++;
    }
  else
    {
      this->query.append("\"");
      this->query.append(arg);
      this->query.append("\"");
    }
  return ;
}

/**
 *  Set the next argument as a time_t.
 *
 *  \param[in] arg Next value.
 */
void MySQLHaveArgs::SetArg(time_t arg)
{
  this->SetArg((int)arg);
  return ;
}
