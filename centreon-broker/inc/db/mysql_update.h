/*
** mysql_update.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/04/09 Matthieu Kermagoret
*/

#ifndef DB_MYSQL_UPDATE_H_
# define DB_MYSQL_UPDATE_H_

# include <mysql.h>
# include "db/update_query.h"

namespace                   CentreonBroker
{
  class                     MySQLUpdate : public UpdateQuery
  {
    friend class            MySQLConnection;

   private:
    MYSQL*                  myconn_;
    MYSQL_BIND*             myparams_;
    MYSQL_STMT*             mystmt_;
                            MySQLUpdate(MYSQL* myconn);
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
    void                    SetString(int arg, const char* value);
    void                    SetTimeT(int arg, time_t value);
  };
}

#endif /* !DB_MYSQL_UPDATE_H_ */
