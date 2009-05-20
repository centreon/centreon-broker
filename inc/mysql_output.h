/*
** mysql_output.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/19/09 Matthieu Kermagoret
*/

#ifndef MYSQL_OUTPUT_H_
# define MYSQL_OUTPUT_H_

# include <boost/thread.hpp>
# include <list>
# include <string>
# include "event_subscriber.h"
# include "exception.h"

// XXX
# include <mysql_connection.h>
# include <mysql_prepared_statement.h>

namespace                      CentreonBroker
{
  class                        HostStatusEvent;
  class                        ServiceStatusEvent;

  class                        MySQLOutput
    : private EventSubscriber
    {
      // Initialization parameters
      std::string              host_;
      std::string              user_;
      std::string              password_;
      std::string              db_;
      // MySQL objects
      sql::Connection*         myconn_;
      sql::PreparedStatement** stmts_;
      // MySQL performance objects
      int                      queries_count_;
      boost::system_time       timeout_;
      // Events processing
      std::list<Event*>        events_;
      boost::condition_variable eventscv_;
      boost::mutex              eventsm_;
      // Thread specific parameter
      volatile bool            exit_thread_;
      boost::thread*           thread_;

                               MySQLOutput(const MySQLOutput& mysqlo);
      MySQLOutput&             operator=(const MySQLOutput& mysqlo);
      void                     Commit();
      void                     Connect();
      void                     Disconnect();
      sql::PreparedStatement** PrepareQueries(sql::Connection& conn)
                                 throw (Exception);
      // XXX : remove ProcessEvent
      void                     ProcessEvent(Event* event);
      sql::PreparedStatement*  ProcessHostStatusEvent(HostStatusEvent* hse);
      sql::PreparedStatement*  ProcessServiceStatusEvent(
                                 ServiceStatusEvent* sse);
      Event*                   WaitEvent();

      // EventSubscriber
      void                     OnEvent(Event* e) throw ();

     public:
                               MySQLOutput();
                               ~MySQLOutput();
      void                     Destroy();
      void                     Init(const std::string& host,
                                    const std::string& user,
                                    const std::string& password,
                                    const std::string& db);
      // Thread
      void                     operator()();
    };
}

#endif /* !MYSQL_OUTPUT_H_ */
