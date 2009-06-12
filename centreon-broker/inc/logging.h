/*
** logging.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/12/09 Matthieu Kermagoret
** Last update 06/12/09 Matthieu Kermagoret
*/

#ifndef LOGGING_H_
# define LOGGING_H_

# include <boost/thread/mutex.hpp>
# include <map>
# include <sys/types.h>

namespace                    CentreonBroker
{
  class                      Logging
  {
   private:
    std::map<pthread_t, int> indent_;
    boost::mutex             mutex_;
                             Logging(const Logging& logging);
    Logging&                 operator=(const Logging& logging);

   public:
                             Logging();
                             ~Logging();
    void                     AddDebug(const char* str);
    void                     AddError(const char* str);
    void                     AddInfo(const char* str);
    void                     Deindent();
    void                     Indent();
    void                     ThreadOver();
  };

  extern Logging             logging;
}

#endif /* !LOGGING_H_ */
