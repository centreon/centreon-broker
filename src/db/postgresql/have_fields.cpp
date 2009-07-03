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
#include "db/db_exception.h"
#include "db/postgresql/have_fields.h"
#include "logging.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  PgSQLHaveFields copy constructor.
 */
PgSQLHaveFields::PgSQLHaveFields(const PgSQLHaveFields& pghf) throw ()
  : HaveFields(), Query(), PgSQLQuery(NULL)
{
  (void)pghf;
}

/**
 *  PgSQLHaveFields operator= overload.
 */
PgSQLHaveFields& PgSQLHaveFields::operator=(const PgSQLHaveFields& p) throw ()
{
  (void)p;
  return (*this);
}

/**
 *  Free ressources used by an argument.
 */
void PgSQLHaveFields::DeleteArg(int idx)
{
  assert(this->param_format_);
  assert(this->param_values_);
  // XXX : the deletion might cause some troubles because the proper delete
  //       overload might not be chosen
  if (this->param_format_[idx] && this->param_values_[idx])
    {
      this->param_format_[idx] = 0;
      delete (this->param_values_[idx]);
      this->param_values_[idx] = NULL;
    }
  return ;
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  PgSQLHaveFields default constructor.
 */
PgSQLHaveFields::PgSQLHaveFields(PGconn* pgconn) throw ()
  : PgSQLQuery(pgconn), arg_(0)
{
}

/**
 *  PgSQLHaveFields destructor.
 */
PgSQLHaveFields::~PgSQLHaveFields()
{
  if (this->param_values_)
    {
      if (this->param_format_)
	for (unsigned int i = 0; i < this->nparams_; i++)
	  this->DeleteArg(i);
      delete [] this->param_values_;
      this->param_values_ = NULL;
    }
  if (this->param_format_)
    {
      delete [] (this->param_format_);
      this->param_format_ = NULL;
    }
  if (this->param_length_)
    {
      delete [] (this->param_length_);
      this->param_length_ = NULL;
    }
}

/**
 *  Allocate memory for the query arguments.
 */
void PgSQLHaveFields::Prepare()
{
#ifndef NDEBUG
  logging.LogDebug("Preparing bound argument structures...");
#endif /* !NDEBUG */
  assert(!this->param_format_);
  assert(!this->param_length_);
  assert(!this->param_values_);
  assert(this->pgconn_);
  assert(!this->stmt_name_.empty());

  // Fetch the statement's argument count
  {
    PGresult* res;

    res = PQdescribePrepared(this->pgconn_, this->stmt_name_.c_str());
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
	this->nparams_ = PQnparams(res);
	PQclear(res);
      }
  }
  this->param_format_ = new int[this->nparams_];
  memset(this->param_format_,
         0,
         this->nparams_ * sizeof(*this->param_format_));
  this->param_length_ = new int[this->nparams_];
  memset(this->param_length_,
         0,
         this->nparams_ * sizeof(*this->param_length_));
  this->param_values_ = new char*[this->nparams_];
  memset(this->param_values_,
         0,
         this->nparams_ * sizeof(*this->param_values_));
  return ;
}

/**
 *  Reset the argument counter.
 */
void PgSQLHaveFields::Reset() throw ()
{
  this->arg_ = 0;
  return ;
}

/**
 *  Set a bool argument.
 */
void PgSQLHaveFields::SetBool(bool value)
{
  assert(this->param_format_);
  assert(this->param_length_);
  assert(this->param_values_);
  if (!this->param_values_[this->arg_])
    {
      this->DeleteArg(this->arg_);
      this->param_format_[this->arg_] = 1;
      this->param_length_[this->arg_] = sizeof(bool);
      this->param_values_[this->arg_] = (char*)(new bool);
    }
  *(bool*)(this->param_values_[this->arg_]) = value;
  this->arg_++;
  return ;
}

/**
 *  Set a double argument.
 */
void PgSQLHaveFields::SetDouble(double value)
{
  assert(this->param_format_);
  assert(this->param_length_);
  assert(this->param_values_);
  if (!this->param_values_[this->arg_])
    {
      this->DeleteArg(this->arg_);
      this->param_format_[this->arg_] = 1;
      this->param_length_[this->arg_] = sizeof(double);
      this->param_values_[this->arg_] = (char*)(new double);
    }
  *(double*)(this->param_values_[this->arg_]) = value;
  this->arg_++;
  return ;
}

/**
 *  Set an int argument.
 */
void PgSQLHaveFields::SetInt(int value)
{
  assert(this->param_format_);
  assert(this->param_length_);
  assert(this->param_values_);
  if (!this->param_values_[this->arg_])
    {
      this->DeleteArg(this->arg_);
      this->param_format_[this->arg_] = 1;
      this->param_length_[this->arg_] = sizeof(int);
      this->param_values_[this->arg_] = (char*)(new int);
    }
  *(int*)(this->param_values_[this->arg_]) = value;
  this->arg_++;
  return ;
}

/**
 *  Set a short argument.
 */
void PgSQLHaveFields::SetShort(short value)
{
  assert(this->param_format_);
  assert(this->param_length_);
  assert(this->param_values_);
  if (!this->param_values_[this->arg_])
    {
      this->DeleteArg(this->arg_);
      this->param_format_[this->arg_] = 1;
      this->param_length_[this->arg_] = sizeof(short);
      this->param_values_[this->arg_] = (char*)(new short);
    }
  *(short*)(this->param_values_[this->arg_]) = value;
  this->arg_++;
  return ;
}

/**
 *  Set a string argument. The given string shall remain valid at least until
 *  the query execution.
 */
void PgSQLHaveFields::SetString(const std::string& value)
{
  assert(this->param_format_);
  assert(this->param_length_);
  assert(this->param_values_);
  if (!this->param_values_[this->arg_])
    {
      this->DeleteArg(this->arg_);
      this->param_format_[this->arg_] = 0;
      this->param_length_[this->arg_] = 0;
      this->param_values_[this->arg_] = NULL;
    }
  // We do not need to set the length here cause it's ignored.
  this->param_values_[this->arg_] = const_cast<char*>(value.c_str());
  this->arg_++;
  return ;
}

/**
 *  Set a time argument (currently forward it to the SetInt method).
 */
void PgSQLHaveFields::SetTime(time_t value)
{
  this->SetInt(value);
  return ;
}
