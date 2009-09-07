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
#include <cstring>
#include <sstream>
#include "db/db_exception.h"
#include "db/postgresql/have_args.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Clean ressources related to a specific argument.
 *
 *  An argument might have allocated ressources. Release those ressources.
 *
 *  \param[in] idx Index of the argument to clean.
 */
void PgSQLHaveArgs::CleanArg(int idx)
{
  assert(this->args_format_);
  assert(this->args_values_);
  // XXX : the deletion here might cause some troubles because the proper
  //       delete overload might not be chosen
  if (this->args_format_[idx] && this->args_values_[idx])
    delete (this->args_values_[idx]);
  this->args_format_[idx] = 0;
  this->args_values_[idx] = NULL;
  return ;
}

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief PgSQLHaveArgs constructor.
 *
 *  Initialize the new object.
 *
 *  \param[in] pgconn The PostgreSQL connection object.
 */
PgSQLHaveArgs::PgSQLHaveArgs(PGconn* pgconn)
  : PgSQLQuery(pgconn),
    arg_(0),
    args_count_(0),
    args_format_(NULL),
    args_length_(NULL),
    args_values_(NULL) {}

/**
 *  \brief PgSQLHaveArgs copy constructor.
 *
 *  Initialize the new object from the given object.
 *
 *  \param[in] pha Object to copy data from.
 */
PgSQLHaveArgs::PgSQLHaveArgs(const PgSQLHaveArgs& pha)
  : HaveArgs(pha), Query(pha), PgSQLQuery(NULL)
{
  // XXX : copy arguments
}

/**
 *  \brief PgSQLHaveArgs destructor.
 *
 *  Release all ressources previously acquired.
 */
