/*
** exception.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/19/09 Matthieu Kermagoret
** Last update 06/04/09 Matthieu Kermagoret
*/

#include "exception.h"

using namespace CentreonBroker;

/**
 *  Exception copy constructor.
 */
Exception::Exception(const Exception& e) : boost::system::system_error(e)
{
}

/**
 *  Build an Exception from a Boost system_error.
 */
Exception::Exception(const boost::system::system_error& se)
  : boost::system::system_error(se)
{
}

/**
 *  Build an Exception from an error_code.
 */
Exception::Exception(int error_code)
  : boost::system::system_error(boost::system::error_code(error_code,
                                  boost::system::system_category))
{
}

/**
 *  Build an Exception from an error_code and a message.
 */
Exception::Exception(int error_code, const char* what_msg)
  : boost::system::system_error(boost::system::error_code(error_code,
                                  boost::system::system_category),
                                what_msg)
{
}

/**
 *  Exception destructor.
 */
Exception::~Exception() throw ()
{
}

/**
 *  Exception operator= overload.
 */
Exception& Exception::operator=(const Exception& e)
{
  boost::system::system_error::operator=(e);
  return (*this);
}

/**
 *  Get the error message associated with the exception.
 */
const char* Exception::what() const throw ()
{
  return (this->boost::system::system_error::what());
}
