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

#include <algorithm>                  // for find
#include <assert.h>
#include <soci.h>
#ifdef USE_MYSQL
# include <soci-mysql.h>
#endif /* USE_MYSQL */
#include <sstream>
#include <stdlib.h>                   // for abort
#include "events/events.h"
#include "exception.h"
#include "interface/db/destination.h"
#include "interface/db/internal.h"
#include "logging.h"
#include "mapping.h"
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
 *  Clean tables with data associated to the instance.
 */
void Destination::CleanTables(int instance_id)
{
  // ProgramStatus
  *this->conn_ << "DELETE FROM " << MappedType<Events::ProgramStatus>::table
               << " WHERE instance_id=" << instance_id;

  return ;
}

/**
 *  Insert an object in the DB using its mapping.
 */
template <typename T>
void Destination::Insert(const T& t)
{
  std::string query;

  // Build query string.
  query = "INSERT INTO ";
  query.append(MappedType<T>::table);
  query.append("(");
  for (typename std::map<std::string, GetterSetter<T> >::const_iterator
         it = DBMappedType<T>::map.begin(),
         end = DBMappedType<T>::map.end();
       it != end;
       ++it)
    {
      query.append(it->first);
      query.append(", ");
    }
  query.resize(query.size() - 2);
  query.append(") VALUES(");
  for (typename std::map<std::string, GetterSetter<T> >::const_iterator
         it = DBMappedType<T>::map.begin(),
         end = DBMappedType<T>::map.end();
       it != end;
       ++it)
    {
      query.append(":");
      query.append(it->first);
      query.append(", ");
    }
  query.resize(query.size() - 2);
  query.append(")");
  LOGDEBUG(query.c_str());

  // Execute query.
  *this->conn_ << query, soci::use(t);

  return ;
}

/**
 *  Update an object in the DB using its prepared statement.
 */
template <typename T>
void Destination::PreparedUpdate(const T& t,
                                 soci::statement& st,
                                 T& tmp)
{
  tmp = t;
  st.execute(true);
  return ;
}

/**
 *  Prepare an update statement for later execution.
 */
template <typename T>
void Destination::PrepareUpdate(std::auto_ptr<soci::statement>& st,
                                T& t,
                                const std::vector<std::string>& id)
{
  std::string query;

  // Build query string.
  query = "UPDATE ";
  query.append(MappedType<T>::table);
  query.append(" SET ");
  for (typename std::map<std::string, GetterSetter<T> >::const_iterator
         it = DBMappedType<T>::map.begin(),
         end = DBMappedType<T>::map.end();
       it != end;
       ++it)
    if (std::find(id.begin(), id.end(), it->first) == id.end())
      {
        query.append(it->first);
        query.append("=:");
        query.append(it->first);
        query.append(", ");
      }
  query.resize(query.size() - 2);
  query.append(" WHERE ");
  for (std::vector<std::string>::const_iterator
         it = id.begin(),
         end = id.end();
       it != end;
       ++it)
    {
      query.append(*it);
      query.append("=:");
      query.append(*it);
      query.append(" AND ");
    }
  query.resize(query.size() - 5);
  LOGDEBUG(query.c_str());

  // Prepare statement.
  st.reset(new soci::statement((this->conn_->prepare << query, soci::use(t))));

  return ;
}

/**
 *  Process an Acknowledgement event.
 */
void Destination::ProcessAcknowledgement(const Events::Acknowledgement& ack)
{
  LOGDEBUG("Processing Acknowledgement event ...");
  try
    {
      this->Insert(ack);
    }
  catch (const soci::soci_error& se)
    {
      this->PreparedUpdate(ack,
                           *this->acknowledgement_stmt_,
                           this->acknowledgement_);
    }
  return ;
}

/**
 *  Process a Comment event.
 */
