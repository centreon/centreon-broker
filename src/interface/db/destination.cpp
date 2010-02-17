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

#include <assert.h>
#include <soci.h>
#ifdef USE_MYSQL
# include <soci-mysql.h>
#endif /* USE_MYSQL */
#ifdef USE_ORACLE
# include <soci-oracle.h>
#endif /* USE_ORACLE */
#ifdef USE_POSTGRESQL
# include <soci-postgresql.h>
#endif /* USE_POSTGRESQL */
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
 *  \brief Get the ID of an instance by its name.
 *
 *  The Destination class caches instance IDs as those are used within almost
 *  every table of the schema. This avoids expensive SELECT queries.
 *
 *  \param[in] instance The name of the Nagios instance.
 *
 *  \return The database ID of the Nagios instance.
 */
int Destination::GetInstanceID(const std::string& instance)
{
  int id;
  std::map<std::string, int>::iterator it;

  it = this->instances_.find(instance);
  if (it == this->instances_.end())
    {
    }
  else
    id = it->second;
  return (id);
}

/**
 *  Insert an object in the DB using its mapping.
 */
template <typename T, bool set_instance_id>
void Destination::Insert(const T& t)
{
  std::string query;

  // Build query string.
  query = "INSERT INTO ";
  query.append(MappedType<T>::table);
  query.append("(");
  // XXX : browse mapping
  query.append(") VALUES(");
  // XXX : browse mapping again
  query.append(")");

  {
    soci::details::once_temp_type ott(*this->conn_ << query);

    // Bind object.
    ott, soci::use(t);

    // Query will be executed on ott destruction.
  }

  return ;
}

/**
 *  Process an Acknowledgement event.
 */
void Destination::ProcessAcknowledgement(const Events::Acknowledgement& ack)
{
  LOGDEBUG("Processing Acknowledgement event ...");
  this->Insert<Events::Acknowledgement, true>(ack);
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
          this->Insert<Events::Comment, true>(comment);
        }
      catch (const soci::soci_error& se)
        {
          // XXX : try to update
        }
    }
  else if (comment.type == NEBTYPE_COMMENT_DELETE)
    {
      // XXX : delete
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
      || (downtime.type == NEBTYPE_DOWNTIME_LOAD))
    {
    }
  else if (downtime.type == NEBTYPE_DOWNTIME_START)
    {
    }
  else if ((downtime.type == NEBTYPE_DOWNTIME_STOP)
           || (downtime.type == NEBTYPE_DOWNTIME_DELETE))
    {
    }
  return ;
}

/**
 *  Process an Host event.
 */
void Destination::ProcessHost(const Events::Host& host)
{
  LOGDEBUG("Processing Host event ...");
  this->Insert<Events::Host, true>(host);
  return ;
}

/**
 *  Process an HostCheck event.
 */
void Destination::ProcessHostCheck(const Events::HostCheck& host_check)
{
  LOGDEBUG("Processing HostCheck event ...");
  // XXX : update host
  return ;
}

/**
 *  Process a HostDependency event.
 */
void Destination::ProcessHostDependency(const Events::HostDependency& hd)
{
  LOGDEBUG("Processing HostDependency event ...");

  return ;
}

/**
 *  Process a HostGroup event.
 */
void Destination::ProcessHostGroup(const Events::HostGroup& hg)
{
  LOGDEBUG("Processing HostGroup event...");
  this->Insert<Events::HostGroup, true>(hg);
  return ;
}

/**
 *  Process a HostGroupMember event.
 */
void Destination::ProcessHostGroupMember(const Events::HostGroupMember& hgm)
{
  LOGDEBUG("Processing HostGroupMember event ...");

  return ;
}

/**
 *  Process a HostParent event.
 */
void Destination::ProcessHostParent(const Events::HostParent& hp)
{
  LOGDEBUG("Processing HostParent event ...");

  return ;
}

/**
 *  Process a HostStatus event.
 */
