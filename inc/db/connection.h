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

#ifndef DB_CONNECTION_H_
# define DB_CONNECTION_H_

# include "db/db_exception.h"

namespace                 CentreonBroker
{
  namespace               DB
  {
    /**
     *  Forward declarations.
     */
    // Normal queries
    class                 Delete;
    class                 Insert;
    class                 Select;
    class                 Truncate;
    class                 Update;
    // Mapped queries
    template              <typename ObjectType>
    class                 MappedDelete;
    template              <typename ObjectType>
    class                 MappedInsert;
    template              <typename ObjectType>
    class                 MappedSelect;
    template              <typename ObjectType>
    class                 MappedUpdate;
    // Misc
    template              <typename ObjectType>
    class                 MappingGetters;
    template              <typename ObjectType>
    class                 MappingSetters;

    /**
     *  \class Connection connection.h "db/connection.h"
     *  \brief Abstraction of a database connection.
     *
     *  This interface represents a connection to a database server like MySQL
     *  or PostgreSQL.
     */
    class                 Connection
    {
     public:
      enum                DBMS
      {
	UNKNOWN = 0,
	MYSQL,
	ORACLE,
	POSTGRESQL
      };

     private:
      const DBMS          dbms_;

     protected:
                          Connection(const Connection& conn) throw ();
      Connection&         operator=(const Connection& conn) throw ();

     public:
                          Connection(DBMS dbms) throw ();
      virtual             ~Connection();
      virtual void        AutoCommit(bool activate = true) = 0;
      virtual void        Commit() = 0;
      virtual void        Connect(const std::string& host,
                                  const std::string& user,
                                  const std::string& password,
                                  const std::string& db) = 0;
      virtual void        Disconnect() = 0;
      DBMS                GetDbms() const throw ();
      virtual Delete*     GetDelete() = 0;
      virtual Insert*     GetInsert() = 0;
      virtual Select*     GetSelect() = 0;
      virtual Truncate*   GetTruncate() = 0;
      virtual Update*     GetUpdate() = 0;
      template            <typename T>
      MappedInsert<T>*    GetMappedInsert(const MappingGetters<T>& mapping);
      template            <typename T>
      MappedSelect<T>*    GetMappedSelect(const MappingSetters<T>& mapping);
      template            <typename T>
      MappedUpdate<T>*    GetMappedUpdate(const MappingGetters<T>& mapping);
    };
  }
}

# include <cassert>

# ifdef USE_MYSQL
#  include "db/mysql/connection.h"
# endif /* USE_MYSQL */

# ifdef USE_ORACLE
//# include "db/oracle/connection.h"
# endif /* USE_ORACLE */

# ifdef USE_POSTGRESQL
#  include "db/postgresql/connection.h"
# endif /* USE_POSTGRESQL */

/**
 *  Return an INSERT query matching the DBMS. Because we can't override a
 *  template method, we will dynamic_cast the Connection to call the proper
 *  method.
 */
template <typename T>                              // Template
CentreonBroker::DB::MappedInsert<T>*               // Return type
  CentreonBroker::DB::Connection::GetMappedInsert( // Method
    const MappingGetters<T>& mapping)              // Argument
{
  MappedInsert<T>* insert;

  switch (this->dbms_)
    {
# ifdef USE_MYSQL
     case MYSQL:
      insert = dynamic_cast<MySQLConnection*>(this)
	->GetMappedInsert<T>(mapping);
      break ;
# endif /* USE_MYSQL */

# ifdef USE_ORACLE
     case ORACLE:
# endif /* USE_ORACLE */

# ifdef USE_POSTGRESQL
     case POSTGRESQL:
      insert = dynamic_cast<PgSQLConnection*>(this)
        ->GetMappedInsert<T>(mapping);
      break ;
# endif /* USE_POSTGRESQL */

     default:
      assert(false);
      throw (DBException(this->dbms_,
                         DBException::QUERY_PREPARATION,
                         "Unsupported DBMS"));
    }
  return (insert);
}

/**
 *  Return a SELECT query matching the DBMS. Because we can't override a
 *  template method, we will dynamic_cast the Connection to call the proper
 *  method.
 */
template <typename T>                              // Template
CentreonBroker::DB::MappedSelect<T>*               // Return type
  CentreonBroker::DB::Connection::GetMappedSelect( // Method
    const MappingSetters<T>& mapping)              // Argument
{
  MappedSelect<T>* select;

  switch (this->dbms_)
    {
# ifdef USE_MYSQL
     case MYSQL:
      select = dynamic_cast<MySQLConnection*>(this)
	->GetMappedSelect<T>(mapping);
      break ;
# endif /* USE_MYSQL */

# ifdef USE_ORACLE
     case ORACLE:
# endif /* USE_ORACLE */

# ifdef USE_POSTGRESQL
     case POSTGRESQL:
      select = dynamic_cast<PgSQLConnection*>(this)
        ->GetMappedSelect<T>(mapping);
      break ;
# endif /* USE_POSTGRESQL */

     default:
      assert(false);
      throw (DBException(this->dbms_,
                         DBException::QUERY_PREPARATION,
                         "Unsupported DBMS"));
    }
  return (select);
}

/**
 *  Return an UPDATE query matching the DBMS. Because we can't override a
 *  template, we will dynamic_cast the Connection to call the proper method.
 */
template <typename T>                              // Template
CentreonBroker::DB::MappedUpdate<T>*               // Return type
  CentreonBroker::DB::Connection::GetMappedUpdate( // Method
    const MappingGetters<T>& mapping)              // Argument
{
  MappedUpdate<T>* update;

  switch (this->dbms_)
    {
# ifdef USE_MYSQL
     case MYSQL:
      update = dynamic_cast<MySQLConnection*>(this)
	->GetMappedUpdate<T>(mapping);
      break ;
# endif /* USE_MYSQL */

# ifdef USE_ORACLE
     case ORACLE:
# endif /* USE_ORACLE */

# ifdef USE_POSTGRESQL
     case POSTGRESQL:
      update = dynamic_cast<PgSQLConnection*>(this)
	->GetMappedUpdate<T>(mapping);
      break ;
# endif /* USE_POSTGRESQL */

     default:
      assert(false);
      throw (DBException(this->dbms_,
                         DBException::QUERY_PREPARATION,
                         "Unsupported DBMS"));
    }
  return (update);
}

#endif /* !DB_CONNECTION_H_ */
