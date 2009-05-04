/*
** exception.cpp for CentreonBroker in /home/mkermagoret/work/centreon-broker
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/04/09 Matthieu Kermagoret
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
  this->msg = exception.msg;
}

/**
 *  Constructs an Exception from a string.
 */
Exception::Exception(const char* str)
{
  this->msg = str;
}

/**
 *  Constructs an Exception from a string.
 */
Exception::Exception(const std::string& str)
{
  this->msg = str;
}

/**
 *  Exception destructor.
 */
Exception::~Exception()
{
}

/**
 *  Exception operator= overload.
 */
Exception& Exception::operator=(const Exception& exception)
{
  this->msg = exception.msg;
  return (*this);
}

/**
 *  Set an Exception message from a string.
 */
Exception& Exception::operator=(const char* str)
{
  this->msg = str;
  return (*this);
}

/**
 *  Set an Exception message from a string.
 */
Exception& Exception::operator=(const std::string& str)
{
  this->msg = str;
  return (*this);
}

/**
 *  Returns the string associated with the exception.
 */
Exception::operator const std::string&() const
{
  return (this->msg);
}
