/*
** mysql_insert.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/02/09 Matthieu Kermagoret
*/

#include <cassert>
// XXX
#include <mysql_connection.h>
#include <mysql_prepared_statement.h>
#include "db/mysql_insert.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  MySQLInsert default constructor.
 */
MySQLInsert::MySQLInsert(sql::Connection* myconn)
  : myconn_(myconn), mystmt_(NULL)
{
}

/**
 *  Copy all internal data of the given MySQLInsert structure tp the current
 *  instance.
 */
void MySQLInsert::InternalCopy(const MySQLInsert& myinsert)
{
  this->myconn_ = myinsert.myconn_;
  if (this->mystmt_)
    delete (this->mystmt_);
  // XXX : fix this problem with shared_ptr
  this->mystmt_ = myinsert.mystmt_;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  MySQLInsert copy constructor.
 */
MySQLInsert::MySQLInsert(const MySQLInsert& myinsert) : Query(myinsert)
{
  this->InternalCopy(myinsert);
}

/**
 *  MySQLInsert destructor.
 */
MySQLInsert::~MySQLInsert()
{
  if (this->mystmt_)
    delete (this->mystmt_);
}

/**
 *  MySQLInsert operator= overload.
 */
MySQLInsert& MySQLInsert::operator=(const MySQLInsert& myinsert)
{
  this->Query::operator=(myinsert);
  this->InternalCopy(myinsert);
  return (*this);
}

/**
 *  Execute the query.
 */
void MySQLInsert::Execute()
{
  this->mystmt_->execute();
  return ;
}

/**
 *  Prepare the query.
 */
void MySQLInsert::Prepare()
{
  std::string query;

  query = "INSERT INTO ";
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
  this->mystmt_ = this->myconn_->prepareStatement(query);
  this->fields_.clear();
  return ;
}

/**
 *  Set the argument to be of the double type.
 */
void MySQLInsert::SetDouble(int arg, double value)
{
  assert(this->mystmt_);
  this->mystmt_->setDouble(arg, value);
  return ;
}

/**
 *  Set the argument to be of the int type.
 */
void MySQLInsert::SetInt(int arg, int value)
{
  assert(this->mystmt_);
  this->mystmt_->setInt(arg, value);
  return ;
}

/**
 *  Set the argument to be of the short type.
 */
void MySQLInsert::SetShort(int arg, short value)
{
  assert(this->mystmt_);
  // XXX : setShort
  this->mystmt_->setInt(arg, value);
  return ;
}

/**
 *  Set the argument to be of the string type.
 */
void MySQLInsert::SetString(int arg, const std::string& value)
{
  assert(this->mystmt_);
  this->mystmt_->setString(arg, value);
  return ;
}

/**
 *  Set the argument to be of the time_t type.
 */
void MySQLInsert::SetTimeT(int arg, time_t value)
{
  assert(this->mystmt_);
  this->mystmt_->setInt(arg, value);
  return ;
}
