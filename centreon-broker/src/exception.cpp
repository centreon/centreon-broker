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
