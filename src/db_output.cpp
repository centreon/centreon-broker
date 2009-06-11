/*
** db_output.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/11/09 Matthieu Kermagoret
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
  this->timeout_ = boost::get_system_time() + boost::posix_time::seconds(3);
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
  // Truncate table `hosts`
  {
    std::auto_ptr<DB::Truncate> truncate(this->conn_->GetTruncateQuery());

    // XXX : table names should be configurable
    truncate->SetTable("hosts");
    truncate->Prepare();
    truncate->Execute();
  }
  // Truncate table `services`
  {
    std::auto_ptr<DB::Truncate> truncate(this->conn_->GetTruncateQuery());
    truncate->SetTable("services");
    truncate->Prepare();
    truncate->Execute();
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
      // XXX : some error message, somewhere
      std::cerr << "Exception while registering event, dropping it."
                << std::endl;
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
 *  Process an Host event.
 */
void DBOutput::ProcessHost(const Host& host)
{
  std::auto_ptr<DB::Insert<Host> >
    query(this->conn_->GetInsertQuery<Host>(host_mapping));

  query->Prepare();
  query->Execute(host);
  this->QueryExecuted();
  return ;
}

/**
 *  Process an HostStatus event.
 */
void DBOutput::ProcessHostStatus(const HostStatus& hs)
{
  try
    {
      this->host_status_stmt_->Execute(hs);
    }
  catch (DB::DBException& dbe)
    {
      if (dbe.GetReason() != DB::DBException::QUERY_EXECUTION)
	throw ;
    }
  if (this->host_status_stmt_->GetUpdateCount() == 0)
    this->ProcessHost(Host(hs));
  else
    this->QueryExecuted();
  return ;
}

/**
 *  Process a Service event.
 */
void DBOutput::ProcessService(const Service& service)
{
  // XXX : code unfinished
  return ;
}

/**
 *  Process a ServiceStatus event.
 */
void DBOutput::ProcessServiceStatus(const ServiceStatus& ss)
{
  // XXX : code unfinished
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
                break ;
            }
        }
      catch (Exception& e)
        {
          std::cerr << "Unrecoverable error (" << e.what() << ')'
                    << std::endl;
          break ;
        }
      this->Disconnect();
      // XXX : retry interval should be configurable
      sleep(10);
    }
  return ;
}

/**
 *  Request the object to stop processing.
 */
void DBOutput::Destroy()
{
  this->exit_ = true;
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
  this->thread_->detach();
  delete (this->thread_);
  this->thread_ = NULL;
  return ;
}
