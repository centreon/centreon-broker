/*
** connection.h for CentreonBroker in ./inc/db/postgresql
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/12/09 Matthieu Kermagoret
** Last update 06/12/09 Matthieu Kermagoret
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

namespace              CentreonBroker
{
  namespace            DB
  {
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

      // XXX : GetQueries()
    };
  }
}

#endif /* !DB_POSTGRESQL_CONNECTION_H_ */
