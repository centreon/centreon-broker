/*
** exception.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/19/09 Matthieu Kermagoret
*/

#ifndef EXCEPTION_H_
# define EXCEPTION_H_

# include <boost/system/system_error.hpp>

namespace        CentreonBroker
{
  /**
   *  Because we're using the Boost library, most of the time, exceptions are
   *  raised within Boost code. It's easier for us to rethrow the exception.
   */
  class          Exception : public boost::system::system_error
    {
     public:
                 Exception(const Exception& e);
                 Exception(const boost::system::system_error& se);
                 Exception(int val);
		 Exception(int val, const char* msg);
                 ~Exception() throw ();
      Exception& operator=(const Exception& e);
    };
}

#endif /* !EXCEPTION_H_ */
