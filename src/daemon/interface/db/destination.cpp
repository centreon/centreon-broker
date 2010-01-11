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

#include <memory>
#include "db/connection.h"
#include "db/db_exception.h"
#include "db/delete.h"
#include "db/insert.h"
#ifdef USE_MYSQL
# include "db/mysql/connection.h"
#endif /* USE_MYSQL */
#ifdef USE_POSTGRESQL
# include "db/postgresql/connection.h"
#endif /* USE_POSTGRESQL */
#include "db/predicate.h"
#include "db/select.h"
#include "db/update.h"
#include "interface/db/destination.h"
#include "exception.h"
#include "events/comment.h"
#include "events/event.h"
#include "events/host.h"
#include "events/host_group.h"
#include "events/host_status.h"
#include "events/service_group.h"
#include "logging.h"
#include "nagios/broker.h"

using namespace Interface::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Destination copy constructor.
 *
 *  As Destination is not copiable, the copy constructor is declared private.
 *  Any attempt to use it will result in a call to abort().
 *
 *  \param[in] destination Unused.
 */
Destination::Destination(const Destination& destination)
  : Interface::Destination()
{
  (void)destination;
  assert(false);
  abort();
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  As Destination is not copiable, the assignment operator is declared
 *  private. Any attempt to use it will result in a call to abort().
 *
 *  \param[in] destination Unused.
 *
 *  \return *this
 */
Destination& Destination::operator=(const Destination& destination)
{
  (void)destination;
  assert(false);
  abort();
  return (*this);
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
void Destination::Connect()
{
  // Fetch already existing instances from the database and store them in a
  // cache.
  {
    std::auto_ptr<CentreonBroker::DB::Select> query(this->conn_->GetSelect());

    query->SetTable("program_status");
    query->AddField("instance_name");
    query->AddField("instance_id");
    query->Execute();
    while (query->Next())
      {
	std::string name;

	query->GetString(name);
	this->instances_[name] = query->GetInt();
      }
  }

  // Deactivate autocommit.
  this->conn_->AutoCommit(false);

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
void Destination::Disconnect()
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
 *  The Destination class caches instance IDs as those are used within almost
 *  every table of the schema. This avoids expensive SELECT queries.
 *
 *  \param[in] instance The name of the Nagios instance.
 *
 *  \return The database ID of the Nagios instance.
 */
int Destination::GetInstanceId(const std::string& instance)
{
  int id;
  std::map<std::string, int>::iterator it;

  it = this->instances_.find(instance);
  if (it == this->instances_.end())
    {
      Events::ProgramStatus ps;
      std::auto_ptr<CentreonBroker::DB::MappedInsert<Events::ProgramStatus> >
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
 *  Prepare most often used statements.
 */
void Destination::PrepareStatements()
{
  // Host insert statement.
  this->host_stmt_.reset(this->conn_->GetMappedInsert<Events::Host>(host_get_mapping));
  this->host_stmt_->SetTable("host");
  this->host_stmt_->AddField("instance_id");
  this->host_stmt_->Prepare();

  // HostStatus update statement.
  this->host_status_stmt_.reset(this->conn_->GetMappedUpdate<Events::HostStatus>(
                                  host_status_get_mapping));
  this->host_status_stmt_->SetTable("host");
  this->host_status_stmt_->SetPredicate(
					CentreonBroker::DB::And(CentreonBroker::DB::Equal(CentreonBroker::DB::Field("instance_id"),
											  CentreonBroker::DB::Placeholder()),
								CentreonBroker::DB::Equal(CentreonBroker::DB::Field("host_name"),
											  CentreonBroker::DB::Placeholder())
	    ));
  this->host_status_stmt_->Prepare();

  // ProgramStatus update statement.
  this->program_status_stmt_.reset(this->conn_->GetMappedUpdate<Events::ProgramStatus>(
                                     program_status_get_mapping));
  this->program_status_stmt_->SetTable("program_status");
  this->program_status_stmt_->SetPredicate(
					   CentreonBroker::DB::Equal(CentreonBroker::DB::Field("instance_id"),
								     CentreonBroker::DB::Placeholder()));
  this->program_status_stmt_->Prepare();

  // Service insert statement.
  this->service_stmt_.reset(this->conn_->GetMappedInsert<Events::Service>(
                              service_get_mapping));
  this->service_stmt_->SetTable("service");
  this->service_stmt_->AddField("instance_id");
  this->service_stmt_->Prepare();

  // ServiceStatus update statement.
  this->service_status_stmt_.reset(this->conn_->GetMappedUpdate<Events::ServiceStatus>(
                                     service_status_get_mapping));
  this->service_status_stmt_->SetTable("service");
  this->service_status_stmt_->SetPredicate(
					   CentreonBroker::DB::And(CentreonBroker::DB::Equal(CentreonBroker::DB::Field("instance_id"),
											     CentreonBroker::DB::Placeholder()),
								   CentreonBroker::DB::And(CentreonBroker::DB::Equal(CentreonBroker::DB::Field("host_name"),
														     CentreonBroker::DB::Placeholder()),
											   CentreonBroker::DB::Equal(CentreonBroker::DB::Field("service_description"),
														     CentreonBroker::DB::Placeholder()))
	    ));
  this->service_status_stmt_->Prepare();

  return ;
}

/**
 *  Process an Acknowledgement event.
 */
void Destination::ProcessAcknowledgement(const Events::Acknowledgement& ack)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Processing Acknowledgement event...");
#endif /* !NDEBUG */
  std::auto_ptr<CentreonBroker::DB::MappedInsert<Events::Acknowledgement> >
    query(this->conn_->GetMappedInsert<Events::Acknowledgement>(
            acknowledgement_get_mapping));

  query->SetTable("acknowledgement");
  query->AddField("instance_id");
  query->SetArg(ack);
  ((CentreonBroker::DB::HaveArgs*)query.get())->SetArg(this->GetInstanceId(ack.instance));
  try
    {
      query->Execute();
    }
  catch (const CentreonBroker::DB::DBException& dbe) // acknowledgement redefinition
    {
      // XXX : shouldn't we try to update the comment ?
      if (dbe.GetReason() != CentreonBroker::DB::DBException::QUERY_EXECUTION)
        throw ;
    }
  return ;
}

/**
 *  Process a Comment event.
 */
void Destination::ProcessComment(const Events::Comment& comment)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Processing Comment event...");
#endif /* !NDEBUG */
  if ((comment.type == NEBTYPE_COMMENT_ADD)
      || comment.type == NEBTYPE_COMMENT_LOAD)
    {
      std::auto_ptr<CentreonBroker::DB::MappedInsert<Events::Comment> >
	query(this->conn_->GetMappedInsert<Events::Comment>(comment_get_mapping));

      query->SetTable("comment");
      query->AddField("instance_id");
      query->SetArg(comment);
      ((CentreonBroker::DB::HaveArgs*)query.get())->SetArg(
        this->GetInstanceId(comment.instance));
      try
        {
          query->Execute();
        }
      catch (const CentreonBroker::DB::DBException& dbe) // comment redefinition
        {
          // XXX : shouldn't we try to update the comment ?
          if (dbe.GetReason() != CentreonBroker::DB::DBException::QUERY_EXECUTION)
            throw ;
        }
    }
  else if (comment.type == NEBTYPE_COMMENT_DELETE)
    {
      std::auto_ptr<CentreonBroker::DB::Delete> query(this->conn_->GetDelete());

      query->SetTable("comment");
      query->SetPredicate(CentreonBroker::DB::Equal(CentreonBroker::DB::Field("internal_comment_id"),
						    CentreonBroker::DB::Terminal(comment.internal_id)));
      query->Execute();
    }
  return ;
}

/**
 *  Process a Downtime event.
 */
void Destination::ProcessDowntime(const Events::Downtime& downtime)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Processing Downtime event...");
#endif /* !NDEBUG */
  if ((downtime.type == NEBTYPE_DOWNTIME_ADD)
      || (downtime.type == NEBTYPE_DOWNTIME_LOAD))
    {
      std::auto_ptr<CentreonBroker::DB::MappedInsert<Events::Downtime> >
	query(this->conn_->GetMappedInsert<Events::Downtime>(downtime_get_mapping));

      query->SetTable("scheduled_downtime");
      query->AddField("instance_id");
      query->SetArg(downtime);
      ((CentreonBroker::DB::HaveArgs*)query.get())->SetArg(
        this->GetInstanceId(downtime.instance));
      try
        {
          query->Execute();
        }
      catch (const CentreonBroker::DB::DBException& dbe) // downtime redefinition
        {
          // XXX : shouldn't we try to update the downtime ?
          if (dbe.GetReason() != CentreonBroker::DB::DBException::QUERY_EXECUTION)
            throw ;
        }
    }
  else if (downtime.type == NEBTYPE_DOWNTIME_START)
    {
      std::auto_ptr<CentreonBroker::DB::Update> query(this->conn_->GetUpdate());

      query->SetTable("scheduled_downtime");
      query->AddField("start_time");
      query->AddField("was_started");
      query->SetPredicate(CentreonBroker::DB::And(CentreonBroker::DB::Equal(CentreonBroker::DB::Field("instance_id"),
									    CentreonBroker::DB::Terminal(this->GetInstanceId(
                                                           downtime.instance))
                                            ),
						  CentreonBroker::DB::Equal(CentreonBroker::DB::Field("downtime_id"),
							    CentreonBroker::DB::Terminal(downtime.id))));
      query->SetArg(time(NULL));
      query->SetArg(true);
      query->Execute();
      if (query->GetUpdateCount() == 0)
        {
	  Events::Downtime dt(downtime);

          dt.type = NEBTYPE_DOWNTIME_ADD;
          this->ProcessDowntime(dt);
        }
    }
  else if ((downtime.type == NEBTYPE_DOWNTIME_STOP)
           || (downtime.type == NEBTYPE_DOWNTIME_DELETE))
    {
      std::auto_ptr<CentreonBroker::DB::Delete> query(this->conn_->GetDelete());

      query->SetTable("scheduled_downtime");
      query->SetPredicate(CentreonBroker::DB::And(CentreonBroker::DB::Equal(CentreonBroker::DB::Field("instance_id"),
							    CentreonBroker::DB::Terminal(this->GetInstanceId(
                                                           downtime.instance))
                                            ),
						  CentreonBroker::DB::Equal(CentreonBroker::DB::Field("downtime_id"),
									    CentreonBroker::DB::Terminal(downtime.id))));
      query->Execute();
    }
  return ;
}

/**
 *  Process an Host event.
 */
void Destination::ProcessHost(const Events::Host& host)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Processing Host event...");
#endif /* !NDEBUG */
  this->host_stmt_->SetArg(host);
  ((CentreonBroker::DB::HaveArgs*)this->host_stmt_.get())->SetArg(
    this->GetInstanceId(host.instance));
  try
    {
      this->host_stmt_->Execute();
    }
  catch (const CentreonBroker::DB::DBException& dbe) // usually because of a host redefinition
    {
      if (dbe.GetReason() != CentreonBroker::DB::DBException::QUERY_EXECUTION)
	throw ;
    }
  return ;
}

/**
 *  Process a HostGroup event.
 */
void Destination::ProcessHostGroup(const Events::HostGroup& hg)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Processing HostGroup event...");
#endif /* !NDEBUG */
  int id;
  std::auto_ptr<CentreonBroker::DB::MappedInsert<Events::Group> >
    query(this->conn_->GetMappedInsert<Events::Group>(group_get_mapping));

  query->SetTable("hostgroup");
  query->SetArg(hg);
  try
    {
      query->Execute();
      id = query->InsertId();
    }
  catch (const CentreonBroker::DB::DBException& dbe)
    {
      if (dbe.GetReason() != CentreonBroker::DB::DBException::QUERY_EXECUTION)
        throw ;

      // Get ID of already inserted host group
      std::auto_ptr<CentreonBroker::DB::Select> select(this->conn_->GetSelect());

      select->SetTable("hostgroup");
      select->AddField("id");
      select->SetPredicate(CentreonBroker::DB::Equal(CentreonBroker::DB::Field("hostgroup_name"),
                                     CentreonBroker::DB::Terminal(hg.name.c_str())));
      select->Execute();
      if (!select->Next()) // can't find host group
	return ;
      id = select->GetInt();
    }
  for (std::list<std::string>::const_iterator it = hg.members.begin();
       it != hg.members.end();
       ++it)
    {
      int host_id;
      std::auto_ptr<CentreonBroker::DB::Select> select(this->conn_->GetSelect());

      select->SetTable("host");
      select->AddField("id");
      select->SetPredicate(CentreonBroker::DB::And(CentreonBroker::DB::Equal(CentreonBroker::DB::Field("instance_id"),
                                             CentreonBroker::DB::Terminal(this->GetInstanceId(
                                                            hg.instance))),
				   CentreonBroker::DB::Equal(CentreonBroker::DB::Field("host_name"),
                                             CentreonBroker::DB::Terminal((*it).c_str()))));
      select->Execute();
      if (!select->Next())
        continue ;
      host_id = select->GetInt();
      select.reset();

      std::auto_ptr<CentreonBroker::DB::Insert> insert(this->conn_->GetInsert());
      insert->SetTable("host_hostgroup");
      insert->AddField("host");
      insert->AddField("hostgroup");
      insert->SetArg(host_id);
      insert->SetArg(id);
      try
	{
	  insert->Execute();
	}
      catch (const CentreonBroker::DB::DBException& dbe)
	{
	  if (dbe.GetReason() != CentreonBroker::DB::DBException::QUERY_EXECUTION)
	    throw ;
	}
    }
  return ;
}

