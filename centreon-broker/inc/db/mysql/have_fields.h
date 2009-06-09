/*
** have_fields.h for CentreonBroker in ./inc/db/mysql
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/09/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
*/

#ifndef DB_MYSQL_HAVE_FIELDS_H_
# define DB_MYSQL_HAVE_FIELDS_H_

# include <mysql.h>
# include "db/have_fields.h"

namespace              CentreonBroker
{
  namespace            DB
  {
    class              MySQLHaveFields : virtual public HaveFields
    {
     private:
                       MySQLHaveFields(const MySQLHaveFields& myhf) throw ();
      MySQLHaveFields& operator=(const MySQLHaveFields& myhf) throw ();
      void             DeleteArg(MYSQL_BIND* arg);

     protected:
      int              arg_;
      MYSQL_BIND*      myargs_;
      unsigned int     size_;

     public:
                       MySQLHaveFields() throw ();
      virtual          ~MySQLHaveFields();
      void             Prepare(MYSQL_STMT* mystmt);
      void             Reset() throw ();
      void             SetDouble(double value);
      void             SetInt(int value);
      void             SetShort(short value);
      void             SetString(const std::string& value);
      void             SetTime(time_t value);
    };
  }
}

#endif /* !DB_MYSQL_HAVE_FIELDS_H_ */
