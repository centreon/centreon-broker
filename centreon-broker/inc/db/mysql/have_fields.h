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

#ifndef DB_MYSQL_HAVE_FIELDS_H_
# define DB_MYSQL_HAVE_FIELDS_H_

# include <mysql.h>
# include "db/have_fields.h"

namespace              CentreonBroker
{
  namespace            DB
  {
    class              MySQLHaveFields : virtual public HaveFields
    {
     private:
                       MySQLHaveFields(const MySQLHaveFields& myhf) throw ();
      MySQLHaveFields& operator=(const MySQLHaveFields& myhf) throw ();
      void             DeleteArg(MYSQL_BIND* arg);

     protected:
      int              arg_;
      MYSQL_BIND*      myargs_;
      unsigned int     size_;

     public:
                       MySQLHaveFields() throw ();
      virtual          ~MySQLHaveFields();
      void             Prepare(MYSQL_STMT* mystmt);
      void             Reset() throw ();
      void             SetDouble(double value);
      void             SetInt(int value);
      void             SetShort(short value);
      void             SetString(const std::string& value);
      void             SetTime(time_t value);
    };
  }
}

#endif /* !DB_MYSQL_HAVE_FIELDS_H_ */
