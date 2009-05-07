/*
** mysqloutput.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/07/09 Matthieu Kermagoret
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
      volatile bool            exit_thread;
      std::string              host;
      std::string              user;
      std::string              password;
      std::string              db;
      std::list<Event*>        events;
      ConditionVariable        condvar;
      Mutex                    mutex;
      sql::PreparedStatement*  current_stmt;
      int                      current_arg;
                               MySQLOutput(const MySQLOutput& mysqlo);
      MySQLOutput&             operator=(const MySQLOutput& mysqlo);
      sql::PreparedStatement** PrepareQueries(sql::Connection& conn)
	throw (Exception);

      // EventSubscriber
      // XXX
      //void                     OnEvent(Event* e) throw ();
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
      // XXX
      void                     OnEvent(Event* e) throw ();
    };
}

#endif /* !MYSQLOUTPUT_H_ */
