/*
** exception.cpp for CentreonBroker in /home/mkermagoret/work/centreon-broker
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/12/09 Matthieu Kermagoret
*/

#include "exception.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  Exception default constructor.
 */
Exception::Exception()
{
}

/**
 *  Exception copy constructor.
 */
Exception::Exception(const Exception& exception)
{
  this->what_ = exception.what_;
}

/**
 *  Constructs an Exception from a string.
 */
Exception::Exception(const char* str)
{
  this->what_ = str;
}

/**
 *  Constructs an Exception from a string.
 */
Exception::Exception(const std::string& str)
{
  this->what_ = str;
}

/**
 *  Exception destructor.
 */
Exception::~Exception() throw()
{
}

/**
 *  Exception operator= overload.
 */
Exception& Exception::operator=(const Exception& exception)
{
  this->what_ = exception.what_;
  return (*this);
}

/**
 *  Set an Exception message from a string.
 */
Exception& Exception::operator=(const char* str)
{
  this->what_ = str;
  return (*this);
}

/**
 *  Set an Exception message from a string.
 */
Exception& Exception::operator=(const std::string& str)
{
  this->what_ = str;
  return (*this);
}

/**
 *  Returns the string associated with the exception.
 */
const char* Exception::what() const throw()
{
  return (this->what_.c_str());
}
