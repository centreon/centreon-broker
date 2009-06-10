/*
** db_output.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/10/09 Matthieu Kermagoret
*/

#ifndef DB_OUTPUT_H_
# define DB_OUTPUT_H_

# include <boost/thread/thread_time.hpp>
# include <string>
# include <vector>
# include "db/connection.h"
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
  class                        Event;
  class                        Host;
  class                        HostStatus;
  class                        Query;

  class                        DBOutput : private EventSubscriber
  {
   private:
    DB::Connection::DBMS       dbms_;
    std::string                host_;
    std::string                user_;
    std::string                password_;
    std::string                db_;
    DB::Connection*            conn_;
    DB::Update<HostStatus>*    host_status_stmt_;
    // Performance objects
    int                        queries_;
    boost::system_time         timeout_;
    // Events
    WaitableList<Event>        events_;
    std::map<std::string, int> instances_; // instance_id cache
    // Thread
    volatile bool              exit_;
    boost::thread*             thread_;

    void                       Commit();
    void                       Connect();
    void                       Disconnect();
    int                        GetObjectInstance(const HostStatus& ev);
    int                        GetInstanceId(const std::string& instance);
    void                       OnEvent(Event* e) throw ();
    void                       ProcessEvent(Event* event);
    void                       ProcessHost(const Host& host);
    void                       ProcessHostStatus(const HostStatus& hs);
    void                       ProcessService(const Service& service);
    void                       ProcessServiceStatus(const ServiceStatus& ss);
    void                       QueryExecuted();

   public:
                               DBOutput(DB::Connection::DBMS dbms);
                               DBOutput(const DBOutput& dbo);
                               ~DBOutput();
    DBOutput&                  operator=(const DBOutput& dbo);
    void                       operator()();
    void                       Destroy();
    void                       Init(const std::string& host,
                                    const std::string& user,
                                    const std::string& password,
                                    const std::string& db);
  };
}

#endif /* !DB_OUTPUT_H_ */
