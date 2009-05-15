/*
** mysqloutput.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/15/09 Matthieu Kermagoret
*/

#include <cassert>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <iostream>
#include <memory>
#include <sstream>
#include <unistd.h>
#include "event.h"
#include "hoststatusevent.h"
#include "mysqloutput.h"
#include "servicestatusevent.h"

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
    // XXX : use_db different name
    use_db->execute("TRUNCATE TABLE nagios_hoststatus;");
    use_db->execute("TRUNCATE TABLE nagios_servicestatus;");
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
      "UPDATE nagios_hoststatus SET "       \
        "instance_id=?, "                   \
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
        "check_timeperiod_object_id=? "     \
        "WHERE host_object_id=?",

      "UPDATE nagios_servicestatus SET "      \
        "instance_id=?, "                     \
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
        "check_timeperiod_object_id=? "       \
        "WHERE service_object_id=?"
    };

  try
    {
      stmt = new sql::PreparedStatement*[sizeof(queries) / sizeof(*queries) + 1];
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
  sql::PreparedStatement* stmt;

  // XXX : if an exception occur, we should try to put the event back into the
  // list, otherwise properly discard it
  switch (event->GetType())
    {
      // XXX
     case 0: // HostStatusEvent
      stmt = ProcessHostStatusEvent(static_cast<HostStatusEvent*>(event));
      break ;
     case 1: // ServiceStatusEvent
      stmt = ProcessServiceStatusEvent(static_cast<ServiceStatusEvent*>(event));
      break ;
     default: // Don't know event type
      assert(false);
      // XXX : quit one way or another
    }
  // XXX : assert(stmt);
  if (stmt)
    stmt->execute();
  // XXX : MySQL commit interval should be configurable
  if (++this->queries_count_ >= 10000)
    this->Commit();
  return ;
}

/**
 *  Process a HostStatusEvent.
 */
sql::PreparedStatement* MySQLOutput::ProcessHostStatusEvent(HostStatusEvent* h)
{
  int arg;
  int val;
  sql::PreparedStatement* stmt;
  static std::map<std::string, int> id;
  std::map<std::string, int>::iterator it;
  time_t t;

  {
    std::string id_str;

    id_str = h->GetNagiosInstance() + h->GetHost();
    it = id.find(id_str);
    if (it == id.end())
      {
	val = id.size() + 1;
	id[id_str] = val;
	{
	  std::stringstream val_str;
	  std::auto_ptr<sql::Statement> insert((*this->myconn_).createStatement());

	  val_str << val;
	  insert->execute(std::string("INSERT INTO nagios_hoststatus " \
				      " SET host_object_id=")
				      + val_str.str()
				      + ";");
	}
      }
    else
      val = (*it).second;
  }
  // XXX : Almost no type is good
  arg = 0;
  stmt = this->stmts_[0];
  // XXX : instance_id
  stmt->setInt(++arg, 1);

  // XXX : possible race condition with gmtime() if used with multiple
  //       MySQLOutput
  t = h->GetStatusUpdateTime();
  stmt->setDateTime(++arg, gmtime(&t));

  stmt->setString(++arg, h->GetOutput());
  stmt->setString(++arg, h->GetPerfdata());
  stmt->setInt(++arg, h->GetCurrentState());
  stmt->setInt(++arg, h->GetHasBeenChecked());
  stmt->setInt(++arg, h->GetShouldBeScheduled());
  stmt->setInt(++arg, h->GetCurrentCheckAttempt());
  stmt->setInt(++arg, h->GetMaxCheckAttempts());

  t = h->GetLastCheck();
  stmt->setDateTime(++arg, gmtime(&t));

  t = h->GetNextCheck();
  stmt->setDateTime(++arg, gmtime(&t));

  stmt->setInt(++arg, h->GetCheckType());

  t = h->GetLastStateChange();
  stmt->setDateTime(++arg, gmtime(&t));

  t = h->GetLastHardStateChange();
  stmt->setDateTime(++arg, gmtime(&t));

  stmt->setInt(++arg, h->GetLastHardState());

  t = h->GetLastTimeUp();
  stmt->setDateTime(++arg, gmtime(&t));

  t = h->GetLastTimeDown();
  stmt->setDateTime(++arg, gmtime(&t));

  t = h->GetLastTimeUnreachable();
  stmt->setDateTime(++arg, gmtime(&t));

  stmt->setInt(++arg, h->GetStateType());

  t = h->GetLastNotification();
  stmt->setDateTime(++arg, gmtime(&t));

  t = h->GetNextNotification();
  stmt->setDateTime(++arg, gmtime(&t));

  stmt->setInt(++arg, h->GetNoMoreNotifications());
  stmt->setInt(++arg, h->GetNotificationsEnabled());
  stmt->setInt(++arg, h->GetProblemHasBeenAcknowledged());
  stmt->setInt(++arg, h->GetAcknowledgementType());
  stmt->setInt(++arg, h->GetCurrentNotificationNumber());
  stmt->setInt(++arg, h->GetPassiveChecksEnabled());
  stmt->setInt(++arg, h->GetActiveChecksEnabled());
  stmt->setInt(++arg, h->GetEventHandlerEnabled());
  stmt->setInt(++arg, h->GetFlapDetectionEnabled());
  stmt->setInt(++arg, h->GetIsFlapping());
  stmt->setDouble(++arg, h->GetPercentStateChange());
  stmt->setDouble(++arg, h->GetLatency());
  stmt->setDouble(++arg, h->GetExecutionTime());
  stmt->setInt(++arg, h->GetScheduledDowntimeDepth());
  stmt->setInt(++arg, h->GetFailurePredictionEnabled());
  stmt->setInt(++arg, h->GetProcessPerformanceData());
  stmt->setInt(++arg, h->GetObsessOverHost());
  stmt->setInt(++arg, h->GetModifiedHostAttributes());
  stmt->setString(++arg, h->GetEventHandler());
  stmt->setString(++arg, h->GetCheckCommand());
  stmt->setInt(++arg, h->GetNormalCheckInterval());
  stmt->setInt(++arg, h->GetRetryCheckInterval());
  stmt->setInt(++arg, h->GetCheckTimeperiodObjectId());
  stmt->setInt(++arg, val);
  return (stmt);
}

