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

#include "db/mysql/have_args.h"

using namespace CentreonBroker::DB;

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
MySQLHaveArgs::MySQLHaveArgs(MYSQL* mysql) : MySQLQuery(mysql) {}

/**
 *  \brief MySQLHaveArgs copy constructor.
 *
 *  Initialize the new object from the given object.
 *
 *  \param[in] mha Object to copy data from.
 */
MySQLHaveArgs::MySQLHaveArgs(const MySQLHaveArgs& mha)
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
  if (this->stmt && mysql_bind(this->stmt, this->args_))
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
    this->args_ = new MYSQL_BIND[this->args_count_];
  this->MySQLQuery::Prepare();
  return ;
}

/**
 *  \brief Set the next argument as a bool.
 */
void MySQLHaveArgs::SetArg(bool arg)
{
  if (this->stmt)
    {
      MYSQL_BIND* mybind;

      mybind = this->args_ + this->arg_;
      if (mybind->buffer_type != MYSQL_TYPE_TINYINT)
	{
	  this->CleanArg(mybind);
	  memset(mybind, 0, sizeof(*mybind));
	  mybind->buffer_type = MYSQL_TYPE_TINYINT;
	  mybind->buffer = static_cast<void*>(new bool);
	  mybind->buffer_length = sizeof(bool);
	  mybind->length = &(mybind->buffer_length);
	}
      *static_cast<bool*>(mybind->buffer) = arg;
    }
  else
    if (arg)
      this->query.append("true");
    else
      this->query.append("false");
  return ;
}
