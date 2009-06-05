/*
** mysql_connection.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/05/09 Matthieu Kermagoret
*/

#ifndef DB_MYSQL_CONNECTION_H_
# define DB_MYSQL_CONNECTION_H_

# include <mysql.h>
# include <string>
# include "db/connection.h"

namespace            sql
{
  class              Connection;
}

namespace CentreonBroker
{
  class              Query;
  class              Update;

  class              MySQLConnection : public DBConnection
  {
   private:
    MYSQL            myconn_;
    void             InternalCopy(const MySQLConnection& myconn);

   public:
                     MySQLConnection();
                     MySQLConnection(const MySQLConnection& myconn);
                     ~MySQLConnection();
    MySQLConnection& operator=(const MySQLConnection& myconn);
    void             Commit();
    void             Connect(const std::string& host,
                             const std::string& user,
                             const std::string& password,
                             const std::string& db);
    void             Disconnect();
    Query*           GetInsertQuery();
    TruncateQuery*   GetTruncateQuery();
    UpdateQuery*     GetUpdateQuery();
  };
}

#endif /* !DB_MYSQL_CONNECTION_H_ */
