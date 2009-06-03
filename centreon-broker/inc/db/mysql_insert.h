/*
** mysql_insert.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/02/09 Matthieu Kermagoret
*/

#ifndef DB_MYSQL_INSERT_H_
# define DB_MYSQL_INSERT_H_

# include "db/query.h"

namespace                   sql
{
  class                     Connection;
  class                     PreparedStatement;
}

namespace                   CentreonBroker
{
  class                     MySQLInsert : public Query
  {
    friend class            MySQLConnection;

   private:
    sql::Connection*        myconn_;
    sql::PreparedStatement* mystmt_;
                            MySQLInsert(sql::Connection* myconn);
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
    void                    SetString(int arg, const std::string& value);
    void                    SetTimeT(int arg, time_t value);
  };
}

#endif /* !DB_MYSQL_INSERT_H_ */