/**
 *  Process a HostStatus event.
 */
void Destination::ProcessHostStatus(const Events::HostStatus& hs)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Processing HostStatus event...");
#endif /* !NDEBUG */
  try
    {
      this->host_status_stmt_->SetArg(hs);
      ((CentreonBroker::DB::HaveArgs*)this->host_status_stmt_.get())->SetArg(
        this->GetInstanceId(hs.instance));
      ((CentreonBroker::DB::HaveArgs*)this->host_status_stmt_.get())->SetArg(hs.host);
      this->host_status_stmt_->Execute();
    }
  catch (CentreonBroker::DB::DBException& dbe)
    {
      if (dbe.GetReason() != CentreonBroker::DB::DBException::QUERY_EXECUTION)
        throw ;
    }
  if (this->host_status_stmt_->GetUpdateCount() == 0)
    this->ProcessHost(Events::Host(hs));
  return ;
}

/**
 *  Process a Log event.
 */
void Destination::ProcessLog(const Events::Log& log)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Processing Log event...");
#endif /* !NDEBUG */
  std::auto_ptr<CentreonBroker::DB::MappedInsert<Events::Log> >
    query(this->conn_->GetMappedInsert<Events::Log>(log_get_mapping));

  query->SetTable("log");
  query->SetArg(log);
  query->Execute();
  return ;
}

