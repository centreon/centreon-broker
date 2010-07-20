/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include "exception.h"

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy exception parameters from the argument to the current instance.
 *
 *  Copy data members defined within the Exception class (namely the error code
 *  and the message) from the given object to the current instance. This method
 *  is used by the copy constructor and the assignment operator.
 *  \par Safety No throw guarantee.
 *
 *  \param[in] e Exception to copy data from.
 *
 *  \see Exception(const Exception&)
 *  \see operator=(const Exception&)
 */
void Exception::InternalCopy(const Exception& e) throw ()
{
  this->ec_ = e.ec_;
  this->msg_ = e.msg_;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Build an Exception from an error code and a message.
 *
 *  \param[in] error_code Exception error code.
 *  \param[in] what_msg   Exception message.
 */
Exception::Exception(int error_code, const char* msg) throw ()
  : ec_(error_code), msg_(msg) {}

/**
 *  \brief Exception copy constructor.
 *
 *  Copy the error code and the associated message from the given Exception
 *  object to the current instance.
 *
 *  \param[in] e Exception to copy data from.
 */
Exception::Exception(const Exception& e) throw () : std::exception(e)
{
  this->InternalCopy(e);
}

/**
 *  Exception destructor.
 */
Exception::~Exception() throw () {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy the error code and the associated message from the given Exception
 *  object to the current instance.
 *  \par Safety No throw guarantee.
 *
 *  \param[in] e Exception to copy data from.
 */
Exception& Exception::operator=(const Exception& e) throw ()
{
  this->std::exception::operator=(e);
  this->InternalCopy(e);
  return (*this);
}

/**
 *  Get the exception error code.
 *  \par Safety No throw guarantee.
 *
 *  \return Exception error code.
 */
int Exception::GetErrorCode() const throw ()
{
  return (this->ec_);
}

/**
 *  Get the exception message.
 *  \par Safety No throw guarantee.
 *
 *  \return exception message.
 */
const char* Exception::GetMsg() const throw ()
{
  return (this->msg_);
}

/**
 *  Get the exception message.
 *  \par Safety No throw guarantee.
 *
 *  \return exception message.
 */
const char* Exception::what() const throw ()
{
  return (this->GetMsg());
}