PgSQLHaveArgs::~PgSQLHaveArgs()
{
  if (this->args_values_)
    {
      if (this->args_format_)
	for (unsigned int i = 0; i < this->args_count_; i++)
	  this->CleanArg(i);
      delete [] this->args_values_;
    }
  if (this->args_format_)
    delete [] (this->args_format_);
  if (this->args_length_)
    delete [] (this->args_length_);
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all arguments of the given object to the current instance.
 *
 *  \param[in] pha Object to copy data from.
 *
 *  \return *this
 */
PgSQLHaveArgs& PgSQLHaveArgs::operator=(const PgSQLHaveArgs& pha)
{
  // XXX : copy arguments
  return (*this);
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  \brief Execute the query.
 *
 *  When the underlying query is executed, the job of PgSQLHaveArgs is to
 *  provides the arguments to PgSQLQuery (in the case of a prepared statement).
 */
void PgSQLHaveArgs::Execute()
{
  if (!this->stmt_name.empty() && this->args_count_ > 0)
    {
      assert(this->args_values_);
      assert(this->args_length_);
      assert(this->args_format_);
      this->PgSQLQuery::Execute(this->args_count_,
                                this->args_values_,
                                this->args_length_,
                                this->args_format_);
    }
  else
    this->PgSQLQuery::Execute();
  this->arg_ = 0;
  return ;
}

/**
 *  \brief Prepare the query.
 *
 *  When the underlying query is being prepared, the PgSQLHaveArgs object is
 *  responsible of allocating necessary memory to store query arguments. For
 *  this purpose it will call the overriden GetArgCount() method. Control is
 *  then transfered to the PgSQLQuery::Prepare() method.
 */
void PgSQLHaveArgs::Prepare()
{
  assert(!this->args_format_);
  assert(!this->args_length_);
  assert(!this->args_values_);

  // Prepare the statement object
  this->PgSQLQuery::Prepare();
  assert(this->pgconn_);
  assert(!this->stmt_name.empty());

  // Fetch the statement's argument count
  {
    PGresult* res;

    res = PQdescribePrepared(this->pgconn_, this->stmt_name.c_str());
    if (!res)
      throw (DBException(0,
			 DBException::QUERY_PREPARATION,
			 "Could not allocate memory."));
    else if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
	DBException dbe(0,
                        DBException::QUERY_PREPARATION,
                        PQresultErrorMessage(res));
	PQclear(res);
	throw (dbe);
      }
    else
      {
	this->args_count_ = PQnparams(res);
	PQclear(res);
      }
  }

  // Allocate memory for arguments structures
  this->args_format_ = new int[this->args_count_];
  memset(this->args_format_,
         0,
         this->args_count_ * sizeof(*this->args_format_));
  this->args_length_ = new int[this->args_count_];
  memset(this->args_length_,
         0,
         this->args_count_ * sizeof(*this->args_length_));
  this->args_values_ = new char*[this->args_count_];
  memset(this->args_values_,
         0,
         this->args_count_ * sizeof(*this->args_values_));
  return ;
}

/**
 *  Set the next argument as a bool.
 *
 *  \param[in] arg Next value.
 */
void PgSQLHaveArgs::SetArg(bool arg)
{
  if (!this->stmt_name.empty())
    {
      assert(this->args_format_);
      assert(this->args_length_);
      assert(this->args_values_);
      if (!this->args_format_[this->arg_]
          || (this->args_length_[this->arg_] != sizeof(bool)))
        {
          this->CleanArg(this->arg_);
          this->args_format_[this->arg_] = 1;
          this->args_length_[this->arg_] = sizeof(bool);
          this->args_values_[this->arg_] = (char*)(new bool);
        }
      *(bool*)(this->args_values_[this->arg_++]) = arg;
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
void PgSQLHaveArgs::SetArg(double arg)
{
  if (!this->stmt_name.empty())
    {
      assert(this->args_format_);
      assert(this->args_length_);
      assert(this->args_values_);
      if (!this->args_format_[this->arg_]
          || (this->args_length_[this->arg_] != sizeof(double)))
        {
          this->CleanArg(this->arg_);
          this->args_format_[this->arg_] = 1;
          this->args_length_[this->arg_] = sizeof(double);
          this->args_values_[this->arg_] = (char*)(new double);
        }
      *(double*)(this->args_values_[this->arg_++]) = arg;
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
void PgSQLHaveArgs::SetArg(int arg)
{
  if (!this->stmt_name.empty())
    {
      assert(this->args_format_);
      assert(this->args_length_);
      assert(this->args_values_);
      if (!this->args_format_[this->arg_]
          || (this->args_length_[this->arg_] != sizeof(int)))
        {
          this->CleanArg(this->arg_);
          this->args_format_[this->arg_] = 1;
          this->args_length_[this->arg_] = sizeof(int);
          this->args_values_[this->arg_] = (char*)(new int);
        }
      *(int*)(this->args_values_[this->arg_++]) = arg;
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
void PgSQLHaveArgs::SetArg(short arg)
{
  if (!this->stmt_name.empty())
    {
      assert(this->args_format_);
      assert(this->args_length_);
      assert(this->args_values_);
      if (!this->args_format_[this->arg_]
          || (this->args_length_[this->arg_] != sizeof(short)))
        {
          this->CleanArg(this->arg_);
          this->args_format_[this->arg_] = 1;
          this->args_length_[this->arg_] = sizeof(short);
          this->args_values_[this->arg_] = (char*)(new short);
        }
      *(short*)(this->args_values_[this->arg_++]) = arg;
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
 *  Set the next argument as a string.
 *
 *  \param[in] arg Next value.
 */
void PgSQLHaveArgs::SetArg(const std::string& arg)
{
  if (!this->stmt_name.empty())
    {
      assert(this->args_format_);
      assert(this->args_length_);
      assert(this->args_values_);
      if (this->args_format_[this->arg_])
	{
	  this->CleanArg(this->arg_);
	  this->args_format_[this->arg_] = 0;
	}
      this->args_length_[this->arg_] = arg.size();
      this->args_values_[this->arg_++] = const_cast<char*>(arg.c_str());
    }
  else
    {
      char* safe_str;

      // XXX : potential leak
      safe_str = new char[arg.size() * 2 + 1];
      PQescapeStringConn(this->pgconn_,
                         safe_str,
                         arg.c_str(),
                         arg.size(),
                         NULL);
      this->query.append("'");
      this->query.append(safe_str);
      delete [] safe_str;
      this->query.append("'");
    }
  return ;
}

/**
 *  Set the next argument as a time_t.
 *
 *  \param[in] arg Next value.
 */
void PgSQLHaveArgs::SetArg(time_t arg)
{
  this->SetArg((int)arg);
  return ;
}