/**
 *  Process a ProgramStatus event.
 */
void Destination::ProcessProgramStatus(const Events::ProgramStatus& ps)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Processing ProgramStatus event...");
#endif /* !NDEBUG */
  try
    {
      this->program_status_stmt_->SetArg(ps);
      ((CentreonBroker::DB::HaveArgs*)(this->program_status_stmt_.get()))->SetArg(
        this->GetInstanceId(ps.instance));
      this->program_status_stmt_->Execute();
    }
  catch (CentreonBroker::DB::DBException& dbe)
    {
      if (dbe.GetReason() != CentreonBroker::DB::DBException::QUERY_EXECUTION)
        throw ;
    }
  if (this->program_status_stmt_->GetUpdateCount() == 0)
    {
      std::auto_ptr<CentreonBroker::DB::MappedInsert<Events::ProgramStatus> >
        query(this->conn_->GetMappedInsert<Events::ProgramStatus>(
          program_status_get_mapping));

      query->SetTable("program_status");
      query->SetArg(ps);
      query->Execute();
    }
  return ;
}

/**
 *  Process a Service event.
 */
void Destination::ProcessService(const Events::Service& service)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Processing Service event...");
#endif /* !NDEBUG */
  // When processing the service definition, we have to fetch the corresponding
  // host_id before inserting it.
  std::auto_ptr<CentreonBroker::DB::Select> query(this->conn_->GetSelect());
  Events::Service myservice(service);

  query->SetTable("host");
  query->AddField("host_id");
  query->SetPredicate(CentreonBroker::DB::And(CentreonBroker::DB::Equal(CentreonBroker::DB::Field("instance_id"),
                                        CentreonBroker::DB::Terminal(this->GetInstanceId(
                                                       myservice.instance))),
                              CentreonBroker::DB::Equal(CentreonBroker::DB::Field("host_name"),
                                        CentreonBroker::DB::Terminal(myservice.host.c_str()))));
  query->Execute();
  if (query->Next())
    myservice.host_id = query->GetInt();
  query.reset();

  try
    {
      this->service_stmt_->SetArg(myservice);
      ((CentreonBroker::DB::HaveArgs*)this->service_stmt_.get())->SetArg(
        this->GetInstanceId(myservice.instance));
      this->service_stmt_->Execute();
    }
  // usually because of a service redefinition
  catch (const CentreonBroker::DB::DBException& dbe)
    {
      if (dbe.GetReason() != CentreonBroker::DB::DBException::QUERY_EXECUTION)
	throw ;
    }
  return ;
}

