/*
** mysqloutput.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/06/09 Matthieu Kermagoret
*/

#include <cassert>
#include <cstring>
#include <mysql.h>
#include "event.h"
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
MySQLOutput::MySQLOutput(const MySQLOutput& mysqlo)
  : EventSubscriber(), Thread()
{
  (void)mysqlo;
}

/**
 *  MySQLOutput operator= overload.
 */
MySQLOutput& MySQLOutput::operator=(const MySQLOutput& mysqlo)
{
  (void)mysqlo;
  return (*this);
}

/**
 *  Prepare the queries for execution on the MySQL server.
 */
void MySQLOutput::PrepareQueries(MYSQL* mysql)
{
  const char* queries[] =
    {
      "INSERT INTO test VALUES(?, ?, ?, ?)",
      "INSERT INTO bidule VALUES(?, ?, ?, ?, ?",
      NULL
    };
  enum_field_types args[][42] =
    {
      {
	MYSQL_TYPE_SHORT,
	MYSQL_TYPE_SHORT,
	MYSQL_TYPE_SHORT,
	MYSQL_TYPE_SHORT
      },
      {
	MYSQL_TYPE_LONG,
	MYSQL_TYPE_LONG,
	MYSQL_TYPE_LONG,
	MYSQL_TYPE_LONG
      }
    };

  this->stmt[0] = mysql_stmt_init(mysql);
  query = "INSERT INTO test VALUES(?)";
  mysql_stmt_prepare(this->stmt[0], query, strlen(query));
  this->params[0] = new (MYSQL_BIND[3]);
  this->params[0][1].buffer_type = MYSQL_TYPE_LONG;
  this->params[0][1].buffer = new (int);
  this->params[0][1].buffer_length = sizeof(int);
  this->params[0][1].length = &this->params[0][1].buffer_length;
  this->params[0][1].is_null = NULL;
  this->params[0][1].is_unsigned = 0;
  this->params[0][1].error = NULL;
  return ;
}

/**
 *  This method is called when an event is dumping its data.
 */
void MySQLOutput::OnDump(Event* e, ...)
{
  
}

/**
 *  This method is called when an event occur.
 */
void MySQLOutput::OnEvent(Event* e) throw ()
{
  bool mutex_locked;

  mutex_locked = false;
  try
    {
      this->mutex.Lock();
      mutex_locked = true;
      this->events.push_back(e);
    }
  catch (...)
    {
    }
  // Try to not to leave the mutex locked.
  if (mutex_locked)
    try
      {
	this->mutex.Unlock();
      }
    catch (...)
      {
      }
  return ;
}

/**
 *  This function waits for queries to be executed on the server on its own
 *  thread.
 */
int MySQLOutput::Core()
{
  Event* event;
  MYSQL* mysql;
  bool error = false;

  // XXX
  do
    {
      mysql = mysql_init(NULL);
      mysql_real_connect(mysql,
                         this->host.c_str(),
                         this->user.c_str(),
                         this->password.c_str(),
                         this->db.c_str(),
                         0,
                         NULL,
                         0);
      this->stmt[0] = mysql_stmt_init(mysql);
      this->PrepareQueries(mysql);
      while ((!this->exit_thread || !this->events.empty())
	     && !error)
	{
	  this->mutex.Lock();
	  if (this->events.empty())
	    {
	      this->condvar.Wait(this->mutex);
	      if (this->events.empty())
		{
		  this->mutex.Unlock();
		  assert(this->exit_thread);
		  break ;
		}
	    }
	  event = this->events.front();
	  this->events.pop_front();
	  event->Dump(this);
	  mysql_stmt_execute(this->stmt[0]);
	}
      // delete statements
      mysql_close(mysql);
      /*
	if (error)
	  sleep(mysql_retry_interval);
      */
    } while (!this->exit_thread); // Will loop on error
  return (1);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  MySQLOutput constructor.
 */
MySQLOutput::MySQLOutput()
{
}

/**
 *  MySQLOutput destructor.
 */
MySQLOutput::~MySQLOutput()
{
}

/**
 *  Close the MySQL connection and free associated ressources.
 */
void MySQLOutput::Destroy()
{
  this->exit_thread = true;
  return ;
}

/**
 *  Set MySQL connection informations but does not really connect to the
 *  server.
 */
void MySQLOutput::Init(const std::string& host,
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
