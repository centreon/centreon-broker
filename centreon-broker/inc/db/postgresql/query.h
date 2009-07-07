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

#ifndef DB_POSTGRESQL_QUERY_H_
# define DB_POSTGRESQL_QUERY_H_

# include <libpq-fe.h>
# include "db/query.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    class          PgSQLQuery : virtual public Query
    {
     private:
      static int   query_nb;
                   PgSQLQuery(const PgSQLQuery& pgquery) throw ();
      PgSQLQuery&  operator=(const PgSQLQuery& pgquery) throw ();

     protected:
      unsigned int nparams_;
      int*         param_format_;
      int*         param_length_;
      char**       param_values_;
      PGconn*      pgconn_;
      std::string  query_;
      PGresult*    result_;
      std::string  stmt_name_;

     public:
                   PgSQLQuery(PGconn* pgconn) throw ();
      virtual      ~PgSQLQuery();
      void         Execute() throw (DBException);
      virtual void Prepare();
    };
  }
}

#endif /* !DB_MYSQL_QUERY_H_ */
