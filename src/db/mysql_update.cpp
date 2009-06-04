/*
** mysql_update.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/04/09 Matthieu Kermagoret
*/

#include <cassert>
#include <cstring>
#include <mysql.h>
#include "db/mysql_update.h"
#include "exception.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  MySQLUpdate default constructor.
 */
MySQLUpdate::MySQLUpdate(MYSQL* myconn)
  : myconn_(myconn), myparams_(NULL), mystmt_(NULL)
{
}

/**
 *  Copy all internal data of the MySQLUpdate object to the current instance.
 */
void MySQLUpdate::InternalCopy(const MySQLUpdate& myupdate)
{
  // XXX : copy MySQL structures (or not)
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  MySQLUpdate copy constructor.
 */
MySQLUpdate::MySQLUpdate(const MySQLUpdate& myupdate) : UpdateQuery(myupdate)
{
  this->InternalCopy(myupdate);
}

/**
 *  MySQLUpdate destructor.
 */
MySQLUpdate::~MySQLUpdate()
{
  if (this->mystmt_)
    mysql_stmt_close(this->mystmt_);
  if (this->myparams_)
    {
      unsigned int param_count;

      param_count = mysql_stmt_param_count(this->mystmt_);
      for (unsigned int i = 0; i < param_count; i++)
        switch (this->myparams_[i].buffer_type)
          {
           case MYSQL_TYPE_DOUBLE:
            delete static_cast<double*>(this->myparams_[i].buffer);
            break ;
           case MYSQL_TYPE_LONG:
            delete static_cast<int*>(this->myparams_[i].buffer);
            break ;
           case MYSQL_TYPE_SHORT:
            delete static_cast<short*>(this->myparams_[i].buffer);
            break ;
           default:
	     ; // Do nothing, just avoid compiler warnings
          }
      delete [] this->myparams_;
    }
}

/**
 *  MySQLUpdate operator= overload.
 */
MySQLUpdate& MySQLUpdate::operator=(const MySQLUpdate& myupdate)
{
  this->UpdateQuery::operator=(myupdate);
  this->InternalCopy(myupdate);
  return (*this);
}

/**
 *  Execute the query.
 */
void MySQLUpdate::Execute()
{
  assert(this->mystmt_);
  assert(this->myparams_);
  if (mysql_stmt_bind_param(this->mystmt_, this->myparams_))
    throw (Exception(mysql_errno(this->myconn_),
                     mysql_error(this->myconn_)));
  if (mysql_stmt_execute(this->mystmt_))
    throw (Exception(mysql_errno(this->myconn_),
                     mysql_error(this->myconn_)));
  if (mysql_stmt_affected_rows(this->mystmt_) == 0)
    throw (Exception(mysql_errno(this->myconn_),
                     mysql_error(this->myconn_)));
  return ;
}

/**
 *  Prepare the query.
 */
void MySQLUpdate::Prepare()
{
  std::string query;

  query = "UPDATE ";
  query += this->table_;
  query += " SET ";
  for (decltype(this->fields_.begin()) it = this->fields_.begin();
       it != this->fields_.end();
       it++)
    {
      query += *it;
      query += "=?, ";
    }
  query.resize(query.size() - 2);
  query += " WHERE ";
  for (decltype(this->uniques_.begin()) it = this->uniques_.begin();
       it != this->uniques_.end();
       it++)
    {
      query += *it;
      query += "=? AND ";
    }
  query.resize(query.size() - 5);
  this->mystmt_ = mysql_stmt_init(this->myconn_);
  if (!this->mystmt_)
    throw (Exception(mysql_errno(this->myconn_),
                     mysql_error(this->myconn_)));
  if (mysql_stmt_prepare(this->mystmt_, query.c_str(), query.size()))
    {
      mysql_stmt_close(this->mystmt_);
      this->mystmt_ = NULL;
      throw (Exception(mysql_errno(this->myconn_),
		       mysql_error(this->myconn_)));
    }
  {
    unsigned int param_count;

    param_count = mysql_stmt_param_count(this->mystmt_);
    this->myparams_ = new MYSQL_BIND[param_count];
    memset(this->myparams_, 0, param_count * sizeof(*this->myparams_));
    this->fields_.clear();
    this->uniques_.clear();
  }
  return ;
}

/**
 *  Set the argument to the double type.
 */
void MySQLUpdate::SetDouble(int arg, double value)
{
  MYSQL_BIND* param;

  assert(this->mystmt_);
  assert(this->myparams_);
  param = this->myparams_ + arg;
  if (!param->buffer)
    {
      param->buffer_type = MYSQL_TYPE_DOUBLE;
      param->buffer = static_cast<void*>(new double);
      param->buffer_length = sizeof(double);
      param->length = NULL;
      param->is_null = NULL;
      param->is_unsigned = false;
      param->error = NULL;
    }
  assert(MYSQL_TYPE_DOUBLE == param->buffer_type);
  *static_cast<double*>(this->myparams_[arg].buffer) = value;
  return ;
}

/**
 *  Set the argument to the int type.
 */
void MySQLUpdate::SetInt(int arg, int value)
{
  MYSQL_BIND* param;

  assert(this->mystmt_);
  assert(this->myparams_);
  param = this->myparams_ + arg;
  if (!param->buffer)
    {
      param->buffer_type = MYSQL_TYPE_LONG;
      param->buffer = static_cast<void*>(new int);
      param->buffer_length = sizeof(int);
      param->length = NULL;
      param->is_null = NULL;
      param->is_unsigned = false;
      param->error = NULL;
    }
  assert(MYSQL_TYPE_LONG == param->buffer_type);
  *static_cast<int*>(this->myparams_[arg].buffer) = value;
  return ;
}

/**
 *  Set the argument to the short type.
 */
void MySQLUpdate::SetShort(int arg, short value)
{
  MYSQL_BIND* param;

  assert(this->mystmt_);
  assert(this->myparams_);
  param = this->myparams_ + arg;
  if (!param->buffer)
    {
      param->buffer_type = MYSQL_TYPE_SHORT;
      param->buffer = static_cast<void*>(new short);
      param->buffer_length = sizeof(short);
      param->length = NULL;
      param->is_null = NULL;
      param->is_unsigned = false;
      param->error = NULL;
    }
  assert(MYSQL_TYPE_SHORT == param->buffer_type);
  *static_cast<short*>(this->myparams_[arg].buffer) = value;
  return ;
}

/**
 *  Set the argument to the string type.
 */
void MySQLUpdate::SetString(int arg, const char* value)
{
  MYSQL_BIND* param;

  assert(this->mystmt_);
  assert(this->myparams_);
  param = this->myparams_ + arg;
  if (!param->buffer)
    {
      param->buffer_type = MYSQL_TYPE_STRING;
      param->buffer = NULL;
      param->buffer_length = 0;
      param->length = &param->buffer_length;
      param->is_null = NULL;
      param->is_unsigned = false;
      param->error = NULL;
    }
  assert(MYSQL_TYPE_STRING == param->buffer_type);
  this->myparams_[arg].buffer = static_cast<void*>(const_cast<char*>(value));
  this->myparams_[arg].buffer_length = strlen(value);
  return ;
}

/**
 *  Set the argument to the time_t type.
 */
void MySQLUpdate::SetTimeT(int arg, time_t value)
{
  this->SetInt(arg, value);
  return ;
}
