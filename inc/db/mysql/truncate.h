/*
** mysql_truncate.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/05/09 Matthieu Kermagoret
** Last update 06/05/09 Matthieu Kermagoret
*/

#ifndef DB_MYSQL_TRUNCATE_H_
# define DB_MYSQL_TRUNCATE_H_

# include <mysql.h>
# include "db/truncate_query.h"

namespace          CentreonBroker
{
  class            MySQLTruncate : public TruncateQuery
  {
    friend class   MySQLConnection;

   private:
    MYSQL*         myconn_;
                   MySQLTruncate(MYSQL* myconn);
    void           InternalCopy(const MySQLTruncate& truncate);

   public:
                   MySQLTruncate(const MySQLTruncate& truncate);
                   ~MySQLTruncate();
    MySQLTruncate& operator=(const MySQLTruncate& truncate);
    void           Execute();
  };
}

#endif /* !DB_MYSQL_TRUNCATE_H_ */
