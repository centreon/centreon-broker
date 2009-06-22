/*
** log.h for CentreonBroker in ./inc/conf
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/22/09 Matthieu Kermagoret
** Last update 06/22/09 Matthieu Kermagoret
*/

#ifndef CONF_LOG_H_
# define CONF_LOG_H_

# include <string>

namespace                CentreonBroker
{
  namespace              Conf
  {
    class                Log
    {
     private:
      enum               Int
      {
	FLAGS = 0,
	INT_NB
      };
      enum               String
      {
	PATH = 0,
	TYPE,
	STRING_NB
      };
      int                ints_[INT_NB];
      std::string        strings_[STRING_NB];
      void               InternalCopy(const Log& l);

     public:
                         Log();
                         Log(const Log& l);
                         ~Log();
      Log&               operator=(const Log& l);
      bool               operator==(const Log& l);
      int                GetFlags() const throw ();
      const std::string& GetPath() const throw ();
      const std::string& GetType() const throw ();
      void               SetFlags(int flags) throw ();
      void               SetPath(const std::string& path);
      void               SetType(const std::string& type);
    };
  }
}

#endif /* !CONF_LOG_H_ */
