/*
** truncate.h for CentreonBroker in ./inc/db/mysql
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/05/09 Matthieu Kermagoret
** Last update 06/10/09 Matthieu Kermagoret
*/

#ifndef DB_MYSQL_TRUNCATE_H_
# define DB_MYSQL_TRUNCATE_H_

# include <mysql.h>
# include "db/db_exception.h"
# include "db/mysql/query.h"
# include "db/truncate.h"

namespace            CentreonBroker
{
  namespace          DB
  {
    class            MySQLTruncate : public Truncate, public MySQLQuery
    {
     private:
                     MySQLTruncate(const MySQLTruncate& mytruncate) throw ();
      MySQLTruncate& operator=(const MySQLTruncate& mytruncate) throw ();

     public:
                     MySQLTruncate(MYSQL* myconn);
                     ~MySQLTruncate();
      void           Execute();
      void           Prepare() throw (DBException);
    };
  }
}

#endif /* !DB_MYSQL_TRUNCATE_H_ */
