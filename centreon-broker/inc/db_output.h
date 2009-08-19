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

#ifndef DB_OUTPUT_H_
# define DB_OUTPUT_H_

# include <boost/thread.hpp>
# include <string>
# include <vector>
# include "db/connection.h"
# include "db/mapping.hpp"
# include "db/update.h"
# include "event_subscriber.h"
# include "mapping.h"
# include "waitable_list.hpp"

namespace                       CentreonBroker
{
  namespace                     Events
  {
    class                       Acknowledgement;
    class                       Comment;
    class                       Connection;
    class                       ConnectionStatus;
    class                       Downtime;
    class                       Event;
    class                       Host;
    class                       HostGroup;
    class                       HostStatus;
    class                       ProgramStatus;
    class                       Query;
    class                       Service;
    class                       ServiceStatus;
  }

  /**
   *  \class DBOutput db_output.h "db_output.h"
   *  \brief Insert/update events in a database.
   *
   *  DBOutput is the main responsible of the insertion/update of events within
   *  a database. It uses abstract database objects to handle different kind of
   *  DBMS.
   */
  class                         DBOutput : private EventSubscriber
  {
   private:
    // Connection informations
    DB::Connection::DBMS        dbms_;
    std::string                 host_;
    std::string                 user_;
    std::string                 password_;
    std::string                 db_;
    DB::Connection*             conn_;
    DB::MappedUpdate<Events::ConnectionStatus>*
                                connection_status_stmt_;
    DB::MappedUpdate<Events::HostStatus>*
                                host_status_stmt_;
    DB::MappedUpdate<Events::ProgramStatus>*
                                program_status_stmt_;
    DB::MappedUpdate<Events::ServiceStatus>*
                                service_status_stmt_;
    // Performance objects
    int                         queries_;
    boost::system_time          timeout_;
    // Events
    WaitableList<Events::Event> events_;
    std::map<std::string, int>  instances_;
    // Thread
    volatile bool               exit_;
    boost::thread*              thread_;

                                DBOutput(const DBOutput& dbo);
    DBOutput&                   operator=(const DBOutput& dbo);
    void                        Commit();
    void                        Connect();
    void                        Disconnect();
    int                         GetInstanceId(const std::string& instance);
    void                        OnEvent(Events::Event* e) throw ();
    void                        PrepareMappings();
    void                        PrepareStatements();
    void                        ProcessAcknowledgement(
                                  const Events::Acknowledgement& ack);
    void                        ProcessComment(const Events::Comment& comment);
    void                        ProcessConnection(
                                  const Events::Connection& connection);
    void                        ProcessConnectionStatus(
                                  const Events::ConnectionStatus& cs);
    void                        ProcessDowntime(const Events::Downtime& downtime);
    void                        ProcessEvent(Events::Event* event);
    void                        ProcessHost(const Events::Host& host);
    void                        ProcessHostGroup(const Events::HostGroup& hg);
    void                        ProcessHostStatus(const Events::HostStatus& hs);
    void                        ProcessLog(const Events::Log& log);
    void                        ProcessProgramStatus(
                                  const Events::ProgramStatus& ps);
    void                        ProcessService(const Events::Service& service);
    void                        ProcessServiceStatus(
                                  const Events::ServiceStatus& ss);
    void                        QueryExecuted();

   public:
                                DBOutput(DB::Connection::DBMS dbms);
                                ~DBOutput();
    void                        operator()();
    void                        Destroy();
    void                        Init(const std::string& host,
                                     const std::string& user,
                                     const std::string& password,
                                     const std::string& db);
  };
}

#endif /* !DB_OUTPUT_H_ */
