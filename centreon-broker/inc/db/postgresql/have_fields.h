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

#ifndef DB_POSTGRESQL_HAVE_FIELDS_H_
# define DB_POSTGRESQL_HAVE_FIELDS_H_

# include <libpq-fe.h>
# include "db/have_fields.h"
# include "db/postgresql/query.h"

namespace              CentreonBroker
{
  namespace            DB
  {
    class              PgSQLHaveFields : virtual public HaveFields,
                                         public PgSQLQuery
    {
     private:
                       PgSQLHaveFields(const PgSQLHaveFields& pghf) throw ();
      PgSQLHaveFields& operator=(const PgSQLHaveFields& pghf) throw ();
      void             DeleteArg(int idx);

     protected:
      int              arg_;

     public:
                       PgSQLHaveFields(PGconn* pgconn) throw ();
      virtual          ~PgSQLHaveFields();
      void             Prepare();
      void             Reset() throw ();
      void             SetBool(bool value);
      void             SetDouble(double value);
      void             SetInt(int value);
      void             SetShort(short value);
      void             SetString(const std::string& value);
      void             SetTime(time_t value);
    };
  }
}

#endif /* !DB_POSTGRESQL_HAVE_FIELDS_H_ */
