/*
** mysql_update.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/03/09 Matthieu Kermagoret
*/

#include <cassert>
// XXX
#include <mysql_connection.h>
#include <mysql_prepared_statement.h>
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
MySQLUpdate::MySQLUpdate(sql::Connection* myconn)
  : myconn_(myconn), mystmt_(NULL)
{
}

/**
 *  Copy all internal data of the MySQLUpdate object to the current instance.
 */
void MySQLUpdate::InternalCopy(const MySQLUpdate& myupdate)
{
  this->myconn_ = myupdate.myconn_;
  if (this->mystmt_)
    delete (this->mystmt_);
  // XXX : fix this with shared_ptr
  this->mystmt_ = myupdate.mystmt_;
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
    delete (this->mystmt_);
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
  this->mystmt_->execute();
  if (this->mystmt_->getUpdateCount() == 0)
    throw (Exception(0, "No UPDATE occured"));
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
  this->mystmt_ = this->myconn_->prepareStatement(query);
  this->fields_.clear();
  this->uniques_.clear();
  return ;
}

/**
 *  Set the argument to the double type.
 */
void MySQLUpdate::SetDouble(int arg, double value)
{
  assert(this->mystmt_);
  this->mystmt_->setDouble(arg, value);
  return ;
}

/**
 *  Set the argument to the int type.
 */
void MySQLUpdate::SetInt(int arg, int value)
{
  assert(this->mystmt_);
  this->mystmt_->setInt(arg, value);
  return ;
}

/**
 *  Set the argument to the short type.
 */
void MySQLUpdate::SetShort(int arg, short value)
{
  assert(this->mystmt_);
  // XXX : setShort
  this->mystmt_->setInt(arg, value);
  return ;
}

/**
 *  Set the argument to the string type.
 */
void MySQLUpdate::SetString(int arg, const std::string& value)
{
  assert(this->mystmt_);
  this->mystmt_->setString(arg, value);
  return ;
}

/**
 *  Set the argument to the time_t type.
 */
void MySQLUpdate::SetTimeT(int arg, time_t value)
{
  assert(this->mystmt_);
  // XXX : something date-related
  this->mystmt_->setInt(arg, value);
  return ;
}
