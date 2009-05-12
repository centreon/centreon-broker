/*
** mysqloutput.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/12/09 Matthieu Kermagoret
*/

#include <cassert>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <iostream>
#include <unistd.h>
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
 *  Commit the current transaction to the DB.
 */
void MySQLOutput::Commit()
{
  this->myconn_->commit();
  this->queries_count_ = 0;
  // If the clock is not working, there's nothing we can do.
  clock_gettime(CLOCK_REALTIME, &this->ts_);
  // XXX : commit time interval shall be configurable
  this->ts_.tv_sec += 10;
  return ;
}

/**
 *  Connect and prepare all DB-related objects.
 */
void MySQLOutput::Connect()
{
  // Get MySQL driver instance
  sql::Driver* driver = get_driver_instance();

  // Parameter checks
  if (this->host_.empty())
    this->host_ = "localhost";
  if (this->user_.empty())
    this->user_ = "root";
  if (this->db_.empty())
    this->db_ = "ndo";

  // MySQL connection
  this->myconn_ = driver->connect(this->host_,
                                  this->user_,
                                  this->password_);
  // XXX : use some JDBC stuff when implemented in MySQL C++ connector
  {
    std::auto_ptr<sql::Statement> use_db((*this->myconn_).createStatement());
    use_db->execute(std::string("USE ") + this->db_ + std::string(";"));
  }

  this->myconn_->setAutoCommit(false);
  // Prepared statements
  this->stmts_ = this->PrepareQueries(*this->myconn_);
  return ;
}

/**
 *  Free all ressources used by MySQL related objects.
 */
void MySQLOutput::Disconnect()
{
  if (this->stmts_)
    {
      for (int i = 0; this->stmts_[i]; i++)
	if (this->stmts_[i])
	  delete (this->stmts_[i]);
      delete (this->stmts_);
      this->stmts_ = NULL;
    }
  if (this->myconn_)
    {
      delete (this->myconn_);
      this->myconn_ = NULL;
    }
  return ;
}

/**
 *  Prepare the queries for execution on the MySQL server.
 */
