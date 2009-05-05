/*
** mysqloutput.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/05/09 Matthieu Kermagoret
*/

#include <mysql.h>
#include "mysqloutput.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  MySQLOutput copy constructor.
 */
MySQLOutput::MySQLOutput(const MySQLOutput& mysqlo) : Thread()
{
  (void)mysqlo;
}

/**
 *  MySQLOutput operator= overload.
 */
MySQLOutput&		MySQLOutput::operator=(const MySQLOutput& mysqlo)
{
  (void)mysqlo;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  MySQLOutput constructor.
 */
MySQLOutput::MySQLOutput() : Thread()
{
}

/**
 *  MySQLOutput destructor.
 */
MySQLOutput::~MySQLOutput()
{
}

/**
 *  Set MySQL connection informations but does not really connect to the
 *  server.
 */
void MySQLOutput::Connect(const std::string& host,
                          const std::string& user,
                          const std::string& password,
                          const std::string& db)
{
  this->host = host;
  this->user = user;
  this->password = password;
  this->db = db;
  return ;
}

/**
 *  This function waits for queries to be executed on the server on its own
 *  thread.
 */
int MySQLOutput::Core()
{
  MYSQL mysql;

  // XXX : handle all MySQL errors
  mysql_init(&mysql);
  mysql_real_connect(&mysql,
                     this->host.c_str(),
		     this->user.c_str(),
		     this->password.c_str(),
		     this->db.c_str(),
		     0, // port
		     NULL, // unix socket
		     0); // client flag
  // XXX : prepare queries
  this->queries_mutex.Lock();
  while (1)
    {
      if (this->queries.empty())
	; // XXX : Condition Wait
    }
  return (0);
}
