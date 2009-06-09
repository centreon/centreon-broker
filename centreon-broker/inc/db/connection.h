/*
** connection.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/29/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
*/

#ifndef DB_CONNECTION_H_
# define DB_CONNECTION_H_

# include "db/db_exception.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    class          Connection
    {
     public:
      enum         DBMS
      {
	MYSQL = 1,
	ORACLE,
	POSTGRESQL
      };

     protected:
      DBMS         dbms_;

     public:
                   Connection(DBMS dbms);
                   Connection(const Connection& conn);
      virtual      ~Connection();
      Connection&  operator=(const Connection& conn);
      virtual void AutoCommit(bool activate = true)
                     throw (DBException) = 0;
      virtual void Commit()
                     throw (DBException) = 0;
      virtual void Connect(const std::string& host,
                           const std::string& user,
	                   const std::string& password,
		           const std::string& db)
                     throw (DBException) = 0;
      virtual void Disconnect() = 0;
      DBMS         GetDbms() const throw ();
    };
  }
}

#endif /* !DB_CONNECTION_H_ */
