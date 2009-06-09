/*
** have_fields.cpp for CentreonBroker in ./src/db/mysql
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/09/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
*/

#include <cassert>
#include <cstring>
#include "db/mysql/have_fields.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  MySQLHaveFields copy constructor.
 */
MySQLHaveFields::MySQLHaveFields(const MySQLHaveFields& myhf) throw ()
  : HaveFields()
{
  (void)myhf;
}

/**
 *  MySQLHaveFields operator= overload.
 */
MySQLHaveFields& MySQLHaveFields::operator=(const MySQLHaveFields& m) throw ()
{
  (void)m;
  return (*this);
}

/**
 *  Free ressources used by an argument.
 */
void MySQLHaveFields::DeleteArg(MYSQL_BIND* arg)
{
  switch (arg->buffer_type)
    {
     case MYSQL_TYPE_DOUBLE:
      delete (static_cast<double*>(arg->buffer));
      break ;
     case MYSQL_TYPE_LONG:
      delete (static_cast<int*>(arg->buffer));
      break ;
     case MYSQL_TYPE_SHORT:
      delete (static_cast<short*>(arg->buffer));
      break ;
     default:
      ; // Do nothing, just avoid compiler warning
    }
  return ;
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  MySQLHaveFields default constructor.
 */
MySQLHaveFields::MySQLHaveFields() throw () : arg_(0), myargs_(NULL), size_(0)
{
}

/**
 *  MySQLHaveFields destructor.
 */
MySQLHaveFields::~MySQLHaveFields()
{
  if (this->myargs_)
    {
      for (unsigned int i = 0; i < this->size_; i++)
	this->DeleteArg(this->myargs_ + i);
      delete [] this->myargs_;
    }
}

/**
 *  Prepare arguments.
 */
void MySQLHaveFields::Prepare(MYSQL_STMT* mystmt)
{
  assert(!this->myargs_);
  assert(mystmt);
  this->size_ = mysql_stmt_param_count(mystmt);
  this->myargs_ = new MYSQL_BIND[this->size_];
  memset(this->myargs_, 0, this->size_ * sizeof(*this->myargs_));
  return ;
}

/**
 *  Reset the argument counter.
 */
void MySQLHaveFields::Reset() throw ()
{
  this->arg_ = 0;
  return ;
}

/**
 *  Set a double argument.
 */
void MySQLHaveFields::SetDouble(double value)
{
  MYSQL_BIND* arg;

  arg = this->myargs_ + this->arg_;
  if (arg->buffer_type != MYSQL_TYPE_DOUBLE)
    {
      this->DeleteArg(arg);
      memset(arg, 0, sizeof(*arg));
      arg->buffer_type = MYSQL_TYPE_DOUBLE;
      arg->buffer = static_cast<void*>(new double);
      arg->buffer_length = sizeof(double);
    }
  *static_cast<double*>(arg->buffer) = value;
  this->arg_++;
  return ;
}

/**
 *  Set an int argument.
 */
void MySQLHaveFields::SetInt(int value)
{
  MYSQL_BIND* arg;

  arg = this->myargs_ + this->arg_;
  if (arg->buffer_type != MYSQL_TYPE_LONG)
    {
      this->DeleteArg(arg);
      memset(arg, 0, sizeof(*arg));
      arg->buffer_type = MYSQL_TYPE_LONG;
      arg->buffer = static_cast<void*>(new int);
      arg->buffer_length = sizeof(int);
    }
  *static_cast<int*>(arg->buffer) = value;
  this->arg_++;
  return ;
}

/**
 *  Set a short argument.
 */
void MySQLHaveFields::SetShort(short value)
{
  MYSQL_BIND* arg;

  arg = this->myargs_ + this->arg_;
  if (arg->buffer_type != MYSQL_TYPE_SHORT)
    {
      this->DeleteArg(arg);
      memset(arg, 0, sizeof(*arg));
      arg->buffer_type = MYSQL_TYPE_SHORT;
      arg->buffer = static_cast<void*>(new short);
      arg->buffer_length = sizeof(short);
    }
  *static_cast<short*>(arg->buffer) = value;
  this->arg_++;
  return ;
}

/**
 *  Set a string argument. The given string shall remain valid at least until
 *  the query execution.
 */
void MySQLHaveFields::SetString(const std::string& value)
{
  MYSQL_BIND* arg;

  arg = this->myargs_ + this->arg_;
  if (arg->buffer_type != MYSQL_TYPE_STRING)
    {
      this->DeleteArg(arg);
      memset(arg, 0, sizeof(*arg));
      arg->buffer_type = MYSQL_TYPE_STRING;
      arg->buffer = NULL;
      arg->buffer_length = 0;
    }
  arg->buffer = static_cast<void*>(const_cast<char*>(value.c_str()));
  arg->buffer_length = value.size();
  this->arg_++;
  return ;
}

/**
 *  Set a time argument (currently forward it to the SetInt method).
 */
void MySQLHaveFields::SetTime(time_t value)
{
  this->SetInt(value);
  return ;
}
