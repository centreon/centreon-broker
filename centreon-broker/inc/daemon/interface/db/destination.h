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

# include <memory>
# include <string>
# include <vector>
# include "db/connection.h"
# include "db/mapping.hpp"
# include "db/update.h"
# include "interface/destination.h"
# include "mapping.h"

// Forward declaration.
namespace                         Events
{
  class                           Acknowledgement;
  class                           Comment;
  class                           Downtime;
  class                           Event;
  class                           Host;
  class                           HostGroup;
  class                           HostStatus;
  class                           ProgramStatus;
  class                           Query;
  class                           Service;
  class                           ServiceGroup;
  class                           ServiceStatus;
}

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
     private:
      std::auto_ptr<CentreonBroker::DB::Connection>
                                  conn_;
      std::auto_ptr<CentreonBroker::DB::MappedInsert<Events::Host> >
                                  host_stmt_;
      std::auto_ptr<CentreonBroker::DB::MappedUpdate<Events::HostStatus> >
                                  host_status_stmt_;
      std::auto_ptr<CentreonBroker::DB::MappedUpdate<Events::ProgramStatus> >
                                  program_status_stmt_;
      std::auto_ptr<CentreonBroker::DB::MappedInsert<Events::Service> >
                                  service_stmt_;
      std::auto_ptr<CentreonBroker::DB::MappedUpdate<Events::ServiceStatus> >
                                  service_status_stmt_;
      std::map<std::string, int>  instances_;
                                  Destination(const Destination& destination);
      Destination&                operator=(const Destination& destination);
      void                        Connect();
      void                        Disconnect();
      int                         GetInstanceId(const std::string& instance);
      void                        PrepareMappings();
      void                        PrepareStatements();
      void                        ProcessAcknowledgement(
                                    const Events::Acknowledgement& ack);
      void                        ProcessComment(const Events::Comment& comment);
      void                        ProcessDowntime(const Events::Downtime& downtime);
      void                        ProcessHost(const Events::Host& host);
      void                        ProcessHostGroup(const Events::HostGroup& hg);
      void                        ProcessHostStatus(const Events::HostStatus& hs);
      void                        ProcessLog(const Events::Log& log);
      void                        ProcessProgramStatus(
                                    const Events::ProgramStatus& ps);
      void                        ProcessService(const Events::Service& service);
      void                        ProcessServiceGroup(
                                    const Events::ServiceGroup& sg);
      void                        ProcessServiceStatus(
                                    const Events::ServiceStatus& ss);

     public:
                                  Destination();
                                  ~Destination();
      void                        Close();
      void                        Event(Events::Event* event);
      void                        Init(CentreonBroker::DB::Connection* conn);
    };
  }
}

#endif /* !INTERFACE_DB_DESTINATION_H_ */
