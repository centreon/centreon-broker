/*
** connection.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
*/

#include <cassert>
#include "db/connection.h"

using namespace CentreonBroker::DB;

/**
 *  Connection default constructor.
 */
Connection::Connection(Connection::DBMS dbms) : dbms_(dbms)
{
}

/**
 *  Connection copy constructor.
 */
Connection::Connection(const Connection& conn)
{
  (void)conn;
  assert(this->dbms_ == conn.dbms_);
}

/**
 *  Connection destructor.
 */
Connection::~Connection()
{
}

/**
 *  Connection operator= overload.
 */
Connection& Connection::operator=(const Connection& conn)
{
  (void)conn;
  assert(this->dbms_ == conn.dbms_);
  return (*this);
}

/**
 *  Returns the type of the current DBMS.
 */
Connection::DBMS Connection::GetDbms() const throw ()
{
  return (this->dbms_);
}
