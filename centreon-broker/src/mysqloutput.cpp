/*
** mysqloutput.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/07/09 Matthieu Kermagoret
*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <mysql.h>
#include "event.h"
#include "mysqloutput.h"

// XXX
#include <mysql_driver.h>
#include <mysql_public_iface.h>

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
sql::PreparedStatement** MySQLOutput::PrepareQueries(sql::Connection& conn)
  throw (Exception)
{
  sql::PreparedStatement** stmt;
  const char* queries[] =
    {
      "INSERT INTO nagios_hoststatus SET "  \
        "hoststatus_id=?, "                 \
        "instance_id=?, "                   \
        "host_object_id=?, "                \
        "status_update_time=?, "            \
        "output=?, "                        \
        "perfdata=?, "                      \
        "current_state=?, "                 \
        "has_been_checked=?, "              \
        "should_be_scheduled=?, "           \
        "current_check_attempt=?, "         \
        "max_check_attempts=?, "            \
        "last_check=?, "                    \
        "next_check=?, "                    \
        "check_type=?, "                    \
        "last_state_change=?, "             \
        "last_hard_state_change=?, "        \
        "last_time_up=?, "                  \
        "last_time_down=?, "                \
        "last_time_unreachable=?, "         \
        "state_type=?, "                    \
        "last_notification=?, "             \
        "next_notification=?, "             \
        "no_more_notifications=?, "         \
        "notifications_enabled=?, "         \
        "problem_has_been_acknowledged=?, " \
        "acknowledgement_type=?, "          \
        "current_notification_number=?, "   \
        "passive_checks_enabled=?, "        \
        "active_checks_enabled=?, "         \
        "event_handler_enabled=?, "         \
        "flap_detection_enabled=?, "        \
        "is_flapping=?, "                   \
        "percent_state_change=?, "          \
        "latency=?, "                       \
        "execution_time=?, "                \
        "scheduled_downtime_depth=?, "      \
        "failure_prediction_enabled=?, "    \
        "process_performance_data=?, "      \
        "obsess_over_host=?, "              \
        "modified_host_attributes=?, "      \
        "event_handler=?, "                 \
        "check_command=?, "                 \
        "normal_check_interval=?, "         \
        "retry_check_interval=?, "          \
        "check_timeperiod_object_id=?"
    };

  try
    {
      stmt = new sql::PreparedStatement*[sizeof(queries) / sizeof(*queries)];
      for (unsigned int i = 0; i < sizeof(queries) / sizeof(*queries); i++)
	stmt[i] = conn.prepareStatement(queries[i]);
    }
  catch (sql::SQLException& e)
    {
      throw (Exception("Statements preparation failed."));
    }
  catch (...)
    {
      throw (Exception("Statements allocation failed."));
    }
  return (stmt);
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
      this->condvar.Broadcast();
    }
  catch (...)
    {
      std::cerr << "Exception, dropping event." << std::endl;
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
 *  Found an object of type const char* while visiting an event.
 */
void MySQLOutput::Visit(const char* arg)
{
  this->current_stmt->setString(this->current_arg, arg);
  this->current_arg++;
  return ;
}

/**
 *  Found an object of type double while visiting an event.
 */
void MySQLOutput::Visit(double arg)
{
  this->current_stmt->setDouble(this->current_arg, arg);
  this->current_arg++;
  return ;
}

/**
 *  Found an object of type int while visiting an event.
 */
void MySQLOutput::Visit(int arg)
{
  this->current_stmt->setInt(this->current_arg, arg);
  this->current_arg++;
  return ;
}

/**
 *  Found an object of type short while visiting an event.
 */
void MySQLOutput::Visit(short arg)
{
  // XXX
  this->current_stmt->setInt(this->current_arg, arg);
  this->current_arg++;
  return ;
}

/**
 *  Found an object of type const std::string& while visiting an event.
 */
void MySQLOutput::Visit(const std::string& arg)
{
  this->current_stmt->setString(this->current_arg, arg);
  this->current_arg++;
  return ;
}

/**
 *  Found an object of type time_t while visiting an event.
 */
void MySQLOutput::Visit(time_t arg)
{
  // XXX
  this->current_stmt->setInt(this->current_arg, arg);
  this->current_arg++;
  return ;
}

/**
 *  This function waits for queries to be executed on the server on its own
 *  thread.
 */
int MySQLOutput::Core()
{
  try
    {
      /*
      ** Connect to the MySQL server.
      */
      sql::Driver* driver = get_driver_instance();
      assert(!this->host.empty());
      assert(!this->user.empty());
      assert(!this->password.empty());
      assert(!this->db.empty());
      sql::Connection* conn = driver->connect(this->host,
					      this->user,
					      this->password);
      {
	sql::Statement* use_db = (*conn).createStatement();
	use_db->execute(std::string("USE ") + this->db + std::string(";"));
	delete use_db;
      }
      // XXX : delete statements
      sql::PreparedStatement** stmts = this->PrepareQueries(*conn);
      while ((!this->exit_thread || !this->events.empty()))
	{
	  Event* event;

	  /*
	  ** Wait for incoming events
	  */
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

	  /*
	  ** Process event.
	  */
	  event = this->events.front();
	  this->events.pop_front();
	  this->mutex.Unlock();
	  this->current_stmt = stmts[event->GetType()];
	  this->current_arg = 1;
	  event->AcceptVisitor(*this);
	  this->current_stmt->execute();
	}
    }
  catch (sql::SQLException& e)
    {
      std::cerr << "Fail: " << e.what() << std::endl;
    }
  catch (...)
    {
      std::cerr << "Unknown failure" << std::endl;
    }
  return (0);
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
  this->Join();
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
  // XXX : optimize memory consumption of useless elements.
  this->host = host;
  this->user = user;
  this->password = password;
  this->db = db;
  this->Run();
  return ;
}
