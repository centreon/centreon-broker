/*
** connection.h for CentreonBroker in ./inc/db/mysql
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/03/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
*/

#ifndef DB_MYSQL_CONNECTION_H_
# define DB_MYSQL_CONNECTION_H_

# include <mysql.h>
# include "db/connection.h"
# include "db/db_exception.h"
# include "db/mysql/insert.hpp"

namespace              CentreonBroker
{
  namespace            DB
  {
    class              MySQLConnection : public Connection
    {
     private:
      ::MYSQL*         myconn_;
                       MySQLConnection(const MySQLConnection& myconn) throw ();
      MySQLConnection& operator=(const MySQLConnection& myconn) throw ();

     public:
                       MySQLConnection() throw (DBException);
                       ~MySQLConnection() throw ();
      void             AutoCommit(bool activate = true) throw (DBException);
      void             Commit() throw (DBException);
      void             Connect(const std::string& host,
			       const std::string& user,
			       const std::string& password,
			       const std::string& db) throw (DBException);
      void             Disconnect();

      template           <typename ObjectType>
      Insert<ObjectType>* GetInsertQuery(const Mapping<ObjectType>* mapping)
      {
	return (new MySQLInsert<ObjectType>(this->myconn_, mapping));
      }
    };
  }
}

#endif /* !DB_MYSQL_CONNECTION_H_ */