/**
 *  Process a ServiceGroup event.
 */
void Destination::ProcessServiceGroup(const Events::ServiceGroup& sg)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Processing ServiceGroup event ...");
#endif /* !NDEBUG */
  std::auto_ptr<CentreonBroker::DB::MappedInsert<Events::Group> >
    query(this->conn_->GetMappedInsert<Events::Group>(group_get_mapping));

  query->SetTable("servicegroup");
  query->SetArg(sg);
  try
    {
      query->Execute();
    }
  catch (const CentreonBroker::DB::DBException& dbe)
    {
      if (dbe.GetReason() != CentreonBroker::DB::DBException::QUERY_EXECUTION)
	throw ;
    }
  return ;
}

/**
 *  Process a ServiceStatus event.
 */
void Destination::ProcessServiceStatus(const Events::ServiceStatus& ss)
{
#ifndef NDEBUG
  CentreonBroker::logging.LogDebug("Processing ServiceStatus event...");
#endif /* !NDEBUG */
  try
    {
      this->service_status_stmt_->SetArg(ss);
      ((CentreonBroker::DB::HaveArgs*)this->service_status_stmt_.get())->SetArg(
        this->GetInstanceId(ss.instance));
      ((CentreonBroker::DB::HaveArgs*)this->service_status_stmt_.get())->SetArg(ss.host);
      ((CentreonBroker::DB::HaveArgs*)this->service_status_stmt_.get())->SetArg(ss.service);
      this->service_status_stmt_->Execute();
    }
  catch (CentreonBroker::DB::DBException& dbe)
    {
      if (dbe.GetReason() != CentreonBroker::DB::DBException::QUERY_EXECUTION)
	throw ;
    }
  if (this->service_status_stmt_->GetUpdateCount() == 0)
    this->ProcessService(Events::Service(ss));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Destination default constructor.
 *
 *  Initialize the Destination with default parameters.
 *
 *  \param[in] dbms Type of the database to use.
 */
Destination::Destination()
  : host_status_stmt_(NULL),
    program_status_stmt_(NULL),
    service_status_stmt_(NULL)
{
}

/**
 *  \brief Destination destructor.
 *
 *  Release all previously allocated ressources.
 */
Destination::~Destination()
{
}

/**
 *  Close the event destination.
 */
void Destination::Close()
{
  // XXX
}

/**
 *  \brief Process an event.
 *
 *  When an event is poped from the list, it will be processed by this method.
 *  We will determine the true event type and process it accordingly.
 *
 *  \param[in] event Event that should be stored in the database.
 */
void Destination::Event(Events::Event* event)
{
  try
    {
      switch (event->GetType())
        {
         case Events::Event::ACKNOWLEDGEMENT:
          ProcessAcknowledgement(*static_cast<Events::Acknowledgement*>(event));
          break ;
         case Events::Event::COMMENT:
          ProcessComment(*static_cast<Events::Comment*>(event));
          break ;
         case Events::Event::DOWNTIME:
          ProcessDowntime(*static_cast<Events::Downtime*>(event));
          break ;
         case Events::Event::HOST:
          ProcessHost(*static_cast<Events::Host*>(event));
          break ;
         case Events::Event::HOSTGROUP:
          ProcessHostGroup(*static_cast<Events::HostGroup*>(event));
          break ;
         case Events::Event::HOSTSTATUS:
          ProcessHostStatus(*static_cast<Events::HostStatus*>(event));
          break ;
         case Events::Event::LOG:
          ProcessLog(*static_cast<Events::Log*>(event));
          break ;
         case Events::Event::PROGRAMSTATUS:
          ProcessProgramStatus(*static_cast<Events::ProgramStatus*>(event));
          break ;
         case Events::Event::SERVICE:
          ProcessService(*static_cast<Events::Service*>(event));
          break ;
         case Events::Event::SERVICEGROUP:
          ProcessServiceGroup(*static_cast<Events::ServiceGroup*>(event));
          break ;
         case Events::Event::SERVICESTATUS:
          ProcessServiceStatus(*static_cast<Events::ServiceStatus*>(event));
          break ;
         default:
          assert(false);
          throw (Exception(event->GetType(), "Invalid event type encountered"));
        }
    }
  catch (...) {}

  // Event self deregistration.
  event->RemoveReader();

  return ;
}

/**
 *  \brief Initialize the object.
 *
 *  Set connection parameters and launch the processing thread.
 *
 *  \param[in] conn Already opened DB connection.
 */
void Destination::Init(CentreonBroker::DB::Connection* conn)
{
  this->conn_.reset(conn);
  this->Connect();
  this->PrepareStatements();
  return ;
}
