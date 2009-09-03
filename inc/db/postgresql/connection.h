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

/**
 *  Because the db/connection.h header includes this header, we have to trick
 *  to allow everything to compile properly.
 */
#include "db/connection.h"

#ifndef DB_POSTGRESQL_CONNECTION_H_
# define DB_POSTGRESQL_CONNECTION_H_

# include <libpq-fe.h>
# include <string>
# include "db/db_exception.h"
# include "db/postgresql/delete.h"
# include "db/postgresql/insert.h"
# include "db/postgresql/select.h"
# include "db/postgresql/truncate.h"
# include "db/postgresql/update.h"
# include "mapping.h"

namespace              CentreonBroker
{
  namespace            DB
  {
    /**
     *  \class PgSQLConnection connection.h "db/postgresql/connection.h"
     *  \brief PostgreSQL connection object.
     *
     *  This class holds informations about the current PostgreSQL session. It
     *  is used to generate query objects.
     *
     *  \see Connection
     */
    class              PgSQLConnection : public Connection
    {
     private:
      PGconn*          pgconn_;
                       PgSQLConnection(const PgSQLConnection& pgconn);
      PgSQLConnection& operator=(const PgSQLConnection& pgconn);

     public:
                       PgSQLConnection();
                       ~PgSQLConnection();
      void             AutoCommit(bool activate = true);
      void             Commit();
      void             Connect(const std::string& host,
		               const std::string& user,
		               const std::string& password,
                               const std::string& db);
      void             Disconnect();
      Delete*          GetDelete();
      Insert*          GetInsert();
      Select*          GetSelect();
      Truncate*        GetTruncate();
      Update*          GetUpdate();

      /**
       *  Get an object-relational INSERT query.
       *
       *  \return A new MappedInsert query object.
       */
      template         <typename T>
      MappedInsert<T>* GetMappedInsert(const MappingGetters<T>& mapping)
      {
	return (new PgSQLMappedInsert<T>(this->pgconn_, mapping));
      }

      /**
       *  Get an object-relational SELECT query.
       *
       *  \return A new MappedSelect query object.
       */
      template         <typename T>
      MappedSelect<T>* GetMappedSelect(const MappingSetters<T>& mapping)
      {
	return (new PgSQLMappedSelect<T>(this->pgconn_, mapping));
      }

      /**
       *  Get an object-relational UPDATE query.
       *
       *  \return A new MappedUpdate query object.
       */
      template         <typename T>
      MappedUpdate<T>* GetMappedUpdate(const MappingGetters<T>& mapping)
      {
	return (new PgSQLMappedUpdate<T>(this->pgconn_, mapping));
      }
    };
  }
}

#endif /* !DB_POSTGRESQL_CONNECTION_H_ */
