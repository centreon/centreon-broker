/*
** exception.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/05/09 Matthieu Kermagoret
*/

#ifndef EXCEPTION_H_
# define EXCEPTION_H_

# include <string>

namespace       CentreonBroker
{
  class         Exception
  {
   protected:
    std::string msg;

   public:
                Exception();
                Exception(const Exception& exception);
                Exception(const char* str);
                Exception(const std::string& str);
                ~Exception();
    Exception&  operator=(const Exception& exception);
    Exception&  operator=(const char* str);
    Exception&  operator=(const std::string& str);
    virtual     operator const std::string&() const;
  };
}

#endif /* !EXCEPTION_H_ */
