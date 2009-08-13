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

#ifndef DB_MYSQL_INSERT_H_
# define DB_MYSQL_INSERT_H_

# include "db/insert.h"
# include "db/mysql/have_args.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    class          MySQLInsert : public Insert, public MySQLHaveArgs
    {
     private:
      void         GenerateQuery();
      unsigned int GetArgCount() throw ();

     protected:
                   MySQLInsert(const MySQLInsert& myinsert);
      MySQLInsert& operator=(const MySQLInsert& myinsert);

     public:
                   MySQLInsert(MYSQL* myconn);
      virtual      ~MySQLInsert();
      void         Execute();
      void         Prepare();
      void         SetArg(bool arg);
    };
  }
}

#endif /* !DB_MYSQL_INSERT_HPP_ */
