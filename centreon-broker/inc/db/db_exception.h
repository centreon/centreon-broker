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

#ifndef DB_EXCEPTION_H_
# define DB_EXCEPTION_H_

# include "exception.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    class          DBException : public CentreonBroker::Exception
    {
     public:
      enum         Reason
      {
	UNKNOWN = 0,
	COMMIT,
	INITIALIZATION,
	QUERY_EXECUTION,
	QUERY_PREPARATION
      };

     private:
      Reason       reason_;
      void         InternalCopy(const DBException& dbe) throw ();

     public:
                   DBException(const DBException& dbe);
		   DBException(int val,
                               Reason reason = UNKNOWN,
                               const char* msg = NULL);
                   ~DBException() throw ();
      DBException& operator=(const DBException& dbe);
      Reason       GetReason() const throw ();
    };
  }
}

#endif /* !DB_EXCEPTION_H_ */