void Destination::ProcessComment(const Events::Comment& comment)
{
  LOGDEBUG("Processing Comment event ...");
  if ((comment.type == NEBTYPE_COMMENT_ADD)
      || comment.type == NEBTYPE_COMMENT_LOAD)
    {
      try
        {
          this->Insert(comment);
        }
      catch (const soci::soci_error& se)
        {
          this->PreparedUpdate(comment,
                               *this->comment_stmt_,
                               this->comment_);
        }
    }
  else if (comment.type == NEBTYPE_COMMENT_DELETE)
    {
      *this->conn_ << "DELETE FROM " << MappedType<Events::Comment>::table
                   << " WHERE internal_id=" << comment.internal_id;
    }
  return ;
}

/**
 *  Process a Downtime event.
 */
void Destination::ProcessDowntime(const Events::Downtime& downtime)
{
  LOGDEBUG("Processing Downtime event ...");

  if ((downtime.type == NEBTYPE_DOWNTIME_ADD)
      || (downtime.type == NEBTYPE_DOWNTIME_LOAD)
      || (downtime.type == NEBTYPE_DOWNTIME_START))
    {
      try
        {
          this->Insert(downtime);
        }
      catch (const soci::soci_error& se)
        {
          this->PreparedUpdate(downtime,
                               *this->downtime_stmt_,
                               this->downtime_);
        }
    }
  else if ((downtime.type == NEBTYPE_DOWNTIME_STOP)
           || (downtime.type == NEBTYPE_DOWNTIME_DELETE))
    {
      *this->conn_ << "DELETE FROM " << MappedType<Events::Downtime>::table
                   << " WHERE downtime_id=" << downtime.id;
    }
  return ;
}

/**
 *  Process an Host event.
 */
void Destination::ProcessHost(const Events::Host& host)
{
  LOGDEBUG("Processing Host event ...");
  this->Insert(host);
  return ;
}

/**
 *  Process an HostCheck event.
 */
void Destination::ProcessHostCheck(const Events::HostCheck& host_check)
{
  LOGDEBUG("Processing HostCheck event ...");
  this->PreparedUpdate(host_check,
                       *this->host_check_stmt_,
                       this->host_check_);
  return ;
}

/**
 *  Process a HostDependency event.
 */
void Destination::ProcessHostDependency(const Events::HostDependency& hd)
{
  LOGDEBUG("Processing HostDependency event ...");
  this->Insert(hd);
  return ;
}

/**
 *  Process a HostGroup event.
 */
void Destination::ProcessHostGroup(const Events::HostGroup& hg)
{
  LOGDEBUG("Processing HostGroup event...");
  this->Insert(hg);
  return ;
}

/**
 *  Process a HostGroupMember event.
 */
void Destination::ProcessHostGroupMember(const Events::HostGroupMember& hgm)
{
  int hostgroup_id;

  LOGDEBUG("Processing HostGroupMember event ...");

  // Fetch host group ID.
  *this->conn_ << "SELECT id FROM "
               << MappedType<Events::HostGroup>::table
               << " WHERE instance_id=" << hgm.instance
               << " AND hostgroup_name=\"" << hgm.group << "\"",
    soci::into(hostgroup_id);

  // Execute query.
  *this->conn_ << "INSERT INTO "
               << MappedType<Events::HostGroupMember>::table
               << "(host, hostgroup) VALUES("
               << hgm.member << ", "
               << hostgroup_id << ")";

  return ;
}

/**
 *  Process a HostParent event.
 */
void Destination::ProcessHostParent(const Events::HostParent& hp)
{
  LOGDEBUG("Processing HostParent event ...");
  this->Insert(hp);
  return ;
}

/**
 *  Process a HostStatus event.
 */
void Destination::ProcessHostStatus(const Events::HostStatus& hs)
{
  LOGDEBUG("Processing HostStatus event ...");
  this->PreparedUpdate<Events::HostStatus>(hs,
                                           *this->host_status_stmt_,
                                           this->host_status_);
  return ;
}

/**
 *  Process a Log event.
 */
