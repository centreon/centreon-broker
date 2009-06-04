/*
** exception.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 06/04/09 Matthieu Kermagoret
*/

#ifndef EXCEPTION_H_
# define EXCEPTION_H_

# include <boost/system/system_error.hpp>

namespace        CentreonBroker
{
  /**
   *  \brief     CentreonBroker root exception class.
   *
   *  Exception is the main class of exceptions thrown within CentreonBroker
   *  code. It directly subclass boost::system::system_error and only adds the
   *  ability to construct the exception from an error code and an error
   *  message (ie. throw (Exception(error_code, error_msg));). For more
   *  information, please refer to boost::system::system_error.
   */
  class                   Exception : public boost::system::system_error
    {
     public:
                          Exception(const Exception& e);
                          Exception(const boost::system::system_error& se);
                          Exception(int val);
                          Exception(int val, const char* msg);
                          ~Exception() throw ();
      Exception&          operator=(const Exception& e);
      virtual const char* what() const throw ();
    };
}

#endif /* !EXCEPTION_H_ */
