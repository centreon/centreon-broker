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

# include <list>
# include <string>
# include "condition_variable.h"
# include "event_subscriber.h"
# include "mutex.h"
# include "thread.h"

// XXX
# include <mysql_connection.h>
# include <mysql_prepared_statement.h>

namespace                      CentreonBroker
{
  class                        HostStatusEvent;
  class                        ServiceStatusEvent;

  class                        MySQLOutput
    : private EventSubscriber, private Thread
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
      struct timespec          ts_;
      // Events processing
      std::list<Event*>        events_;
      ConditionVariable        eventscv_;
      Mutex                    eventsm_;
      // Thread specific parameter
      volatile bool            exit_thread_;

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

      // Thread
      int                      Core();

     public:
                               MySQLOutput();
                               ~MySQLOutput();
      void                     Destroy();
      void                     Init(const std::string& host,
                                    const std::string& user,
                                    const std::string& password,
                                    const std::string& db);
    };
}

#endif /* !MYSQL_OUTPUT_H_ */
