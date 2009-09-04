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

#ifndef DB_ORACLE_CONNECTION_H_
# define DB_ORACLE_CONNECTION_H_

# include <ocilib.h>
# include "db/db_exception.h"
# include "db/oracle/delete.h"
# include "db/oracle/insert.h"
# include "db/oracle/select.h"
# include "db/oracle/truncate.h"
# include "db/oracle/update.h"

namespace                 CentreonBroker
{
  namespace               DB
  {
    /**
     *  \class OracleConnection connection.h "db/oracle/connection.h"
     *  \brief Oracle connection object.
     *
     *  This class holds informations about the current Oracle session. It is
     *  used to generate query objects.
     *
     *  \see Connection
     */
    class                 OracleConnection : public Connection
    {
     private:
      OCI_Connection*     oconn_;
                          OracleConnection(const OracleConnection& oconn);
      OracleConnection&   operator=(const OracleConnection& oconn);

     public:
                          OracleConnection() throw (DBException);
                          ~OracleConnection() throw ();
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
	return (new OracleMappedInsert<T>(this->oconn_, mapping));
      }

      /**
       *  Get an object-relational SELECT query.
       *
       *  \return A new MappedSelect query object.
       */
      template            <typename T>
      MappedSelect<T>*    GetMappedSelect(const MappingSetters<T>& mapping)
      {
	return (new OracleMappedSelect<T>(this->oconn_, mapping));
      }

      /**
       *  Get an object-relational UPDATE query.
       *
       *  \return A new MappedUpdate query object.
       */
      template            <typename T>
      MappedUpdate<T>*    GetMappedUpdate(const MappingGetters<T>& mapping)
      {
	return (new OracleMappedUpdate<T>(this->oconn_, mapping));
      }
    };
  }
}

#endif /* !DB_ORACLE_CONNECTION_H_ */
