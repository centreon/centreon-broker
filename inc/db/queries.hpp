/*
** queries.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/09/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
*/

#ifndef DB_QUERIES_HPP_
# define DB_QUERIES_HPP_

# include "db/connection.h"
# include "db/db_exception.h"

namespace CentreonBroker
{
  namespace DB
  {
    /**
     *  Returns an appropriate insert query.
     */
    template              <typename ObjectType>
    Insert<ObjectType>*   GetInsertQuery(Connection* conn,
                                         const Mapping<ObjectType>* mapping)
      throw (DBException)
    {
      Insert<ObjectType>* insert;

      switch (conn->GetDbms())
	{
         case Connection::MYSQL:
          insert = static_cast<MySQLConnection*>(
                     conn)->GetInsertQuery<ObjectType>(mapping);
          break ;
         default:
	  throw (DBException(0,
                             DBException::QUERY_PREPARATION,
                             "Unsupported DBMS"));
	}
      return (insert);
    }
  }
}

#endif /* !DB_QUERIES_HPP_ */
