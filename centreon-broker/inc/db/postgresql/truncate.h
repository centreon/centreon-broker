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

#ifndef DB_POSTGRESQL_TRUNCATE_H_
# define DB_POSTGRESQL_TRUNCATE_H_

# include <libpq-fe.h>
# include "db/postgresql/query.h"
# include "db/truncate.h"

namespace            CentreonBroker
{
  namespace          DB
  {
    /**
     *  \class PgSQLTruncate truncate.h "db/postgresql/truncate.h"
     *
     *  PostgreSQL TRUNCATE query.
     *
     *  \see Truncate
     */
    class            PgSQLTruncate : public Truncate, public PgSQLQuery
    {
     private:
                     PgSQLTruncate(const PgSQLTruncate& pgtruncate);
      PgSQLTruncate& operator=(const PgSQLTruncate& pgtruncate);
      void           BuildQuery();

     public:
                     PgSQLTruncate(PGconn* pgconn);
                     ~PgSQLTruncate();
      void           Execute();
      void           Prepare();
    };
  }
}

#endif /* !DB_POSTGRESQL_TRUNCATE_H_ */
