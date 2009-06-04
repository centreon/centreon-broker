/*
** db_output.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/04/09 Matthieu Kermagoret
*/

#include <memory>
#include "db/connection.h"
#include "db/mysql_connection.h"
#include "db/update_query.h"
#include "db_output.h"
#include "event.h"
#include "exception.h"
#include "host.h"
#include "host_status.h"
#include "mapping.h"

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
     case MYSQL:
       this->conn_ = new MySQLConnection;
       break;
     default:
       throw (Exception(0, "Unsupported DBMS requested."));
    }
  this->conn_->Connect(this->host_,
                       this->user_,
                       this->password_,
                       this->db_);
  // XXX : truncate tables
  // Prepare HostStatus update query
  {
    UpdateQuery* uq;

    uq = this->conn_->GetUpdateQuery();
    uq->SetTable("hosts");
    uq->AddFields(host_status_fields);
    uq->AddUniques(host_status_uniques);
    uq->Prepare();
    this->stmts_.push_back(uq);
  }
  {
    UpdateQuery* uq;

    uq = this->conn_->GetUpdateQuery();
    uq->SetTable("services");
    uq->AddFields(service_status_fields);
    uq->AddUniques(service_status_uniques);
    uq->Prepare();
    this->stmts_.push_back(uq);
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
  for (std::vector<UpdateQuery*>::iterator it = this->stmts_.begin();
       it != this->stmts_.end();
       it++)
    delete (*it);
  if (this->conn_)
    {
      delete (this->conn_);
      this->conn_ = NULL;
    }
  return ;
}

/**
 *  Execute a query.
 */
void DBOutput::ExecuteQuery(Query* query)
{
  query->Execute();
  // XXX : query number should be configurable
  if (++this->queries_ >= 10000)
    this->Commit();
  return ;
}

/**
 *  Get an instance id.
 */
int DBOutput::GetInstanceId(const std::string& instance)
{
  int id;
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
  std::auto_ptr<Query> query(this->conn_->GetInsertQuery());

  query->SetTable("hosts");
  query->AddFields(host_fields);
  query->Prepare();
  this->SetFields<Host>(*query.get(), host, host_getters);
  this->ExecuteQuery(query.get());
  this->Commit();
  return ;
}

/**
 *  Process an HostStatus event.
 */
void DBOutput::ProcessHostStatus(const HostStatus& hs)
{
  int count;
  UpdateQuery* uq;

  // XXX : hardcoded value
  uq = this->stmts_[0];
  count = this->SetFields<HostStatus>(*uq, hs, host_status_getters);
  for (unsigned int i = 0; host_status_uniques[i]; i++)
    if (!strcmp("host_name", host_status_uniques[i]))
      uq->SetString(count++, hs.GetHostName().c_str());
    else
      assert(false);
  try
    {
      this->ExecuteQuery(uq);
    }
  catch (...) // XXX : more precise
    {
      this->ProcessHost(Host(hs));
    }
  return ;
}

/**
 *  Process a Service event.
 */
void DBOutput::ProcessService(const Service& service)
{
  std::auto_ptr<Query> query(this->conn_->GetInsertQuery());

  query->SetTable("services");
  query->AddFields(service_fields);
  query->Prepare();
  this->SetFields<Service>(*query.get(), service, service_getters);
  this->ExecuteQuery(query.get());
  this->Commit();
  return ;
}

/**
 *  Process a ServiceStatus event.
 */
void DBOutput::ProcessServiceStatus(const ServiceStatus& ss)
{
  int count;
  UpdateQuery* uq;

  // XXX : hardcoded value
  uq = this->stmts_[1];
  count = this->SetFields<ServiceStatus>(*uq, ss, service_status_getters);
  for (unsigned int i = 0; service_status_uniques[i]; i++)
    if (!strcmp("host_name", service_status_uniques[i]))
      uq->SetString(count++, ss.GetHostName().c_str());
    else if (!strcmp("service_description", service_status_uniques[i]))
      uq->SetString(count++, ss.GetServiceDescription().c_str());
    else
      assert(false);
  try
    {
      this->ExecuteQuery(uq);
    }
  catch (...) // XXX : more precise
    {
      this->ProcessService(Service(ss));
    }
  return ;
}

/**
 *  Insert an object in the database.
 */
template <typename ObjectType>
unsigned int DBOutput::SetFields(Query& query,
                                 const ObjectType& obj,
			         const FieldGetter<ObjectType>* getters)
{
  unsigned int i;

  for (i = 0; getters[i].type_; i++)
    switch (getters[i].type_)
      {
       case 'd':
        query.SetDouble(i,
                         (obj.*getters[i].getter_.get_double)());
        break ;
       case 'i':
        query.SetInt(i,
                      (obj.*getters[i].getter_.get_int)());
        break ;
       case 's':
        query.SetShort(i,
                        (obj.*getters[i].getter_.get_short)());
        break ;
       case 'S':
        query.SetString(i,
                         (obj.*getters[i].getter_.get_string)().c_str());
        break ;
       case 't':
        query.SetTimeT(i,
                        (obj.*getters[i].getter_.get_timet)());
        break ;
       default:
        // XXX : some error message
        assert(false);
      }
  return (i);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  DBOutput default constructor.
 */
DBOutput::DBOutput()
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
void DBOutput::Init(DBOutput::DBMS dbms,
                    const std::string& host,
                    const std::string& user,
                    const std::string& password,
                    const std::string& db)
{
  this->dbms_ = dbms;
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