void Destination::ProcessLog(const Events::Log& log)
{
  LOGDEBUG("Processing Log event ...");
  this->Insert(log);
  return ;
}

/**
 *  Process a ProgramStatus event.
 */
void Destination::ProcessProgramStatus(const Events::ProgramStatus& ps)
{
  LOGDEBUG("Processing ProgramStatus event ...");
  std::map<int, time_t>::iterator it;

  if (ps.program_end)
    {
      this->CleanTables(ps.instance);
      it = this->instances_.find(ps.instance);
      if (it != this->instances_.end())
        this->instances_.erase(it);
    }
  else if (((it = this->instances_.find(ps.instance))
	    != this->instances_.end())
	   && (it->second == ps.program_start))
    {
      this->PreparedUpdate<Events::ProgramStatus>(ps,
        *this->program_status_stmt_,
        this->program_status_);
    }
  else
    {
      this->CleanTables(ps.instance);
      this->Insert(ps);
      this->instances_[ps.instance] = ps.program_start;
    }
  return ;
}

/**
 *  Process a Service event.
 */
void Destination::ProcessService(const Events::Service& service)
{
  LOGDEBUG("Processing Service event ...");
  this->Insert(service);
  return ;
}

/**
 *  Process a ServiceCheck event.
 */
void Destination::ProcessServiceCheck(const Events::ServiceCheck& service_check)
{
  LOGDEBUG("Processing ServiceCheck event ...");
  this->PreparedUpdate(service_check,
                       *this->service_check_stmt_,
                       this->service_check_);
  return ;
}

/**
 *  Process a ServiceDependency event.
 */
void Destination::ProcessServiceDependency(const Events::ServiceDependency& sd)
{
  LOGDEBUG("Processing ServiceDependency event ...");
  this->Insert(sd);
  return ;
}

/**
 *  Process a ServiceGroup event.
 */
void Destination::ProcessServiceGroup(const Events::ServiceGroup& sg)
{
  LOGDEBUG("Processing ServiceGroup event ...");
  this->Insert(sg);
  return ;
}

/**
 *  Process a ServiceGroupMember event.
 */
void Destination::ProcessServiceGroupMember(const Events::ServiceGroupMember& sgm)
{
  int servicegroup_id;

  LOGDEBUG("Processing ServiceGroupMember event ...");

  // Fetch service group ID.
  *this->conn_ << "SELECT id FROM "
               << MappedType<Events::ServiceGroup>::table
               << " WHERE instance_id=" << sgm.instance
               << " AND servicegroup_name=\"" << sgm.group << "\"",
    soci::into(servicegroup_id);

  // Execute query.
  *this->conn_ << "INSERT INTO "
               << MappedType<Events::ServiceGroupMember>::table
               << "(service, servicegroup) VALUES("
               << sgm.member << ", "
               << servicegroup_id << ")";

  return ;
}

/**
 *  Process a ServiceStatus event.
 */
