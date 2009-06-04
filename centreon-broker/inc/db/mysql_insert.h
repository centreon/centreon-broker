/*
** mysql_insert.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/04/09 Matthieu Kermagoret
*/

#ifndef DB_MYSQL_INSERT_H_
# define DB_MYSQL_INSERT_H_

# include <mysql.h>
# include "db/query.h"

namespace                   CentreonBroker
{
  class                     MySQLInsert : public Query
  {
    friend class            MySQLConnection;

   private:
    MYSQL*                  myconn_;
    MYSQL_BIND*             myparams_;
    MYSQL_STMT*             mystmt_;
                            MySQLInsert(MYSQL* myconn);
    void                    InternalCopy(const MySQLInsert& myinsert);

   public:
                            MySQLInsert(const MySQLInsert& myinsert);
                            ~MySQLInsert();
    MySQLInsert&            operator=(const MySQLInsert& myinsert);
    void                    Execute();
    void                    Prepare();
    void                    SetDouble(int arg, double value);
    void                    SetInt(int arg, int value);
    void                    SetShort(int arg, short value);
    void                    SetString(int arg, const char* value);
    void                    SetTimeT(int arg, time_t value);
  };
}

#endif /* !DB_MYSQL_INSERT_H_ */
