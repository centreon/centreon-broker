/*
** mysql_connection.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/05/09 Matthieu Kermagoret
*/

#include <mysql.h>
#include "db/mysql_connection.h"
#include "db/mysql_insert.h"
#include "db/mysql_truncate.h"
#include "db/mysql_update.h"
#include "exception.h"

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
  // XXX : find a way to copy MYSQL
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
MySQLConnection::MySQLConnection()
{
  if (mysql_thread_init())
    throw (Exception(mysql_errno(&this->myconn_),
                     mysql_error(&this->myconn_)));
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
  mysql_thread_end();
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
  // XXX : error handling
  mysql_query(&this->myconn_, "COMMIT;");
  mysql_query(&this->myconn_, "START TRANSACTION;");
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
  if (!mysql_init(&this->myconn_))
    throw (Exception(mysql_errno(&this->myconn_),
                     mysql_error(&this->myconn_)));
  if (!mysql_real_connect(&this->myconn_,
                          host.c_str(),
                          user.c_str(),
                          password.c_str(),
                          db.c_str(),
                          0,
                          NULL,
                          CLIENT_FOUND_ROWS)
      || mysql_query(&this->myconn_, "START TRANSACTION;"))
    {
      mysql_close(&this->myconn_);
      throw (Exception(mysql_errno(&this->myconn_),
                       mysql_error(&this->myconn_)));
    }
  return ;
}

/**
 *  Disconnect from the MySQL server.
 */
void MySQLConnection::Disconnect()
{
  mysql_close(&this->myconn_);
  return ;
}

/**
 *  Get a MySQL insert query object.
 */
Query* MySQLConnection::GetInsertQuery()
{
  return (new MySQLInsert(&this->myconn_));
}

/**
 *  Get a MySQL truncate query object.
 */
TruncateQuery* MySQLConnection::GetTruncateQuery()
{
  return (new MySQLTruncate(&this->myconn_));
}

/**
 *  Get a MySQL update query object.
 */
UpdateQuery* MySQLConnection::GetUpdateQuery()
{
  return (new MySQLUpdate(&this->myconn_));
}
