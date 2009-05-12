/*
** mysqloutput.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/12/09 Matthieu Kermagoret
*/

#ifndef MYSQLOUTPUT_H_
# define MYSQLOUTPUT_H_

# include <list>
# include <string>
# include "conditionvariable.h"
# include "eventsubscriber.h"
# include "mutex.h"
# include "thread.h"

// XXX
# include <mysql_connection.h>
# include <mysql_prepared_statement.h>

namespace                      CentreonBroker
{
  class                        MySQLOutput
    : private EventSubscriber, private Thread
    {
     private:
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
      // Used to build the argument list
      sql::PreparedStatement*  cur_stmt_;
      int                      cur_arg_;
      // Thread specific parameter
      volatile bool            exit_thread_;

                               MySQLOutput(const MySQLOutput& mysqlo);
      MySQLOutput&             operator=(const MySQLOutput& mysqlo);
      void                     Commit();
      void                     Connect();
      void                     Disconnect();
      sql::PreparedStatement** PrepareQueries(sql::Connection& conn)
                                 throw (Exception);
      void                     ProcessEvent(Event* event);
      Event*                   WaitEvent();

      // EventSubscriber
      void                     OnEvent(Event* e) throw ();
      void                     Visit(const char* arg);
      void                     Visit(double arg);
      void                     Visit(int arg);
      void                     Visit(short arg);
      void                     Visit(const std::string& arg);
      void                     Visit(time_t arg);

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

#endif /* !MYSQLOUTPUT_H_ */
