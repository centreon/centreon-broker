/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef DB_MYSQL_QUERY_H_
# define DB_MYSQL_QUERY_H_

# include <mysql.h>
# include "db/query.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    class          MySQLQuery : virtual public Query
    {
     private:
                   MySQLQuery(const MySQLQuery& myquery) throw ();
      MySQLQuery&  operator=(const MySQLQuery& myquery) throw ();

     protected:
      MYSQL*       myconn_;
      MYSQL_STMT*  mystmt_;
      std::string  query_;

     public:
                   MySQLQuery(MYSQL* myconn) throw ();
      virtual      ~MySQLQuery();
      void         Execute() throw (DBException);
      virtual void Prepare();
    };
  }
}

#endif /* !DB_MYSQL_QUERY_H_ */
