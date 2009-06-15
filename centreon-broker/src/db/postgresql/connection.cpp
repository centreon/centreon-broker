/*
** connection.cpp for CentreonBroker in ./src/db/postgresql
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/12/09 Matthieu Kermagoret
** Last update 06/12/09 Matthieu Kermagoret
*/

#include <cassert>
#include "db/db_exception.h"
#include "db/postgresql/connection.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  PgSQLConnection copy constructor.
 */
PgSQLConnection::PgSQLConnection(const PgSQLConnection& pgconn)
  : Connection(Connection::POSTGRESQL)
{
  (void)pgconn;
  assert(false);
  return ;
}

/**
 *  PgSQL operator= overload.
 */
PgSQLConnection& PgSQLConnection::operator=(const PgSQLConnection& pgconn)
{
  (void)pgconn;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  PgSQLConnection default constructor.
 */
PgSQLConnection::PgSQLConnection() : Connection(Connection::POSTGRESQL)
{
}

/**
 *  PgSQLConnection destructor.
 */
PgSQLConnection::~PgSQLConnection()
{

}

/**
 *  Toggle the auto-commit mode of the connection.
 */
void PgSQLConnection::AutoCommit(bool activate)
{
}

/**
 *  Commit data of the current transaction.
 */
void PgSQLConnection::Commit()
{
}

/**
 *  Connect to the specified PostgreSQL server.
 */
void PgSQLConnection::Connect(const std::string& host,
			      const std::string& user,
			      const std::string& password,
			      const std::string& db)
{
  std::string conninfo;

  this->pgconn_ = PQconnectdb(conninfo.c_str());
  if (!this->pgconn_ || PQstatus(this->pgconn_) != CONNECTION_OK)
    {
      if (this->pgconn_)
	this->Disconnect();
      throw (DBException(0,
			 DB::DBException::INITIALIZATION,
			 "Connection to PostgreSQL server failed"));
    }
  return ;
}

/**
 *  Disconnect from the PostgreSQL server.
 */
void PgSQLConnection::Disconnect()
{
  assert(this->pgconn_);
  PQfinish(this->pgconn_);
  this->pgconn_ = NULL;
  return ;
}
