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
# include "db/mysql/delete.h"
# include "db/mysql/insert.h"
# include "db/mysql/select.h"
# include "db/mysql/truncate.h"
# include "db/mysql/update.h"

namespace                 CentreonBroker
{
  namespace               DB
  {
    /**
     *  \class MySQLConnection connection.h "db/mysql/connection.h"
     *  \brief MySQL connection object.
     *
     *  This class holds informations about the current MySQL session. It is
     *  used to generate query objects.
     *
     *  \see Connection
     */
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
      Delete*             GetDelete();
      Insert*             GetInsert();
      Select*             GetSelect();
      Truncate*           GetTruncate();
      Update*             GetUpdate();

      /**
       *  Get an object-relational INSERT query.
       *
       *  \return A new MappedInsert query object.
       */
      template            <typename T>
      MappedInsert<T>*    GetMappedInsert(const MappingGetters<T>& mapping)
      {
	return (new MySQLMappedInsert<T>(this->myconn_, mapping));
      }

      /**
       *  Get an object-relational SELECT query.
       *
       *  \return A new MappedSelect query object.
       */
      template            <typename T>
      MappedSelect<T>*    GetMappedSelect(const MappingSetters<T>& mapping)
      {
	return (new MySQLMappedSelect<T>(this->myconn_, mapping));
      }

      /**
       *  Get an object-relational UPDATE query.
       *
       *  \return A new MappedUpdate query object.
       */
      template            <typename T>
      MappedUpdate<T>*    GetMappedUpdate(const MappingGetters<T>& mapping)
      {
	return (new MySQLMappedUpdate<T>(this->myconn_, mapping));
      }
    };
  }
}

#endif /* !DB_MYSQL_CONNECTION_H_ */
