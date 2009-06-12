/*
** db_output.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/12/09 Matthieu Kermagoret
*/

#include <boost/bind.hpp>
#include <memory>
#include "db/db_exception.h"
#include "db/mysql/connection.h"
#include "db/predicate.h"
#include "db_output.h"
#include "event.h"
#include "exception.h"
#include "host.h"
#include "host_status.h"
#include "logging.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Commit the current transaction and reset timeout.
 */
void DBOutput::Commit()
{
  this->conn_->Commit();
  this->queries_ = 0;
  // XXX : timeout should be configurable
  this->timeout_ = boost::get_system_time() + boost::posix_time::seconds(7);
  return ;
}

/**
 *  Perform all DB connection related operations.
 */
void DBOutput::Connect()
{
  // Connect to the DB server
  switch (this->dbms_)
    {
     case DB::Connection::MYSQL:
      this->conn_ = new DB::MySQLConnection;
      break;
     default:
      throw (Exception(0, "Unsupported DBMS requested."));
    }
  this->conn_->Connect(this->host_,
                       this->user_,
                       this->password_,
                       this->db_);
  this->conn_->AutoCommit(false);
  // Truncate table `hosts`
  {
    logging.AddDebug("Truncating table `hosts`");
    logging.Indent();

    std::auto_ptr<DB::Truncate> truncate(this->conn_->GetTruncateQuery());

    // XXX : table names should be configurable
    truncate->SetTable("hosts");
    truncate->Prepare();
    truncate->Execute();
    logging.Deindent();
  }
  // Truncate table `services`
  {
    logging.AddDebug("Truncating table `services`");
    logging.Indent();

    std::auto_ptr<DB::Truncate> truncate(this->conn_->GetTruncateQuery());

    truncate->SetTable("services");
    truncate->Prepare();
    truncate->Execute();
    logging.Deindent();
  }
  // Prepare HostStatus update query
  {
    // XXX : not valid if multiple DBOutput are used
    host_mapping.AddIntField("instance_id", boost::bind(&DBOutput::GetInstanceId,
							this, _1));
    this->host_status_stmt_
      = this->conn_->GetUpdateQuery<HostStatus>(host_status_mapping);
    this->host_status_stmt_->SetPredicate(
      DB::And(DB::Equal(DB::Field("instance_id"),
                        DB::DynamicInt<HostStatus>(boost::bind(
                                                   &DBOutput::GetInstanceId,
                                                   this,
                                                   _1))),
	      DB::Equal(DB::Field("host_name"),
			DB::DynamicString<HostStatus>(&HostStatus::GetHostName)
			)
	      ));
    this->host_status_stmt_->Prepare();
  }
  // Prepare HostStatus update query
  {
    // XXX : not valid if multiple DBOutput are used
    service_mapping.AddIntField("instance_id", boost::bind(&DBOutput::GetInstanceId,
							this, _1));
    this->service_status_stmt_
      = this->conn_->GetUpdateQuery<ServiceStatus>(service_status_mapping);
    this->service_status_stmt_->SetPredicate(
      DB::And(DB::Equal(DB::Field("instance_id"),
                        DB::DynamicInt<ServiceStatus>(boost::bind(
                                                   &DBOutput::GetInstanceId,
                                                   this,
                                                   _1))),
	      DB::And(DB::Equal(DB::Field("host_name"),
			DB::DynamicString<ServiceStatus>(&ServiceStatus::GetHostName)
				),
		      DB::Equal(DB::Field("service_description"),
				DB::DynamicString<ServiceStatus>(&ServiceStatus::GetServiceDescription)))
	      ));
    this->service_status_stmt_->Prepare();
  }
  // Initialize the timeout
  this->Commit();
  return ;
}

/**
 *  Delete all DB connection related objects.
 */
void DBOutput::Disconnect()
{
  // XXX : delete statements
  if (this->conn_)
    {
      delete (this->conn_);
      this->conn_ = NULL;
    }
  return ;
}

/**
 *  Get an instance id.
 */
int DBOutput::GetInstanceId(const Event& event)
{
  int id;
  const std::string& instance(event.GetNagiosInstance());
  // XXX : instance ids should be synchronized with DB
  std::map<std::string, int>::iterator it;

  it = this->instances_.find(instance);
  if (it == this->instances_.end())
    {
      id = this->instances_.size() + 1;
      this->instances_[instance] = id;
    }
  else
    id = this->instances_[instance];
  return (id);
}

/**
 *  Callback called when an event occur. This will push the event into the
 *  list.
 */
void DBOutput::OnEvent(Event* e) throw ()
{
  try
    {
      this->events_.Add(e);
    }
  catch (...)
    {
      logging.AddError("Exception while adding event to list. Dropping event.");
      e->RemoveReader(this);
    }
  return ;
}

/**
 *  Process an event.
 */
void DBOutput::ProcessEvent(Event* event)
{
  switch (event->GetType())
    {
      // XXX : replace hardcoded values with macros
     case 0:
      ProcessHostStatus(*static_cast<HostStatus*>(event));
      break ;
     case 1:
      ProcessServiceStatus(*static_cast<ServiceStatus*>(event));
      break ;
     case 5:
      ProcessConnection(*static_cast<Connection*>(event));
      break ;
     case 8:
      ProcessService(*static_cast<Service*>(event));
      break ;
     case 9:
      ProcessHost(*static_cast<Host*>(event));
      break ;
      // XXX : assert(false) + throw()
    }
  event->RemoveReader(this);
  return ;
}

/**
 *  Process a Connection event.
 */
