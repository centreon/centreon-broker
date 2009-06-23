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
    // Forward declarations
    template              <typename ObjectType>
    class                 Insert;
    template              <typename ObjectType>
    class                 Mapping;
    class                 Truncate;
    template              <typename ObjectType>
    class                 Update;

    /**
     *  Interface to handle a DB connection.
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
      template            <typename ObjectType>
      Insert<ObjectType>* GetInsertQuery(const Mapping<ObjectType>& mapping);
      virtual Truncate*   GetTruncateQuery() = 0;
      template            <typename ObjectType>
      Update<ObjectType>* GetUpdateQuery(const Mapping<ObjectType>& mapping);
    };
  }
}

# include <cassert>
# include "db/mysql/connection.h"
//# include "db/oracle/connection.h"
//# include "db/postgresql/connection.h"

/**
 *  Return an INSERT query matching the DBMS. Because we can't override a
 *  template, we will dynamic_cast the Connection to call the proper method.
 */
template <typename ObjectType>                    // Template
CentreonBroker::DB::Insert<ObjectType>*           // Return type
  CentreonBroker::DB::Connection::GetInsertQuery( // Method
    const Mapping<ObjectType>& mapping)           // Argument
{
  Insert<ObjectType>* insert;

  switch (this->dbms_)
    {
     case MYSQL:
      insert = dynamic_cast<MySQLConnection*>(this)
	->GetInsertQuery<ObjectType>(mapping);
      break ;
     case ORACLE:
     case POSTGRESQL:
     default:
      assert(false);
      throw (DBException(this->dbms_,
                         DBException::QUERY_PREPARATION,
                         "Unsupported DBMS"));
    }
  return (insert);
}

/**
 *  Return an UPDATE query matching the DBMS. Because we can't override a
 *  template, we will dynamic_cast the Connection to call the proper method.
 */
template <typename ObjectType>                    // Template
CentreonBroker::DB::Update<ObjectType>*           // Return type
  CentreonBroker::DB::Connection::GetUpdateQuery( // Method
    const Mapping<ObjectType>& mapping)           // Argument
{
  Update<ObjectType>* update;

  switch (this->dbms_)
    {
     case MYSQL:
      update = dynamic_cast<MySQLConnection*>(this)
	->GetUpdateQuery<ObjectType>(mapping);
      break ;
     case ORACLE:
     case POSTGRESQL:
     default:
      assert(false);
      throw (DBException(this->dbms_,
                         DBException::QUERY_PREPARATION,
                         "Unsupported DBMS"));
    }
  return (update);
}

#endif /* !DB_CONNECTION_H_ */
