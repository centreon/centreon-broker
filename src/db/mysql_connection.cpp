/*
** mysql_connection.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/03/09 Matthieu Kermagoret
*/

#include <cassert>
// XXX
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <mysql_statement.h>
#include "db/mysql_connection.h"
#include "db/mysql_insert.h"
#include "db/mysql_update.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Copy all internal data of the MySQLConnection object to the current
 *  instance.
 */
void MySQLConnection::InternalCopy(const MySQLConnection& myconn)
{
  if (this->myconn_)
    delete (this->myconn_);
  // XXX : fix this with shared_ptr
  this->myconn_ = myconn.myconn_;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  MySQLConnection default constructor.
 */
MySQLConnection::MySQLConnection() : myconn_(NULL)
{
}

/**
 *  MySQLConnection copy constructor.
 */
MySQLConnection::MySQLConnection(const MySQLConnection& myconn)
  : DBConnection(myconn)
{
  this->InternalCopy(myconn);
}

/**
 *  MySQLConnection destructor.
 */
MySQLConnection::~MySQLConnection()
{
  if (this->myconn_)
    delete (this->myconn_);
}

/**
 *  MySQLConnection operator= overload.
 */
MySQLConnection& MySQLConnection::operator=(const MySQLConnection& myconn)
{
  this->DBConnection::operator=(myconn);
  this->InternalCopy(myconn);
  return (*this);
}

/**
 *  Commit the current transaction.
 */
void MySQLConnection::Commit()
{
  this->myconn_->commit();
  return ;
}

/**
 *  Initialize the MySQL connection.
 */
void MySQLConnection::Connect(const std::string& host,
                              const std::string& user,
                              const std::string& password,
                              const std::string& db)
{
  sql::Driver* driver;

  driver = get_driver_instance();
  this->myconn_ = driver->connect(host, user, password);
  // XXX : auto_ptr deprecated
  {
    std::auto_ptr<sql::Statement> stmt(this->myconn_->createStatement());

    stmt->execute(std::string("USE ") + db + std::string(";"));
  }
  this->myconn_->setAutoCommit(false);
  return ;
}

/**
 *  Disconnect from the MySQL server.
 */
void MySQLConnection::Disconnect()
{
  assert(this->myconn_);
  delete (this->myconn_);
  this->myconn_ = NULL;
  return ;
}

/**
 *  Get a MySQL insert query object.
 */
Query* MySQLConnection::GetInsertQuery()
{
  assert(this->myconn_);
  return (new MySQLInsert(this->myconn_));
}

/**
 *  Get a MySQL update query object.
 */
UpdateQuery* MySQLConnection::GetUpdateQuery()
{
  assert(this->myconn_);
  return (new MySQLUpdate(this->myconn_));
}
