/*
** db_output.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/17/09 Matthieu Kermagoret
*/

#ifndef DB_OUTPUT_H_
# define DB_OUTPUT_H_

# include <boost/thread/thread_time.hpp>
# include <string>
# include <vector>
# include "db/connection.h"
# include "db/mapping.hpp"
# include "db/update.hpp"
# include "event_subscriber.h"
# include "mapping.h"
# include "waitable_list.hpp"

namespace                      boost
{
  class                        thread;
}

namespace                      CentreonBroker
{
  namespace                    DB
  {
    class                      Connection;
    template                   <typename ObjectType>
    class                      Update;
  }
  class                        Acknowledgement;
  class                        Comment;
  class                        Event;
  class                        Host;
  class                        HostStatus;
  class                        ProgramStatus;
  class                        Query;
  class                        Service;
  class                        ServiceStatus;

  class                           DBOutput : private EventSubscriber
  {
   private:
    // Object-Relational mappings
    DB::Mapping<Acknowledgement>  acknowledgement_mapping_;
    DB::Mapping<Connection>       connection_mapping_;
    DB::Mapping<ConnectionStatus> connection_status_mapping_;
    DB::Mapping<Host>             host_mapping_;
    DB::Mapping<HostStatus>       host_status_mapping_;
    DB::Mapping<ProgramStatus>    program_status_mapping_;
    DB::Mapping<Service>          service_mapping_;
    DB::Mapping<ServiceStatus>    service_status_mapping_;
    // Connection informations
    DB::Connection::DBMS          dbms_;
    std::string                   host_;
    std::string                   user_;
    std::string                   password_;
    std::string                   db_;
    DB::Connection*               conn_;
    DB::Update<ConnectionStatus>* connection_status_stmt_;
    DB::Update<HostStatus>*       host_status_stmt_;
    DB::Update<ProgramStatus>*    program_status_stmt_;
    DB::Update<ServiceStatus>*    service_status_stmt_;
    // Performance objects
    int                           queries_;
    boost::system_time            timeout_;
    // Events
    WaitableList<Event>           events_;
    std::map<std::string, int>    instances_;
    // Thread
    volatile bool                 exit_;
    boost::thread*                thread_;

                                  DBOutput(const DBOutput& dbo);
    DBOutput&                     operator=(const DBOutput& dbo);
    void                          CleanTable(const std::string& table);
    void                          Commit();
    void                          Connect();
    void                          Disconnect();
    int                           GetInstanceId(const Event& event);
    void                          OnEvent(Event* e) throw ();
    void                          ProcessAcknowledgement(
                                    const Acknowledgement& ack);
    void                          ProcessComment(const Comment& comment);
    void                          ProcessConnection(
                                    const Connection& connection);
    void                          ProcessConnectionStatus(
                                    const ConnectionStatus& cs);
    void                          ProcessEvent(Event* event);
    void                          ProcessHost(const Host& host);
    void                          ProcessHostStatus(const HostStatus& hs);
    void                          ProcessProgramStatus(
                                    const ProgramStatus& ps);
    void                          ProcessService(const Service& service);
    void                          ProcessServiceStatus(
                                    const ServiceStatus& ss);
    void                          QueryExecuted();

   public:
                                  DBOutput(DB::Connection::DBMS dbms);
                                  ~DBOutput();
    void                          operator()();
    void                          Destroy();
    void                          Init(const std::string& host,
                                       const std::string& user,
                                       const std::string& password,
                                       const std::string& db);
  };
}

#endif /* !DB_OUTPUT_H_ */