void Destination::ProcessServiceStatus(const Events::ServiceStatus& ss)
{
  LOGDEBUG("Processing ServiceStatus event ...");
  this->PreparedUpdate<Events::ServiceStatus>(ss,
                                              *this->service_status_stmt_,
                                              this->service_status_);
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
Destination::Destination() {}

/**
 *  \brief Destination destructor.
 *
 *  Release all previously allocated ressources.
 */
Destination::~Destination()
{
  this->Close();
}

/**
 *  Close the event destination.
 */
void Destination::Close()
{
  this->acknowledgement_stmt_.reset();
  this->comment_stmt_.reset();
  this->downtime_stmt_.reset();
  this->host_check_stmt_.reset();
  this->host_status_stmt_.reset();
  this->program_status_stmt_.reset();
  this->service_check_stmt_.reset();
  this->service_status_stmt_.reset();
  this->conn_.reset();
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
         case Events::Event::HOSTCHECK:
          ProcessHostCheck(*static_cast<Events::HostCheck*>(event));
          break ;
         case Events::Event::HOSTDEPENDENCY:
          ProcessHostDependency(*static_cast<Events::HostDependency*>(event));
          break ;
         case Events::Event::HOSTGROUP:
          ProcessHostGroup(*static_cast<Events::HostGroup*>(event));
          break ;
         case Events::Event::HOSTGROUPMEMBER:
          ProcessHostGroupMember(*static_cast<Events::HostGroupMember*>(event));
          break ;
         case Events::Event::HOSTPARENT:
          ProcessHostParent(*static_cast<Events::HostParent*>(event));
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
         case Events::Event::SERVICECHECK:
          ProcessServiceCheck(*static_cast<Events::ServiceCheck*>(event));
          break ;
         case Events::Event::SERVICEDEPENDENCY:
          ProcessServiceDependency(*static_cast<Events::ServiceDependency*>(event));
          break ;
         case Events::Event::SERVICEGROUP:
          ProcessServiceGroup(*static_cast<Events::ServiceGroup*>(event));
          break ;
         case Events::Event::SERVICEGROUPMEMBER:
          ProcessServiceGroupMember(*static_cast<Events::ServiceGroupMember*>(event));
          break ;
         case Events::Event::SERVICESTATUS:
          ProcessServiceStatus(*static_cast<Events::ServiceStatus*>(event));
          break ;
         default: // Discard event.
          LOGINFO("Invalid event type encountered.");
        }
    }
  catch (...)
    {
      // Event self deregistration.
      event->RemoveReader();

      // Rethrow the exception
      throw ;
    }

  // Event self deregistration.
  event->RemoveReader();

  return ;
}

/**
 *  \brief Connect the database destination.
 *
 *  Connect to the specified database using the specified credentials.
 *
 *  \param[in] db_type Database type.
 *  \param[in] host    DB server.
 *  \param[in] db      Database
 *  \param[in] user    User name to use for authentication.
 *  \param[in] pass    Password to use for authentication.
 */
void Destination::Connect(Destination::DB db_type,
                          const std::string& db,
                          const std::string& host,
                          const std::string& user,
                          const std::string& pass)
{
  // Connect to DB.
  {
    std::stringstream ss;

    switch (db_type)
      {
#ifdef USE_MYSQL
       case MYSQL:
        ss << "dbname=" << db
           << " host=" << host
           << " user=" << user
           << " password=" << pass;
        this->conn_.reset(new soci::session(soci::mysql, ss.str()));
        break ;
#endif /* USE_MYSQL */

      default:
        throw Exception(0, "Unsupported DBMS requested.");
      }
  }

  std::vector<std::string> id;

  id.clear();
  id.push_back("author_name");
  id.push_back("entry_time");
  id.push_back("host_id");
  id.push_back("service_id");
  this->PrepareUpdate(this->acknowledgement_stmt_,
                      this->acknowledgement_,
                      id);

  id.clear();
  id.push_back("internal_id");
  this->PrepareUpdate(this->comment_stmt_,
                      this->comment_,
                      id);

  id.clear();
  id.push_back("downtime_id");
  this->PrepareUpdate(this->downtime_stmt_,
                      this->downtime_,
                      id);

  id.clear();
  id.push_back("host_id");
  this->PrepareUpdate(this->host_check_stmt_,
                      this->host_check_,
                      id);

  id.clear();
  id.push_back("host_id");
  this->PrepareUpdate(this->host_status_stmt_,
                      this->host_status_,
                      id);

  id.clear();
  id.push_back("instance_name");
  this->PrepareUpdate(this->program_status_stmt_,
                      this->program_status_,
                      id);

  id.clear();
  id.push_back("service_id");
  this->PrepareUpdate(this->service_check_stmt_,
                      this->service_check_,
                      id);

  id.clear();
  id.push_back("service_id");
  this->PrepareUpdate(this->service_status_stmt_,
                      this->service_status_,
                      id);

  return ;
}
