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

#include <stddef.h>
#include "exception.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy internal data from an object to the current instance.
 *
 *  Copy data members defined within the Exception class (namely the error code
 *  and the message) from the given object to the current instance. This method
 *  is used by the copy constructor and the assignment operator.
 *
 *  \param[in] e Object to copy data from.
 *
 *  \see Exception
 *  \see operator=
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
 *  \brief Exception copy constructor.
 *
 *  Copy data from the given Exception to the current instance.
 *
 *  \param[in] e Object to copy data from.
 */
Exception::Exception(const Exception& e) throw () : std::exception(e)
{
  this->InternalCopy(e);
}

/**
 *  Build an Exception from an error code.
 *
 *  \param[in] error_code Exception error code.
 */
Exception::Exception(int error_code) throw () : ec_(error_code), msg_(NULL) {}

/**
 *  Build an Exception from an error code and a message.
 *
 *  \param[in] error_code Exception error code.
 *  \param[in] what_msg   Exception message.
 */
Exception::Exception(int error_code, const char* msg) throw ()
  : ec_(error_code), msg_(msg) {}

/**
 *  Exception destructor.
 */
Exception::~Exception() throw () {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given Exception to the current instance.
 *
 *  \param[in] e Object to copy data from.
 */
Exception& Exception::operator=(const Exception& e) throw ()
{
  this->std::exception::operator=(e);
  this->InternalCopy(e);
  return (*this);
}

/**
 *  Get the exception error code.
 *
 *  \return Exception error code.
 */
int Exception::GetErrorCode() const throw ()
{
  return (this->ec_);
}

/**
 *  Get the exception message.
 *
 *  \return exception message.
 */
const char* Exception::GetMsg() const throw ()
{
  return (this->msg_);
}

/**
 *  Get the exception message.
 *
 *  \return exception message.
 */
const char* Exception::what() const throw ()
{
  return (this->GetMsg());
}
