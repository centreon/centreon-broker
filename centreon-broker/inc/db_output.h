/*
** db_output.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/03/09 Matthieu Kermagoret
*/

#ifndef DB_OUTPUT_H_
# define DB_OUTPUT_H_

# include <boost/thread/thread_time.hpp>
# include <string>
# include <vector>
# include "event_subscriber.h"
# include "waitable_list.hpp"

namespace                      boost
{
  class                        thread;
}

namespace                      CentreonBroker
{
  class                        DBConnection;
  class                        Event;
  class                        Host;
  class                        HostStatus;
  class                        Query;
  class                        UpdateQuery;

  class                        DBOutput : private EventSubscriber
  {
   public:
    enum                       DBMS
    {
      MYSQL = 1
    };

   private:
    // DBMS
    DBMS                       dbms_;
    std::string                host_;
    std::string                user_;
    std::string                password_;
    std::string                db_;
    DBConnection*              conn_;
    std::vector<UpdateQuery*>  stmts_;
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
    void                       ExecuteQuery(Query* query);
    int                        GetInstanceId(const std::string& instance);
    void                       OnEvent(Event* e) throw ();
    void                       ProcessEvent(Event* event);
    void                       ProcessHost(const Host& host);
    void                       ProcessHostStatus(const HostStatus& hs);

   public:
                               DBOutput();
                               DBOutput(const DBOutput& dbo);
                               ~DBOutput();
    DBOutput&                  operator=(const DBOutput& dbo);
    void                       operator()();
    void                       Destroy();
    void                       Init(DBMS dbms,
                                    const std::string& host,
                                    const std::string& user,
                                    const std::string& password,
                                    const std::string& db);
  };
}

#endif /* !DB_OUTPUT_H_ */
