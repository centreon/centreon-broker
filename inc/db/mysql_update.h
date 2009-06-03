/*
** mysql_update.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/02/09 Matthieu Kermagoret
*/

#ifndef DB_MYSQL_UPDATE_H_
# define DB_MYSQL_UPDATE_H_

# include "db/update_query.h"

namespace                   sql
{
  class                     Connection;
  class                     PreparedStatement;
}

namespace                   CentreonBroker
{
  class                     MySQLUpdate : public UpdateQuery
  {
    friend class            MySQLConnection;

   private:
    sql::Connection*        myconn_;
    sql::PreparedStatement* mystmt_;
                            MySQLUpdate(sql::Connection* myconn);
    void                    InternalCopy(const MySQLUpdate& myupdate);

   public:
                            MySQLUpdate(const MySQLUpdate& myupdate);
                            ~MySQLUpdate();
    MySQLUpdate&            operator=(const MySQLUpdate& myupdate);
    void                    Execute();
    void                    Prepare();
    void                    SetDouble(int arg, double value);
    void                    SetInt(int arg, int value);
    void                    SetShort(int arg, short value);
    void                    SetString(int arg, const std::string& value);
    void                    SetTimeT(int arg, time_t value);
  };
}

#endif /* !DB_MYSQL_UPDATE_H_ */
