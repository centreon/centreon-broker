/*
** exception.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/12/09 Matthieu Kermagoret
*/

#ifndef EXCEPTION_H_
# define EXCEPTION_H_

# include <exception>
# include <string>

namespace               CentreonBroker
{
  /**
   *  This is the standard interface for exception thrown within CentreonBroker
   *  code. Nothing fancy, just a wrapper class around an std::string.
   */
  class                 Exception : public std::exception
  {
   protected:
    std::string         what_;

   public:
                        Exception();
                        Exception(const Exception& exception);
                        Exception(const char* str);
                        Exception(const std::string& str);
    virtual             ~Exception() throw();
    Exception&          operator=(const Exception& exception);
    Exception&          operator=(const char* str);
    Exception&          operator=(const std::string& str);
    virtual const char* what() const throw();
  };
}

#endif /* !EXCEPTION_H_ */
