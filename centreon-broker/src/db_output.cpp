/*
** db_output.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/03/09 Matthieu Kermagoret
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
// XXX : remove
#include <mysql_public_iface.h>
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
  // Prepare queries
  {
    UpdateQuery* uq;

    uq = this->conn_->GetUpdateQuery();
    uq->SetTable("hosts");
    uq->AddFields(host_status_fields);
    uq->AddUniques(host_status_uniques);
    uq->Prepare();
    this->stmts_.push_back(uq);
  }
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
      e->AddReader(this);
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
  for (unsigned int i = 0; host_getters[i].type_; i++)
    switch (host_getters[i].type_)
      {
       case 'd':
        query->SetDouble(i + 1,
                         (host.*host_getters[i].getter_.get_double)());
        break ;
       case 'i':
        query->SetInt(i + 1,
                      (host.*host_getters[i].getter_.get_int)());
        break ;
       case 's':
        query->SetShort(i + 1,
                        (host.*host_getters[i].getter_.get_short)());
        break ;
       case 'S':
        query->SetString(i + 1,
                         (host.*host_getters[i].getter_.get_string)());
        break ;
       case 't':
        query->SetTimeT(i + 1,
                        (host.*host_getters[i].getter_.get_timet)());
        break ;
       default:
        // XXX : some error message
        assert(false);
      }
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

  uq = this->stmts_[0];
  for (count = 0; host_status_getters[count].type_; count++)
    switch (host_status_getters[count].type_)
      {
       case 'd':
        uq->SetDouble(count + 1,
                      (hs.*host_status_getters[count].getter_.get_double)());
        break ;
       case 'i':
        uq->SetInt(count + 1,
                   (hs.*host_status_getters[count].getter_.get_int)());
        break ;
       case 's':
        uq->SetShort(count + 1,
                     (hs.*host_status_getters[count].getter_.get_short)());
        break ;
       case 'S':
        uq->SetString(count + 1,
                      (hs.*host_status_getters[count].getter_.get_string)());
        break ;
       case 't':
        uq->SetTimeT(count + 1,
                     (hs.*host_status_getters[count].getter_.get_timet)());
        break ;
       default:
        // XXX : some error message
        assert(false);
      }
  for (unsigned int i = 0; host_status_uniques[i]; i++)
    if (!strcmp("host_name", host_status_uniques[i]))
      uq->SetString(++count, hs.GetHostName());
    else
      assert(false);
  try
    {
      this->ExecuteQuery(uq);
    }
  catch (sql::SQLException& e)
    {
      std::cerr << "SQL: " << e.what() << std::endl;
      this->ProcessHost(Host(hs));
    }
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
      catch (sql::SQLException& e)
        {
          std::cerr << "Recoverable MySQL error" << std::endl
                    << "    " << e.what() << std::endl;
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
