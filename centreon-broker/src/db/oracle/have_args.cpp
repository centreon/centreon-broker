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
#include "db/oracle/have_args.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*         Protected Methods           *
*                                     *
**************************************/

/**
 *  \brief OracleHaveArgs constructor.
 *
 *  Initialize the new object.
 *
 *  \param[in] oconn The Oracle connection object.
 */
OracleHaveArgs::OracleHaveArgs(OCI_Connection* oconn)
  : OracleQuery(oconn),
    arg_(0),
    args_(NULL),
    args_count_(0) {}

/**
 *  \brief OracleHaveArgs copy constructor.
 *
 *  Initialize the new object from the given object.
 *
 *  \param[in] mha Object to copy data from.
 */
OracleHaveArgs::OracleHaveArgs(const OracleHaveArgs& oha)
  : HaveArgs(oha), Query(oha), OracleQuery(oha)
{
  // XXX : copy arguments
}

/**
 *  \brief OracleHaveArgs destructor.
 *
 *  Release all ressources previously acquired.
 */
OracleHaveArgs::~OracleHaveArgs()
{
  if (this->args_)
    {
      for (unsigned int i = 0; i < this->args_count_; i++)
	if (this->args_[i])
	  delete [] this->args_[i];
      delete [] this->args_;
    }
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all arguments of the given object to the current instance.
 *
 *  \param[in] oha Object to copy data from.
 *
 *  \return *this
 */
OracleHaveArgs& OracleHaveArgs::operator=(const OracleHaveArgs& oha)
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
 *  When the underlying query is executed, the job of OracleHaveArgs is to bind
 *  arguments to the statement (in the case of a prepared statement). Then it
 *  transfers control to the OracleQuery::Execute method.
 */
void OracleHaveArgs::Execute()
{
  this->OracleQuery::Execute();
  return ;
}

/**
 *  \brief Prepare the query.
 *
 *  When the underlying query is being prepared, the OracleHaveArgs object is
 *  responsible of allocating the data structures that will hold query
 *  arguments. For this purpose it will call the overriden GetArgCount()
 *  method. Control is then transfered to the OracleQuery::Prepare() method.
 */
void OracleHaveArgs::Prepare()
{
  this->args_count_ = this->GetArgCount();
  if (this->args_count_)
    {
      this->args_ = new void*[this->args_count_];
      memset(this->args_, 0, this->args_count_ * sizeof(*this->args_));
      this->arg_ = 0;
    }
  this->OracleQuery::Prepare();
  return ;
}

/**
 *  Set the next argument as a bool.
 *
 *  \param[in] arg Next value.
 */
void OracleHaveArgs::SetArg(bool arg)
{
  if (this->stmt)
    {
      if (!this->args_[this->arg_])
	this->args_[this->arg_] = new char[sizeof(int)];
      *static_cast<int*>(this->args_[this->arg_]) = (arg ? 1 : 0);
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
void OracleHaveArgs::SetArg(double arg)
{
  if (this->stmt)
    {
      if (!this->args_[this->arg_])
	this->args_[this->arg_] = new char[sizeof(double)];
      *static_cast<double*>(this->args_[this->arg_]) = arg;
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
void OracleHaveArgs::SetArg(int arg)
{
  if (this->stmt)
    {
      if (!this->args_[this->arg_])
	this->args_[this->arg_] = new char[sizeof(int)];
      *static_cast<int*>(this->args_[this->arg_]) = arg;
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
void OracleHaveArgs::SetArg(short arg)
{
  if (this->stmt)
    {
      if (!this->args_[this->arg_])
	this->args_[this->arg_] = new char[sizeof(short)];
      *static_cast<short*>(this->args_[this->arg_]) = arg;
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
 *  Set the next argument as a string.
 *
 *  \param[in] arg Next value.
 */
void OracleHaveArgs::SetArg(const std::string& arg)
{
  if (this->stmt)
    {
      if (this->args_[this->arg_])
	delete [] this->args_[this->arg_];
      this->args_[this->arg_] = NULL;
      this->args_[this->arg_] = new char[arg.size() + 1];
      strcpy(this->args_[this->arg_], arg.c_str());
      this->arg_++;
    }
  else
    {
      // XXX : unsafe str
      this->query.append(arg);
    }
  return ;
}

/**
 *  Set the next argument as a time_t.
 *
 *  \param[in] arg Next value.
 */
void OracleHaveArgs::SetArg(time_t arg)
{
  this->SetArg((int)arg);
  return ;
}
