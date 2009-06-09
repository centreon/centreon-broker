/*
** query.h for CentreonBroker in ./inc/db/mysql
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/09/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
*/

#ifndef DB_MYSQL_QUERY_H_
# define DB_MYSQL_QUERY_H_

# include <mysql.h>
# include "db/query.h"

namespace         CentreonBroker
{
  namespace       DB
  {
    class         MySQLQuery : virtual public Query
    {
     private:
                  MySQLQuery(const MySQLQuery& myquery) throw ();
      MySQLQuery& operator=(const MySQLQuery& myquery) throw ();

     protected:
      MYSQL*      myconn_;
      MYSQL_STMT* mystmt_;
      std::string query_;

     public:
                  MySQLQuery(MYSQL* myconn) throw ();
      virtual     ~MySQLQuery();
      void        Execute() throw (DBException);
      void        Prepare() throw (DBException);
    };
  }
}

#endif /* !DB_MYSQL_QUERY_H_ */