sql::PreparedStatement** MySQLOutput::PrepareQueries(sql::Connection& conn)
  throw (Exception)
{
  sql::PreparedStatement** stmt;
  // XXX : tables names may be different
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
        "last_hard_state=?, "               \
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
        "check_timeperiod_object_id=?",
      "INSERT INTO nagios_servicestatus SET " \
        "servicestatus_id=?, "                \
        "instance_id=?, "                     \
        "service_object_id=?, "               \
        "status_update_time=?, "              \
        "output=?, "                          \
        "perfdata=?, "                        \
        "current_state=?, "                   \
        "has_been_checked=?, "                \
        "should_be_scheduled=?, "             \
        "current_check_attempt=?, "           \
        "max_check_attempts=?, "              \
        "last_check=?, "                      \
        "next_check=?, "                      \
        "check_type=?, "                      \
        "last_state_change=?, "               \
        "last_hard_state_change=?, "          \
        "last_hard_state=?, "                 \
        "last_time_ok=?, "                    \
        "last_time_warning=?, "               \
        "last_time_unknown=?, "               \
        "last_time_critical=?, "              \
        "state_type=?, "                      \
        "last_notification=?, "               \
        "next_notification=?, "               \
        "no_more_notifications=?, "           \
        "notifications_enabled=?, "           \
        "problem_has_been_acknowledged=?, "   \
        "acknowledgement_type=?, "            \
        "current_notification_number=?, "     \
        "passive_checks_enabled=?, "          \
        "active_checks_enabled=?, "           \
        "event_handler_enabled=?, "           \
        "flap_detection_enabled=?, "          \
        "is_flapping=?, "                     \
        "percent_state_change=?, "            \
        "latency=?, "                         \
        "execution_time=?, "                  \
        "scheduled_downtime_depth=?, "        \
        "failure_prediction_enabled=?, "      \
        "process_performance_data=?, "        \
        "obsess_over_service=?, "             \
        "modified_service_attributes=?, "     \
        "event_handler=?, "                   \
        "check_command=?, "                   \
        "normal_check_interval=?, "           \
        "retry_check_interval=?, "            \
        "check_timeperiod_object_id=?"
    };

  try
    {
      stmt = new sql::PreparedStatement*[sizeof(queries) / sizeof(*queries)];
      for (unsigned int i = 0; i < sizeof(queries) / sizeof(*queries); i++)
	stmt[i] = conn.prepareStatement(queries[i]);
      stmt[sizeof(queries) / sizeof(*queries)] = NULL;
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
 *  Process a received event.
 */
void MySQLOutput::ProcessEvent(Event* event)
{
  // XXX : if an exception occur, we should try to put the event back into the
  // list, otherwise properly discard it
  this->cur_stmt_ = this->stmts_[event->GetType()];
  this->cur_arg_ = 1;
  event->AcceptVisitor(*this);
  this->cur_stmt_->execute();
  // XXX : MySQL commit interval should be configurable
  if (++this->queries_count_ >= 10000)
    this->Commit();
  return ;
}

/**
 *  Wait for incoming events. Return NULL when thread shall exit.
 */
Event* MySQLOutput::WaitEvent()
{
  Event* event;

  event = NULL;
  this->eventsm_.Lock();
  while (!this->exit_thread_ || !this->events_.empty())
    {
      bool wait_return;
      struct timespec ts;

      // Try to fetch an event from the internal list.
      if (!this->events_.empty())
	{
	  event = this->events_.front();
	  this->events_.pop_front();
	  break ;
	}
      try
	{
	  wait_return = this->eventscv_.TimedWait(this->eventsm_, &this->ts_);
	}
      catch (ConditionVariableException& cve)
	{
	  // Even when an error occur, the mutex shall be locked. It is our
	  // responsability to release it.
	  this->eventsm_.Unlock();
	  throw ;
	}
      // The timeout occured, so commit data.
      if (wait_return)
	{
	  this->eventsm_.Unlock();
	  this->Commit();
	  this->eventsm_.Lock();
	}
    }
  this->eventsm_.Unlock();
  return (event);
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
      this->eventsm_.Lock();
      mutex_locked = true;
      this->events_.push_back(e);
      e->AddReader(this);
      this->eventscv_.Broadcast();
    }
  catch (...)
    {
      std::cerr << "Exception, dropping event." << std::endl;
    }
  // Try to not to leave the mutex locked.
  if (mutex_locked)
    try
      {
	this->eventsm_.Unlock();
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
  this->cur_stmt_->setString(this->cur_arg_, arg);
  this->cur_arg_++;
  return ;
}

/**
 *  Found an object of type double while visiting an event.
 */
void MySQLOutput::Visit(double arg)
{
  this->cur_stmt_->setDouble(this->cur_arg_, arg);
  this->cur_arg_++;
  return ;
}

/**
 *  Found an object of type int while visiting an event.
 */
void MySQLOutput::Visit(int arg)
{
  this->cur_stmt_->setInt(this->cur_arg_, arg);
  this->cur_arg_++;
  return ;
}

/**
 *  Found an object of type short while visiting an event.
 */
void MySQLOutput::Visit(short arg)
{
  // XXX
  this->cur_stmt_->setInt(this->cur_arg_, arg);
  this->cur_arg_++;
  return ;
}

/**
 *  Found an object of type const std::string& while visiting an event.
 */
void MySQLOutput::Visit(const std::string& arg)
{
  this->cur_stmt_->setString(this->cur_arg_, arg);
  this->cur_arg_++;
  return ;
}

/**
 *  Found an object of type time_t while visiting an event.
 */
void MySQLOutput::Visit(time_t arg)
{
  // XXX
  this->cur_stmt_->setDateTime(this->cur_arg_, ctime(&arg));
  this->cur_arg_++;
  return ;
}

/**
 *  This function waits for queries to be executed on the server on its own
 *  thread.
 */
int MySQLOutput::Core()
{
  while (!this->exit_thread_ || !this->events_.empty())
    {
      try
        {
          Event* event;

          this->Connect();
          // If the clock is not working, there's nothing we can do.
          clock_gettime(CLOCK_REALTIME, &this->ts_);
	  // XXX : commit time interval shall be configurable
	  this->ts_.tv_sec += 10;
          event = this->WaitEvent();
          while (event)
            {
              this->ProcessEvent(event);
              event->RemoveReader(this);
              event = this->WaitEvent();
            }
        }
      catch (sql::SQLException& e)
        {
          std::cerr << "Recoverable MySQL error" << std::endl
                    << "    " << e.what() << std::endl;
        }
      catch (...)
        {
	  std::cerr << "Unrecoverable error (" << __FUNCTION__ << ')'
                    << std::endl;
          break ;
        }
      this->Disconnect();
      // XXX : mysql retry interval shall be configurable
      sleep(10);
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
  this->myconn_ = NULL;
  this->stmts_ = NULL;
}

/**
 *  MySQLOutput destructor.
 */
MySQLOutput::~MySQLOutput()
{
  this->Disconnect();
  assert(this->events_.empty());
}

/**
 *  Close the MySQL connection and free associated ressources.
 */
void MySQLOutput::Destroy()
{
  this->exit_thread_ = true;
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
  this->exit_thread_ = false;
  this->host_ = host;
  this->user_ = user;
  this->password_ = password;
  this->db_ = db;
  this->Run();
  return ;
}
