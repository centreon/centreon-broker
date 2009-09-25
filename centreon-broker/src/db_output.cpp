/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <boost/bind.hpp>
#include <memory>
#include "db/connection.h"
#include "db/db_exception.h"
#ifdef USE_MYSQL
# include "db/mysql/connection.h"
#endif /* USE_MYSQL */
#ifdef USE_POSTGRESQL
# include "db/postgresql/connection.h"
#endif /* USE_POSTGRESQL */
#include "db/predicate.h"
#include "db_output.h"
#include "exception.h"
#include "event_publisher.h"
#include "events/comment.h"
#include "events/event.h"
#include "events/host.h"
#include "events/host_status.h"
#include "file_input.h"
#include "file_output.h"
#include "logging.h"
#include "nagios/broker.h"

using namespace CentreonBroker;
using namespace CentreonBroker::Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief DBOutput copy constructor.
 *
 *  As DBOutput are not copiable, the copy constructor is declared private. Any
 *  attempt to use it will result in a call to abort().
 *
 *  \param[in] dbo Unused.
 */
DBOutput::DBOutput(const DBOutput& dbo) : EventSubscriber()
{
  (void)dbo;
  assert(false);
  abort();
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  As DBOutput are not copiable, the assignment operator is declared private.
 *  Any attempt to use it will result in a call to abort().
 *
 *  \param[in] dbo Unused.
 *
 *  \return *this
 */
DBOutput& DBOutput::operator=(const DBOutput& dbo)
{
  (void)dbo;
  assert(false);
  abort();
  return (*this);
}

/**
 *  \brief Commit the current transaction and reset timeout.
 *
 *  Ask the underlying database to commit the current transaction. The timeout
 *  for autocommit is also rescheduled.
 */
void DBOutput::Commit()
{
  // Commit the transaction on the database server.
  this->conn_->Commit();

  // Reset the queries counter.
  this->queries_ = 0;

  // Reset the transaction timeout.
  this->timeout_ = boost::get_system_time()
                   + boost::posix_time::seconds(this->time_commit_interval_);

  return ;
}

/**
 *  \brief Connect to the preconfigured database.
 *
 *  Using parameters provided by Init(), connect to the database server. Upon
 *  successful return, the connection can later be dropped by calling
 *  Disconnect().
 *
 *  \see Disconnect
 *  \see Init
 */
void DBOutput::Connect()
{
  // Connect to the DB server
  switch (this->dbms_)
    {
#ifdef USE_MYSQL
     case DB::Connection::MYSQL:
      this->conn_.reset(new DB::MySQLConnection);
      break;
#endif /* USE_MYSQL */

#ifdef USE_ORACLE
     case DB::Connection::ORACLE:
      this->conn_.reset(new DB::OracleConnection);
      break ;
#endif /* USE_ORACLE */

#ifdef USE_POSTGRESQL
     case DB::Connection::POSTGRESQL:
      this->conn_.reset(new DB::PgSQLConnection);
      break ;
#endif /* USE_POSTGRESQL */

     default:
      throw (Exception(0, "Unsupported DBMS requested."));
    }
  this->conn_->Connect(this->host_,
                       this->user_,
                       this->password_,
                       this->db_);

  // Deactivate auto-commit.
  this->conn_->AutoCommit(false);

  // Initialize internal auto-commit timeout
  this->Commit();

  // Fetch already existing instances from the database and store them in a
  // cache.
  {
    std::auto_ptr<DB::Select> query(this->conn_->GetSelect());

    query->AddField("instance_name");
    query->AddField("instance_id");
    query->SetTable("program_status");
    query->Execute();
    while (query->Next())
      {
	std::string name;

	query->GetString(name);
	this->instances_[name] = query->GetInt();
      }
  }

  return ;
}

/**
 *  \brief Disconnect from the database server.
 *
 *  Upon a successful call to Connect(), one can disconnect from the server by
 *  calling this method. All previously allocated ressources are freed.
 *
 *  \see Connect
 */
void DBOutput::Disconnect()
{
  this->host_stmt_.reset();
  this->host_status_stmt_.reset();
  this->program_status_stmt_.reset();
  this->service_stmt_.reset();
  this->service_status_stmt_.reset();
  this->conn_.reset();
  this->instances_.clear();
  return ;
}

/**
 *  \brief Get the ID of an instance by its name.
 *
 *  The DBOutput class caches instance IDs as those are used within almost
 *  every table of the schema. This avoids expensive SELECT queries.
 *
 *  \param[in] instance The name of the Nagios instance.
 *
 *  \return The database ID of the Nagios instance.
 */
int DBOutput::GetInstanceId(const std::string& instance)
{
  int id;
  std::map<std::string, int>::iterator it;

  it = this->instances_.find(instance);
  if (it == this->instances_.end())
    {
      Events::ProgramStatus ps;
      std::auto_ptr<DB::MappedInsert<Events::ProgramStatus> >
        query(this->conn_->GetMappedInsert<Events::ProgramStatus>(
          program_status_get_mapping));

      ps.instance = instance;
      query->SetTable("program_status");
      query->SetArg(ps);
      query->Execute();
      id = query->InsertId();
      this->instances_[instance] = id;
    }
  else
    id = it->second;
  return (id);
}

/**
 *  \brief Insert the given object into an internal list for later processing.
 *
 *  Reminder : we use a publish / subscribe model for event propagation.
 *
 *  When a new event is published, the EventPublisher instance will call this
 *  method (we subclass EventSubscriber). We will store the event into our
 *  internal list. This list is processed by a separate thread.
 *
 *  \param[in] e The newly generated event.
 */
void DBOutput::OnEvent(Event* e) throw ()
{
  try
    {
      this->events_.Add(e);
    }
  catch (...)
    {
      logging.LogError("Exception while adding event to list. Dropping event.");
      e->RemoveReader(this);
    }
  return ;
}

/**
 *  Prepare most often used statements.
 */
void DBOutput::PrepareStatements()
{
  // Host insert statement.
  this->host_stmt_.reset(this->conn_->GetMappedInsert<Host>(host_get_mapping));
  this->host_stmt_->SetTable("host");
  this->host_stmt_->AddField("instance_id");
  this->host_stmt_->Prepare();

  // HostStatus update statement.
  this->host_status_stmt_.reset(this->conn_->GetMappedUpdate<HostStatus>(
                                  host_status_get_mapping));
  this->host_status_stmt_->SetTable("host");
  this->host_status_stmt_->SetPredicate(
    DB::And(DB::Equal(DB::Field("instance_id"),
		      DB::Placeholder()),
	    DB::Equal(DB::Field("host_name"),
		      DB::Placeholder())
	    ));
  this->host_status_stmt_->Prepare();

  // ProgramStatus update statement.
  this->program_status_stmt_.reset(this->conn_->GetMappedUpdate<ProgramStatus>(
                                     program_status_get_mapping));
  this->program_status_stmt_->SetTable("program_status");
  this->program_status_stmt_->SetPredicate(
    DB::Equal(DB::Field("instance_id"),
	      DB::Placeholder()));
  this->program_status_stmt_->Prepare();

  // Service insert statement.
  this->service_stmt_.reset(this->conn_->GetMappedInsert<Service>(
                              service_get_mapping));
  this->service_stmt_->SetTable("service");
  this->service_stmt_->AddField("instance_id");
  this->service_stmt_->Prepare();

  // ServiceStatus update statement.
  this->service_status_stmt_.reset(this->conn_->GetMappedUpdate<ServiceStatus>(
                                     service_status_get_mapping));
  this->service_status_stmt_->SetTable("service");
  this->service_status_stmt_->SetPredicate(
    DB::And(DB::Equal(DB::Field("instance_id"),
		      DB::Placeholder()),
	    DB::And(DB::Equal(DB::Field("host_name"),
			      DB::Placeholder()),
		    DB::Equal(DB::Field("service_description"),
			      DB::Placeholder()))
	    ));
  this->service_status_stmt_->Prepare();

  return ;
}

/**
 *  \brief Process an event.
 *
 *  When an event is poped from the list, it will be processed by this method.
 *  We will determine the true event type and process it accordingly.
 *
 *  \param[in] event Event that should be stored in the database.
 */
void DBOutput::ProcessEvent(Event* event)
{
  switch (event->GetType())
    {
     case Event::ACKNOWLEDGEMENT:
      ProcessAcknowledgement(*static_cast<Acknowledgement*>(event));
      break ;
     case Event::COMMENT:
      ProcessComment(*static_cast<Comment*>(event));
      break ;
     case Event::DOWNTIME:
      ProcessDowntime(*static_cast<Downtime*>(event));
      break ;
     case Event::HOST:
      ProcessHost(*static_cast<Host*>(event));
      break ;
     case Event::HOSTGROUP:
      ProcessHostGroup(*static_cast<HostGroup*>(event));
      break ;
     case Event::HOSTSTATUS:
      ProcessHostStatus(*static_cast<HostStatus*>(event));
      break ;
     case Event::LOG:
      ProcessLog(*static_cast<Log*>(event));
      break ;
     case Event::PROGRAMSTATUS:
      ProcessProgramStatus(*static_cast<ProgramStatus*>(event));
      break ;
     case Event::SERVICE:
      ProcessService(*static_cast<Service*>(event));
      break ;
     case Event::SERVICESTATUS:
      ProcessServiceStatus(*static_cast<ServiceStatus*>(event));
      break ;
     default:
      assert(false);
      throw (Exception(event->GetType(), "Invalid event type encountered"));
    }
  event->RemoveReader(this);
  return ;
}

/**
 *  Process an Acknowledgement event.
 */
void DBOutput::ProcessAcknowledgement(const Acknowledgement& ack)
{
#ifndef NDEBUG
  logging.LogDebug("Processing Acknowledgement event...");
#endif /* !NDEBUG */
  std::auto_ptr<DB::MappedInsert<Acknowledgement> >
    query(this->conn_->GetMappedInsert<Acknowledgement>(
            acknowledgement_get_mapping));

  query->SetTable("acknowledgement");
  query->AddField("instance_id");
  query->SetArg(ack);
  ((DB::HaveArgs*)query.get())->SetArg(this->GetInstanceId(ack.instance));
  query->Execute();
  this->QueryExecuted();
  return ;
}

/**
 *  Process a Comment event.
 */
void DBOutput::ProcessComment(const Comment& comment)
{
#ifndef NDEBUG
  logging.LogDebug("Processing Comment event...");
#endif /* !NDEBUG */
  if ((comment.type == NEBTYPE_COMMENT_ADD)
      || comment.type == NEBTYPE_COMMENT_LOAD)
    {
      std::auto_ptr<DB::MappedInsert<Comment> >
	query(this->conn_->GetMappedInsert<Comment>(comment_get_mapping));

      query->SetTable("comment");
      query->AddField("instance_id");
      query->SetArg(comment);
      ((DB::HaveArgs*)query.get())->SetArg(
        this->GetInstanceId(comment.instance));
      query->Execute();
      this->QueryExecuted();
    }
  else if (comment.type == NEBTYPE_COMMENT_DELETE)
    {
      std::auto_ptr<DB::Delete> query(this->conn_->GetDelete());

      query->SetPredicate(DB::Equal(DB::Field("internal_comment_id"),
                                    DB::Terminal(comment.internal_id)));
      query->Execute();
    }
  return ;
}

/**
 *  Process a Downtime event.
 */
void DBOutput::ProcessDowntime(const Downtime& downtime)
{
#ifndef NDEBUG
  logging.LogDebug("Processing Downtime event...");
#endif /* !NDEBUG */
  if ((downtime.type == NEBTYPE_DOWNTIME_ADD)
      || (downtime.type == NEBTYPE_DOWNTIME_LOAD))
    {
      std::auto_ptr<DB::MappedInsert<Downtime> >
	query(this->conn_->GetMappedInsert<Downtime>(downtime_get_mapping));

      query->SetTable("scheduled_downtime");
      query->AddField("instance_id");
      query->SetArg(downtime);
      ((DB::HaveArgs*)query.get())->SetArg(
        this->GetInstanceId(downtime.instance));
      query->Execute();
      this->QueryExecuted();
    }
  else if (downtime.type == NEBTYPE_DOWNTIME_START)
    {
      std::auto_ptr<DB::Update> query(this->conn_->GetUpdate());

      query->SetTable("scheduled_downtime");
      query->AddField("start_time");
      query->AddField("was_started");
      query->SetPredicate(DB::And(DB::Equal(DB::Field("instance_id"),
                                            DB::Terminal(this->GetInstanceId(
                                                           downtime.instance))
                                            ),
                                  DB::Equal(DB::Field("downtime_id"),
                                            DB::Terminal(downtime.id))));
      query->SetArg(time(NULL));
      query->SetArg(true);
      query->Execute();
      this->QueryExecuted();
    }
  else if ((downtime.type == NEBTYPE_DOWNTIME_STOP)
           || (downtime.type == NEBTYPE_DOWNTIME_DELETE))
    {
      std::auto_ptr<DB::Delete> query(this->conn_->GetDelete());

      query->SetTable("scheduled_downtime");
      query->SetPredicate(DB::And(DB::Equal(DB::Field("instance_id"),
                                            DB::Terminal(this->GetInstanceId(
                                                           downtime.instance))
                                            ),
                                  DB::Equal(DB::Field("downtime_id"),
                                            DB::Terminal(downtime.id))));
      query->Execute();
      this->QueryExecuted();
    }
  return ;
}

/**
 *  Process an Host event.
 */
void DBOutput::ProcessHost(const Host& host)
{
#ifndef NDEBUG
  logging.LogDebug("Processing Host event...");
#endif /* !NDEBUG */
  try
    {
      this->host_stmt_->SetArg(host);
      ((DB::HaveArgs*)this->host_stmt_.get())->SetArg(
        this->GetInstanceId(host.instance));
      this->host_stmt_->Execute();
      this->QueryExecuted();
    }
  catch (const DB::DBException& dbe) // usually because of a host redefinition
    {
      if (dbe.GetReason() != DB::DBException::QUERY_EXECUTION)
	throw ;
    }
  return ;
}

/**
 *  Process a HostGroup event.
 */
void DBOutput::ProcessHostGroup(const HostGroup& hg)
{
#ifndef NDEBUG
  logging.LogDebug("Processing HostGroup event...");
#endif /* !NDEBUG */
  std::auto_ptr<DB::MappedInsert<HostGroup> >
    query(this->conn_->GetMappedInsert<HostGroup>(host_group_get_mapping));

  query->SetTable("hostgroup");
  query->SetArg(hg);
  query->Execute();
  this->QueryExecuted();
  return ;
}

/**
 *  Process a HostStatus event.
 */
void DBOutput::ProcessHostStatus(const HostStatus& hs)
{
#ifndef NDEBUG
  logging.LogDebug("Processing HostStatus event...");
#endif /* !NDEBUG */
  try
    {
      this->host_status_stmt_->SetArg(hs);
      ((DB::HaveArgs*)this->host_status_stmt_.get())->SetArg(
        this->GetInstanceId(hs.instance));
      ((DB::HaveArgs*)this->host_status_stmt_.get())->SetArg(hs.host);
      this->host_status_stmt_->Execute();
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
 *  Process a Log event.
 */
void DBOutput::ProcessLog(const Log& log)
{
#ifndef NDEBUG
  logging.LogDebug("Processing Log event...");
#endif /* !NDEBUG */
  std::auto_ptr<DB::MappedInsert<Log> >
    query(this->conn_->GetMappedInsert<Log>(log_get_mapping));

  query->SetTable("log");
  query->SetArg(log);
  query->Execute();
  this->QueryExecuted();
  return ;
}

/**
 *  Process a ProgramStatus event.
 */
void DBOutput::ProcessProgramStatus(const ProgramStatus& ps)
{
#ifndef NDEBUG
  logging.LogDebug("Processing ProgramStatus event...");
#endif /* !NDEBUG */
  try
    {
      this->program_status_stmt_->SetArg(ps);
      ((DB::HaveArgs*)(this->program_status_stmt_.get()))->SetArg(
        this->GetInstanceId(ps.instance));
      this->program_status_stmt_->Execute();
    }
  catch (DB::DBException& dbe)
    {
      if (dbe.GetReason() != DB::DBException::QUERY_EXECUTION)
        throw ;
    }
  if (this->program_status_stmt_->GetUpdateCount() == 0)
    {
      std::auto_ptr<DB::MappedInsert<ProgramStatus> >
        query(this->conn_->GetMappedInsert<ProgramStatus>(
          program_status_get_mapping));

      query->SetTable("program_status");
      query->SetArg(ps);
      query->Execute();
      this->QueryExecuted();
    }
  else
    this->QueryExecuted();
  return ;
}

/**
 *  Process a Service event.
 */
void DBOutput::ProcessService(const Service& service)
{
#ifndef NDEBUG
  logging.LogDebug("Processing Service event...");
#endif /* !NDEBUG */
  // When processing the service definition, we have to fetch the corresponding
  // host_id before inserting it.
  std::auto_ptr<DB::Select> query(this->conn_->GetSelect());
  Service myservice(service);

  query->SetTable("host");
  query->AddField("host_id");
  query->SetPredicate(DB::And(DB::Equal(DB::Field("instance_id"),
                                        DB::Terminal(this->GetInstanceId(
                                                       myservice.instance))),
                              DB::Equal(DB::Field("host_name"),
                                        DB::Terminal(myservice.host.c_str()))));
  query->Execute();
  if (query->Next())
    myservice.host_id = query->GetInt();
  query.reset();

  this->service_stmt_->SetArg(myservice);
  ((DB::HaveArgs*)this->service_stmt_.get())->SetArg(
    this->GetInstanceId(myservice.instance));
  this->service_stmt_->Execute();
  this->QueryExecuted();
  return ;
}

/**
 *  Process a ServiceStatus event.
 */
void DBOutput::ProcessServiceStatus(const ServiceStatus& ss)
{
#ifndef NDEBUG
  logging.LogDebug("Processing ServiceStatus event...");
#endif /* !NDEBUG */
  try
    {
      this->service_status_stmt_->SetArg(ss);
      ((DB::HaveArgs*)this->service_status_stmt_.get())->SetArg(
        this->GetInstanceId(ss.instance));
      ((DB::HaveArgs*)this->service_status_stmt_.get())->SetArg(ss.host);
      ((DB::HaveArgs*)this->service_status_stmt_.get())->SetArg(ss.service);
      this->service_status_stmt_->Execute();
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
  return ;
}

/**
 *  Call this method when a query is executed.
 */
void DBOutput::QueryExecuted()
{
  // Increment the number of executed queries. If this number is greater than
  // the query_commit_interval configuration option, the transaction is
  // commited.
  if (++this->queries_ >= this->query_commit_interval_)
    this->Commit();

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief DBOutput default constructor.
 *
 *  Initialize the DBOutput with default parameters.
 *
 *  \param[in] dbms Type of the database to use.
 */
DBOutput::DBOutput(DB::Connection::DBMS dbms)
  : dbms_(dbms),
    conn_(NULL),
    host_status_stmt_(NULL),
    program_status_stmt_(NULL),
    service_status_stmt_(NULL),
    queries_(0),
    exit_(false),
    thread_(NULL)
{
  EventPublisher::GetInstance().Subscribe(this);
}

/**
 *  \brief DBOutput destructor.
 *
 *  Release all previously allocated ressources.
 */
DBOutput::~DBOutput()
{
#ifndef NDEBUG
  logging.LogDebug("Deleting DBOutput...");
#endif /* !NDEBUG */
  if (this->thread_)
    {
#ifndef NDEBUG
      logging.LogDebug("Waiting for the running DBOutput thread to finish...");
#endif /* !NDEBUG */
      this->thread_->join();
      delete (this->thread_);
    }
}

/**
 *  \brief Entry point of the processing thread.
 *
 *  When Init() is called, it creates a new thread. This method is the thread
 *  entry point.
 */
void DBOutput::operator()()
{
#ifndef NDEBUG
  logging.LogDebug("New thread created (DBOutput)");
#endif /* !NDEBUG */
  try
    {
#ifndef NDEBUG
      logging.LogDebug("Initial connection to the database...");
#endif /* !NDEBUG */
      this->Connect();
    }
  // An error occured on initialization, we won't try to go any further as it
  // is likely that something fatal happened (unreachable server, no memory
  // available, ...)
  catch (...)
    {
      logging.LogError("Initial database initialization failed...");
      logging.LogInfo("Aborting output...");
      this->Disconnect();
      return ;
    }

  // While nobody called Destroy()
  while (!this->exit_)
    {
      Event* event;

      try
        {
	  // Prepare more common statements to reduce load on SQL server
	  this->PrepareStatements();

	  // Load historical data from dumpfile
	  if (!this->dumpfile_.empty())
	    {
#ifndef NDEBUG
	      logging.LogDebug("Trying to open dump file...");
#endif /* !NDEBUG */
	      FileInput fileinput;
	      bool opened;
	      std::stringstream ss;

	      try
		{
		  fileinput.Open(this->dumpfile_);
#ifndef NDEBUG
		  logging.LogDebug("Dump file opened.");
#endif /* !NDEBUG */
		  opened = true;
		}
	      catch (...)
		{
		  opened = false;
		}
	      if (opened)
		{
#ifndef NDEBUG
		  logging.LogDebug("Loading historical data from dumped file...");
#endif /* !NDEBUG */
		  Event* event;

		  while ((event = fileinput.NextEvent()))
		    {
		      event->AddReader(this);
		      this->ProcessEvent(event);
		    }
		  fileinput.Close();
		}
	    }

#ifndef NDEBUG
	  logging.LogDebug("Launching event processing loop...");
#endif /* !NDEBUG */
	  // While no exception happens (like when connection to DB is dropped)
          while (1)
            {
              // Wait for an event with timeout and execute proper commands
              event = this->events_.TimedWait(this->timeout_);
              if (event)
                this->ProcessEvent(event);
              else if (!this->events_.Empty() || !this->exit_)
                this->Commit();
              else
		{
#ifndef NDEBUG
		  logging.LogDebug("DBOutput deletion requested, " \
                                   "exiting thread...");
#endif /* !NDEBUG */
		  break ;
		}
            }
        }

      // If such an exception occur, it's likely because the connection with
      // the DB server has been dropped. So we will disconnect and try to
      // reconnect later.
      catch (DB::DBException& dbe)
	{
	  logging.LogError("Recoverable DB error");
	  logging.LogError(dbe.what());
	}

      // Standard exception should be thrown if an unrecoverable error occured.
      catch (std::exception& e)
        {
	  logging.LogError("Unrecoverable error");
	  logging.LogError(e.what());
          break ;
        }
      catch (...)
	{
	  logging.LogError("Unknown unrecoverable error");
	  break ;
	}

      // Create a dump file, it might be a while before we can reconnect to
      // the DB server.
      FileOutput dumpfile;
      bool switched;

      switched = false;
      try
 	{
	  if (!this->dumpfile_.empty())
	    {
#ifndef NDEBUG
	      logging.LogDebug("Opening dump file...");
#endif /* !NDEBUG */
	      dumpfile.Open(this->dumpfile_.c_str());
	      dumpfile.Lock();
	      dumpfile.StoreEvents(this->events_);
	      EventPublisher::GetInstance().Subscribe(this, &dumpfile);
	      switched = true;
	      dumpfile.StoreEvents(this->events_);
	      dumpfile.Unlock();
	      dumpfile.Run();
	    }
	}
      catch (std::exception& e)
	{
	  logging.LogError("Error while using dump file...");
	  logging.LogError(e.what());
	  dumpfile.Close();
	}
      catch (...)
	{
	  logging.LogError("Unknown error while using dump file...");
	  dumpfile.Close();
	}

      // A DB::DBException occured. Those are recoverable so try to connect
      // again until it works.
      while (1)
	{
	  // Free connection ressources
	  this->Disconnect();

	  try
	    {
	      // Wait before trying to reconnect.
	      boost::thread::sleep(boost::get_system_time()
		                   + boost::posix_time::seconds(
                                       this->connection_retry_interval_));

	      // Destroy() has been called ?
	      if (!this->exit_)
		{
		  logging.LogInfo("Trying connection to DB server again...");
		  this->Connect();
		  // Restore original state
		  // XXX : what if dumpfile is not working ?
		  if (switched)
		    {
		      this->events_.Lock();
		      EventPublisher::GetInstance().Subscribe(&dumpfile, this);
		      this->events_.Unlock();
		    }
		}
	      break ;

	    }
	  // If an exception occur, try to sleep a bit then try again
	  catch (...)
	    {
	    }
	}
      if (this->exit_)
	break ;
    }
#ifndef NDEBUG
  logging.LogDebug("Exiting DBOutput thread");
#endif /* !NDEBUG */

  // Close connection
  this->Disconnect();

  return ;
}

/**
 *  \brief Request the object to stop processing.
 *
 *  This method requires the processing thread to stop.
 */
void DBOutput::Destroy()
{
#ifndef NDEBUG
  logging.LogDebug("Requesting DBOutput to stop processing...");
#endif /* !NDEBUG */
  this->exit_ = true;
  this->events_.CancelWait();
  if (this->thread_)
    this->thread_->interrupt();
  return ;
}

/**
 *  \brief Initialize the object.
 *
 *  Set connection parameters and launch the processing thread.
 *
 *  \param[in] host     IP address or host name to connect to.
 *  \param[in] user     User name to use for authentication.
 *  \param[in] password Password to use for authentication.
 *  \param[in] db       Name of the database to connect to.
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
  return ;
}

/**
 *  \brief Set the connection retry interval.
 *
 *  Specify the amount of time, in seconds, that the DBOutput object should
 *  wait before attempting to reconnect to its database.
 *
 *  \param[in] cri Number of seconds that the DBOutput object should wait
 *                 before attempting to reconnect to the database.
 */
void DBOutput::SetConnectionRetryInterval(unsigned int cri)
{
  this->connection_retry_interval_ = cri;
  return ;
}

/**
 *  Set the name of the file where events should be dumped on failure/exit.
 *
 *  \param[in] df Name of the file where events should be dumped.
 */
void DBOutput::SetDumpFile(const std::string& df)
{
  this->dumpfile_ = df;
  return ;
}

/**
 *  \brief Set the query commit interval.
 *
 *  Specify the maximum number of queries that can be executed before a
 *  transaction commit occurs.
 *
 *  \param[in] qci Maximum number of queries that can be executed before a
 *                 transaction commit occurs.
 */
void DBOutput::SetQueryCommitInterval(unsigned int qci)
{
  this->query_commit_interval_ = qci;
  return ;
}

/**
 *  \brief Set the time commit interval.
 *
 *  Specify the maximum amount of time, in seconds, that can elapse before a
 *  transaction commit occurs.
 *
 *  \param[in] tci Maximum amount of time, in seconds, that can elapse before a
 *                 transaction commit occurs.
 */
void DBOutput::SetTimeCommitInterval(unsigned int tci)
{
  this->time_commit_interval_ = tci;
  return ;
}
