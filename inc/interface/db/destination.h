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

#ifndef INTERFACE_DB_DESTINATION_H_
# define INTERFACE_DB_DESTINATION_H_

# include <map>
# include <string>
# include <soci.h>
# include "events/events.h"
# include "interface/destination.h"

namespace                         Interface
{
  namespace                       DB
  {
    /**
     *  \class Destination destination.h "interface/db/destination.h"
     *  \brief Insert/update events in a database.
     *
     *  Handle insertion of events in their generic form into a database.
     */
    class                         Destination : public Interface::Destination
    {
     public:
      enum                        DB
      {
        UNKNOWN = 0,
        MYSQL,
        ORACLE,
        POSTGRESQL
      };

     private:
      std::auto_ptr<soci::statement> host_check_stmt_;
      std::auto_ptr<soci::statement> host_status_stmt_;
      std::auto_ptr<soci::statement> program_status_stmt_;
      std::auto_ptr<soci::statement> service_check_stmt_;
      std::auto_ptr<soci::statement> service_status_stmt_;
      std::auto_ptr<soci::session> conn_; // Connection object is necessary after statements.
      std::map<std::string, int>  instances_;
      Events::HostCheck           host_check_;
      Events::HostStatus          host_status_;
      Events::ProgramStatus       program_status_;
      Events::ServiceCheck        service_check_;
      Events::ServiceStatus       service_status_;
                                  Destination(const Destination& destination);
      Destination&                operator=(const Destination& destination);
      int                         GetInstanceID(const std::string& instance);
      template                    <typename T>
      void                        Insert(const T& t);
      template                    <typename T>
      void                        PreparedUpdate(const T&t, soci::statement& st, T& tmp);
      template                    <typename T>
      void                        PrepareUpdate(std::auto_ptr<soci::statement>& st,
                                                T& t,
                                                const std::vector<std::string>& id);
      void                        ProcessAcknowledgement(
                                    const Events::Acknowledgement& ack);
      void                        ProcessComment(const Events::Comment& comment);
      void                        ProcessDowntime(const Events::Downtime& downtime);
      void                        ProcessHost(const Events::Host& host);
      void                        ProcessHostCheck(const Events::HostCheck& host_check);
      void                        ProcessHostDependency(const Events::HostDependency& hd);
      void                        ProcessHostGroup(const Events::HostGroup& hg);
      void                        ProcessHostGroupMember(const Events::HostGroupMember& hgm);
      void                        ProcessHostParent(const Events::HostParent& hp);
      void                        ProcessHostStatus(const Events::HostStatus& hs);
      void                        ProcessLog(const Events::Log& log);
      void                        ProcessProgramStatus(
                                    const Events::ProgramStatus& ps);
      void                        ProcessService(const Events::Service& service);
      void                        ProcessServiceCheck(const Events::ServiceCheck& service_check);
      void                        ProcessServiceDependency(const Events::ServiceDependency& sd);
      void                        ProcessServiceGroup(
                                    const Events::ServiceGroup& sg);
      void                        ProcessServiceGroupMember(const Events::ServiceGroupMember& sgm);
      void                        ProcessServiceStatus(
                                    const Events::ServiceStatus& ss);

     public:
                                  Destination();
                                  ~Destination();
      void                        Close();
      void                        Event(Events::Event* event);
      void                        Connect(DB db_type,
                                          const std::string& db,
                                          const std::string& host,
                                          const std::string& user,
                                          const std::string& pass);
    };
  }
}

#endif /* !INTERFACE_DB_DESTINATION_H_ */