void DBOutput::ProcessConnection(const Connection& connection)
{
  logging.AddDebug("Processing Connection event...");
  logging.Indent();

  std::auto_ptr<DB::Insert<Connection> >
    query(this->conn_->GetInsertQuery<Connection>(connection_mapping));

  query->Prepare();
  query->Execute(connection);
  this->QueryExecuted();
  logging.Deindent();
  return ;
}


/**
 *  Process an Host event.
 */
void DBOutput::ProcessHost(const Host& host)
{
  logging.AddDebug("Processing Host event...");
  logging.Indent();

  std::auto_ptr<DB::Insert<Host> >
    query(this->conn_->GetInsertQuery<Host>(host_mapping));

  query->Prepare();
  query->Execute(host);
  this->QueryExecuted();
  logging.Deindent();
  return ;
}

/**
 *  Process an HostStatus event.
 */
void DBOutput::ProcessHostStatus(const HostStatus& hs)
{
  logging.AddDebug("Processing HostStatus event...");
  logging.Indent();
  try
    {
      this->host_status_stmt_->Execute(hs);
    }
  catch (DB::DBException& dbe)
    {
      if (dbe.GetReason() != DB::DBException::QUERY_EXECUTION)
	{
	  logging.Deindent();
	  throw ;
	}
    }
  if (this->host_status_stmt_->GetUpdateCount() == 0)
    this->ProcessHost(Host(hs));
  else
    this->QueryExecuted();
  logging.Deindent();
  return ;
}

/**
 *  Process a Service event.
 */
void DBOutput::ProcessService(const Service& service)
{
  std::auto_ptr<DB::Insert<Service> >
    query(this->conn_->GetInsertQuery<Service>(service_mapping));

  logging.AddDebug("Processing Service event...");
  logging.Indent();
  query->Prepare();
  query->Execute(service);
  this->QueryExecuted();
  logging.Deindent();
  return ;
}

/**
 *  Process a ServiceStatus event.
 */
void DBOutput::ProcessServiceStatus(const ServiceStatus& ss)
{
  logging.AddDebug("Processing ServiceStatus event...");
  logging.Indent();
  try
    {
      this->service_status_stmt_->Execute(ss);
    }
  catch (DB::DBException& dbe)
    {
      if (dbe.GetReason() != DB::DBException::QUERY_EXECUTION)
	throw ;
    }
  if (this->service_status_stmt_->GetUpdateCount() == 0)
    this->ProcessService(Service(ss));
  else
    this->QueryExecuted();
  logging.Deindent();
  return ;
}

/**
 *  Call this method when a query is executed.
 */
void DBOutput::QueryExecuted()
{
  // XXX : query number should be configurable
  if (++this->queries_ >= 10000)
    this->Commit();
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  DBOutput default constructor.
 */
DBOutput::DBOutput(DB::Connection::DBMS dbms) : dbms_(dbms)
{
}

/**
 *  DBOutput copy constructor.
 */
DBOutput::DBOutput(const DBOutput& dbo) : EventSubscriber(dbo)
{
}

/**
 *  DBOutput destructor.
 */
DBOutput::~DBOutput()
{
  logging.AddDebug("Deleting DBOutput...");
  if (this->thread_)
    {
      logging.Indent();
      logging.AddDebug("Waiting for the running thread to finish");
      this->thread_->join();
      delete (this->thread_);
      logging.Deindent();
    }
}

/**
 *  DBOutput operator= overload.
 */
DBOutput& DBOutput::operator=(const DBOutput& dbo)
{
}

/**
 *  Thread entry point.
 */
void DBOutput::operator()()
{
  logging.AddDebug("New thread created (DBOutput)");
  while (!this->exit_ || !this->events_.Empty())
    {
      try
        {
          Event* event;

          this->Connect();
          while (1)
            {
              event = this->events_.TimedWait(this->timeout_);
              if (event)
                this->ProcessEvent(event);
              else if (!this->events_.Empty() || !this->exit_)
                this->Commit();
              else
		{
		  logging.AddDebug("DBOutput deletion requested, " \
                                   "exiting thread...");
		  break ;
		}
            }
        }
      catch (DB::DBException& dbe)
	{
	  logging.AddError("Recoverable DB error");
	  logging.Indent();
	  logging.AddError(dbe.what());
	  logging.Deindent();
	}
      catch (Exception& e)
        {
	  logging.AddError("Unrecoverable error");
	  logging.Indent();
	  logging.AddError(e.what());
	  logging.Deindent();
          break ;
        }
      this->Disconnect();
      if (!this->exit_)
	{
	  // XXX : retry interval should be configurable
	  sleep(10);
	  logging.AddInfo("Trying connection to DB server again...");
	}
    }
  logging.AddDebug("Exiting DBOutput thread");
  logging.ThreadOver();
  return ;
}

/**
 *  Request the object to stop processing.
 */
void DBOutput::Destroy()
{
  logging.AddDebug("Requesting DBOutput to stop processing...");
  logging.Indent();
  this->exit_ = true;
  this->events_.CancelWait();
  logging.Deindent();
  return ;
}

/**
 *  Initialize the object.
 */
void DBOutput::Init(const std::string& host,
                    const std::string& user,
                    const std::string& password,
                    const std::string& db)
{
  this->host_ = host;
  this->user_ = user;
  this->password_ = password;
  this->db_ = db;
  this->exit_ = false;
  this->thread_ = new boost::thread(boost::ref(*this));
  /*
  this->thread_->detach();
  delete (this->thread_);
  this->thread_ = NULL;
  */
  return ;
}
