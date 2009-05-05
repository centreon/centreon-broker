/*
** mysqloutput.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/05/09 Matthieu Kermagoret
*/

#ifndef MYSQLOUTPUT_H_
# define MYSQLOUTPUT_H_

# include <list>
# include <string>
# include "mutex.h"
# include "thread.h"

namespace                    CentreonBroker
{
  class                      MySQLOutput
    : public CentreonBroker::Thread
    {
     private:
      std::string            host;
      std::string            user;
      std::string            password;
      std::string            db;
      std::list<std::string> queries;
      CentreonBroker::Mutex  queries_mutex;
                             MySQLOutput(const MySQLOutput& mysqlo);
      MySQLOutput&           operator=(const MySQLOutput& mysqlo);

     public:
                             MySQLOutput();
                             ~MySQLOutput();
      void                   Connect(const std::string& host,
                                     const std::string& user,
                                     const std::string& password,
                                     const std::string& db);

      // Thread
      int                    Core();

      // XXX : event handling
    };
}

#endif /* !MYSQLOUTPUT_H_ */
