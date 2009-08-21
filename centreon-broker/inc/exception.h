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

#ifndef EXCEPTION_H_
# define EXCEPTION_H_

# include <boost/system/system_error.hpp>

namespace                 CentreonBroker
{
  /**
   *  \class Exception exception.h "exception.h"
   *  \brief CentreonBroker root exception class.
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
    };
}

#endif /* !EXCEPTION_H_ */
