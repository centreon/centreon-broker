/*
** connection.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/10/09 Matthieu Kermagoret
*/

#include <cassert>
#include "db/connection.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Connection copy constructor.
 */
Connection::Connection(const Connection& conn) throw () : dbms_(UNKNOWN)
{
  (void)conn;
  assert(false);
}

/**
 *  Connection operator= overload.
 */
Connection& Connection::operator=(const Connection& conn) throw ()
{
  (void)conn;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Connection default constructor.
 */
Connection::Connection(Connection::DBMS dbms) throw () : dbms_(dbms)
{
}

/**
 *  Connection destructor.
 */
Connection::~Connection()
{
}

/**
 *  Returns the type of the current DBMS.
 */
Connection::DBMS Connection::GetDbms() const throw ()
{
  return (this->dbms_);
}
