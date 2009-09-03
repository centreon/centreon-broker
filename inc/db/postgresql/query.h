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
# include <string>
# include "db/query.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    class          PgSQLQuery : virtual public Query
    {
     private:
      static int   query_nb;
                   PgSQLQuery(const PgSQLQuery& pgquery);
      PgSQLQuery&  operator=(const PgSQLQuery& pgquery);

     protected:
      PGconn*      pgconn_;
      std::string  query;
      PGresult*    result;
      std::string  stmt_name;
                   PgSQLQuery(PGconn* pgconn);

     public:
      virtual      ~PgSQLQuery();
      virtual void Execute();
      void         Execute(unsigned int arg_count,
                           char** values,
                           int* lengths,
                           int* format);
      virtual void Prepare();
    };
  }
}

#endif /* !DB_POSTGRESQL_QUERY_H_ */
