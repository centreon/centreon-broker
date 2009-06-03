/*
** connection.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/03/09 Matthieu Kermagoret
*/

#include "db/connection.h"

using namespace CentreonBroker;

/**
 *  DBConnection default constructor.
 */
DBConnection::DBConnection()
{
}

/**
 *  DBConnection copy constructor.
 */
DBConnection::DBConnection(const DBConnection& dbconn)
{
  (void)dbconn;
}

/**
 *  DBConnection destructor.
 */
DBConnection::~DBConnection()
{
}

/**
 *  DBConnection operator= overload.
 */
DBConnection& DBConnection::operator=(const DBConnection& dbconn)
{
  (void)dbconn;
  return (*this);
}