/**
 *  Process a ServiceStatusEvent.
 */
sql::PreparedStatement* MySQLOutput::ProcessServiceStatusEvent(
  ServiceStatusEvent* sse)
{
  int arg;
  int val;
  sql::PreparedStatement* stmt;
  static std::map<std::string, int> id;
  std::map<std::string, int>::iterator it;

  {
    std::string id_str;

    id_str = sse->GetNagiosInstance() + sse->GetHost() + sse->GetService();
    it = id.find(id_str);
    if (it == id.end())
      {
	val = id.size() + 1;
	id[id_str] = val;
	{
	  std::stringstream val_str;
	  std::auto_ptr<sql::Statement> insert((*this->myconn_).createStatement());

	  val_str << val;
	  insert->execute(std::string("INSERT INTO nagios_servicestatus " \
				      " SET service_object_id=")
				      + val_str.str()
				      + ";");
	}
      }
    else
      val = (*it).second;
  }
  // XXX : Almost no type is good
  arg = 0;
  stmt = this->stmts_[1];

  // XXX : instance_id
  stmt->setInt(++arg, 1);
  stmt->setInt(++arg, sse->GetStatusUpdateTime());
  stmt->setString(++arg, sse->GetOutput());
  stmt->setString(++arg, sse->GetPerfdata());
  stmt->setInt(++arg, sse->GetCurrentState());
  stmt->setInt(++arg, sse->GetHasBeenChecked());
  stmt->setInt(++arg, sse->GetShouldBeScheduled());
  stmt->setInt(++arg, sse->GetCurrentCheckAttempt());
  stmt->setInt(++arg, sse->GetMaxCheckAttempts());
  stmt->setInt(++arg, sse->GetLastCheck());
  stmt->setInt(++arg, sse->GetNextCheck());
  stmt->setInt(++arg, sse->GetCheckType());
  stmt->setInt(++arg, sse->GetLastStateChange());
  stmt->setInt(++arg, sse->GetLastHardStateChange());
  stmt->setInt(++arg, sse->GetLastHardState());
  stmt->setInt(++arg, sse->GetLastTimeOk());
  stmt->setInt(++arg, sse->GetLastTimeWarning());
  stmt->setInt(++arg, sse->GetLastTimeUnknown());
  stmt->setInt(++arg, sse->GetLastTimeCritical());
  stmt->setInt(++arg, sse->GetStateType());
  stmt->setInt(++arg, sse->GetLastNotification());
  stmt->setInt(++arg, sse->GetNextNotification());
  stmt->setInt(++arg, sse->GetNoMoreNotifications());
  stmt->setInt(++arg, sse->GetNotificationsEnabled());
  stmt->setInt(++arg, sse->GetProblemHasBeenAcknowledged());
  stmt->setInt(++arg, sse->GetAcknowledgementType());
  stmt->setInt(++arg, sse->GetCurrentNotificationNumber());
  stmt->setInt(++arg, sse->GetPassiveChecksEnabled());
  stmt->setInt(++arg, sse->GetActiveChecksEnabled());
  stmt->setInt(++arg, sse->GetEventHandlerEnabled());
  stmt->setInt(++arg, sse->GetFlapDetectionEnabled());
  stmt->setInt(++arg, sse->GetIsFlapping());
  stmt->setDouble(++arg, sse->GetPercentStateChange());
  stmt->setDouble(++arg, sse->GetLatency());
  stmt->setDouble(++arg, sse->GetExecutionTime());
  stmt->setInt(++arg, sse->GetScheduledDowntimeDepth());
  stmt->setInt(++arg, sse->GetFailurePredictionEnabled());
  stmt->setInt(++arg, sse->GetProcessPerformanceData());
  stmt->setInt(++arg, sse->GetObsessOverService());
  stmt->setInt(++arg, sse->GetModifiedServiceAttributes());
  stmt->setString(++arg, sse->GetEventHandler());
  stmt->setString(++arg, sse->GetCheckCommand());
  stmt->setDouble(++arg, sse->GetNormalCheckInterval());
  stmt->setDouble(++arg, sse->GetRetryCheckInterval());
  stmt->setInt(++arg, sse->GetCheckTimeperiodObjectId());
  stmt->setInt(++arg, val);
  return (stmt);
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
