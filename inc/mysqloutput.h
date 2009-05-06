/*
** mysqloutput.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/06/09 Matthieu Kermagoret
*/

#ifndef MYSQLOUTPUT_H_
# define MYSQLOUTPUT_H_

# include <list>
# include <mysql.h>
# include <string>
# include "conditionvariable.h"
# include "eventsubscriber.h"
# include "mutex.h"
# include "thread.h"

namespace                    CentreonBroker
{
  class                      MySQLOutput
    : private EventSubscriber, private Thread
    {
     private:
      volatile bool          exit_thread;
      std::string            host;
      std::string            user;
      std::string            password;
      std::string            db;
      std::list<Event*> events;
      ConditionVariable      condvar;
      Mutex                  mutex;
      MYSQL_STMT*            stmt[2];
      MYSQL_BIND*            params[2];
                             MySQLOutput(const MySQLOutput& mysqlo);
      MySQLOutput&           operator=(const MySQLOutput& mysqlo);
      void                   PrepareQueries(MYSQL* mysql);

      // EventSubscriber
      void                   OnDump(Event* e, ...);
      void                   OnEvent(Event* e) throw ();

      // Thread
      int                    Core();

     public:
                             MySQLOutput();
                             ~MySQLOutput();
      void                   Destroy();
      void                   Init(const std::string& host,
                                  const std::string& user,
                                  const std::string& password,
                                  const std::string& db);
    };
}

#endif /* !MYSQLOUTPUT_H_ */
