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

/**
 *  Because the db/connection.h header includes this header, we have to trick
 *  to allow everything to compile properly.
 */
#include "db/connection.h"

#ifndef DB_MYSQL_CONNECTION_H_
# define DB_MYSQL_CONNECTION_H_

# include <mysql.h>
# include "db/db_exception.h"
# include "db/mysql/delete.hpp"
# include "db/mysql/insert.hpp"
# include "db/mysql/truncate.h"
# include "db/mysql/update.hpp"

namespace                 CentreonBroker
{
  namespace               DB
  {
    class                 MySQLConnection : public Connection
    {
     private:
      ::MYSQL*            myconn_;
                          MySQLConnection(const MySQLConnection& myconn)
                            throw ();
      MySQLConnection&    operator=(const MySQLConnection& myconn) throw ();

     public:
                          MySQLConnection() throw (DBException);
                          ~MySQLConnection() throw ();
      void                AutoCommit(bool activate = true)
                            throw (DBException);
      void                Commit() throw (DBException);
      void                Connect(const std::string& host,
			          const std::string& user,
			          const std::string& password,
			          const std::string& db);
      void                Disconnect() throw ();

      template            <typename ObjectType>
      Delete<ObjectType>* GetDeleteQuery(const Mapping<ObjectType>& mapping)
      {
	return (new MySQLDelete<ObjectType>(this->myconn_, mapping));
      }

      template            <typename ObjectType>
      Insert<ObjectType>* GetInsertQuery(const Mapping<ObjectType>& mapping)
      {
	return (new MySQLInsert<ObjectType>(this->myconn_, mapping));
      }

      Truncate*           GetTruncateQuery();

      template            <typename ObjectType>
      Update<ObjectType>* GetUpdateQuery(const Mapping<ObjectType>& mapping)
      {
	return (new MySQLUpdate<ObjectType>(this->myconn_, mapping));
      }
    };
  }
}

#endif /* !DB_MYSQL_CONNECTION_H_ */
