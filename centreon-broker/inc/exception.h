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

# include <exception>

namespace         CentreonBroker
{
  /**
   *  \class Exception exception.h "exception.h"
   *  \brief CentreonBroker root exception class.
   *
   *  Exception is the main class of exceptions thrown within CentreonBroker
   *  code. It directly subclass std::exception. It has the ability to
   *  construct the exception from an error code and an error message (ie.
   *  throw (Exception(error_code, error_msg)); ).
   */
  class           Exception : public std::exception
  {
   private:
    int         ec_;
    const char* msg_;
    void        InternalCopy(const Exception& e) throw ();

   public:
                Exception(const Exception& e) throw ();
                Exception(int val) throw ();
                Exception(int val, const char* msg) throw ();
                ~Exception() throw ();
    Exception&  operator=(const Exception& e) throw ();
    int         GetErrorCode() const throw ();
    const char* GetMsg() const throw ();
    const char* what() const throw ();
  };
}

#endif /* !EXCEPTION_H_ */