void Destination::ProcessHostStatus(const Events::HostStatus& hs)
{
  LOGDEBUG("Processing HostStatus event ...");
  try
    {
      /* XXX this->PreparedUpdate<Events::HostStatus, true>(hs,
                                                     this->host_status_stmt_,
                                                     this->host_status_);*/
    }
  catch (const soci::soci_error& se)
    {
      this->ProcessHost(hs);
    }
  return ;
}

/**
 *  Process a Log event.
 */
void Destination::ProcessLog(const Events::Log& log)
{
  LOGDEBUG("Processing Log event ...");
  this->Insert<Events::Log, true>(log);
  return ;
}

/**
 *  Process a ProgramStatus event.
 */
void Destination::ProcessProgramStatus(const Events::ProgramStatus& ps)
{
  LOGDEBUG("Processing ProgramStatus event ...");
  try
    {
      /*
      this->PreparedUpdate<Events::ProgramStatus, true>(ps,
        this->program_status_stmt_,
        this->program_status_);
      */
    }
  catch (const soci::soci_error& se)
    {
      this->Insert<Events::ProgramStatus, true>(ps);
    }
  return ;
}

/**
 *  Process a Service event.
 */
void Destination::ProcessService(const Events::Service& service)
{
  LOGDEBUG("Processing Service event ...");
  this->Insert<Events::Service, true>(service);
  return ;
}

/**
 *  Process a ServiceCheck event.
 */
void Destination::ProcessServiceCheck(const Events::ServiceCheck& service_check)
{
  LOGDEBUG("Processing ServiceCheck event ...");
  // XXX : update service
  return ;
}

/**
 *  Process a ServiceDependency event.
 */
void Destination::ProcessServiceDependency(const Events::ServiceDependency& sd)
{
  LOGDEBUG("Processing ServiceDependency event ...");
  // XXX : insert
  return ;
}

/**
 *  Process a ServiceGroup event.
 */
void Destination::ProcessServiceGroup(const Events::ServiceGroup& sg)
{
  LOGDEBUG("Processing ServiceGroup event ...");
  this->Insert<Events::ServiceGroup, true>(sg);
  return ;
}

/**
 *  Process a ServiceGroupMember event.
 */
void Destination::ProcessServiceGroupMember(const Events::ServiceGroupMember& sgm)
{
  LOGDEBUG("Processing ServiceGroupMember event ...");
  // XXX : insert
  return ;
}

/**
 *  Process a ServiceStatus event.
 */
void Destination::ProcessServiceStatus(const Events::ServiceStatus& ss)
{
  LOGDEBUG("Processing ServiceStatus event ...");
  try
    {
      /*
      this->PreparedUpdate<Events::ServiceStatus, true>(ss,
        this->service_status_stmt_,
        this->service_status_);
      */
    }
  catch (const soci::soci_error& se)
    {
      this->ProcessService(ss);
    }
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
  // XXX
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
	ss << "dbname=" << db << " user=" << user << " password=" << pass;
	this->conn_.reset(new soci::session(soci::mysql, ss.str()));
	break ;
#endif /* USE_MYSQL */

#ifdef USE_ORACLE
      case ORACLE:
	break ;
#endif /* USE_ORACLE */

#ifdef USE_POSTGRESQL
      case POSTGRESQL:
	break ;
#endif /* USE_POSTGRESQL */

      default:
	throw Exception(0, "Unsupported DBMS requested.");
      }
  }

  std::vector<int> ids;
  std::vector<std::string> names;
  // XXX : seems like vectors should be sized before being usable in queries ...
  /*
  // Fetch already existing instances from the database.
  (*this->conn_) << "SELECT instance_id, instance_name FROM program_status",
    soci::into(ids), soci::into(names);

  // Insert instances in the cache.
  {
    std::vector<int>::const_iterator end_id, it_id;
    std::vector<std::string>::const_iterator end_name, it_name;

    end_id = ids.end();
    end_name = names.end();
    for (it_id = ids.begin(), it_name = names.begin();
         (it_id != end_id) && (it_name != end_name);
         ++it_id, ++it_name)
      this->instances_[*it_name] = *it_id;
      }*/

  return ;
}
