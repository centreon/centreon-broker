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

#ifndef DB_POSTGRESQL_INSERT_H_
# define DB_POSTGRESQL_INSERT_H_

# include <ctime>
# include <string>
# include "db/insert.h"
# include "db/postgresql/have_args.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  \class PgSQLInsert insert.h "db/postgresql/insert.h"
     *
     *  PostgreSQL INSERT query.
     *
     *  \see Insert
     */
    class          PgSQLInsert : virtual public Insert, public PgSQLHaveArgs
    {
     private:
      void         GenerateQueryBeginning();

     protected:
                   PgSQLInsert(const PgSQLInsert& pginsert);
      PgSQLInsert& operator=(const PgSQLInsert& pginsert);

     public:
                   PgSQLInsert(PGconn* pgconn);
      virtual      ~PgSQLInsert();
      void         Execute();
      unsigned int InsertId();
      void         Prepare();
      virtual void SetArg(bool arg);
      virtual void SetArg(double arg);
      virtual void SetArg(int arg);
      virtual void SetArg(short arg);
      virtual void SetArg(const std::string& arg);
      virtual void SetArg(time_t arg);
    };

    /**
     *  \class PgSQLMappedInsert insert.h "db/postgresql/insert.h"
     *
     *  Object-relational PostgreSQL INSERT query.
     *
     *  \see MappedInsert
     */
    template       <typename T>
    class          PgSQLMappedInsert : public MappedInsert<T>,
                                       public PgSQLInsert
    {
     protected:
      /**
       *  \brief PgSQLMappedInsert copy constructor.
       *
       *  Build the new mapped query by copying data from the given object.
       *
       *  \param[in] pgminsert Object to copy data from.
       */
                         PgSQLMappedInsert(const PgSQLMappedInsert& pgminsert)
	: MappedInsert<T>(pgminsert), PgSQLInsert(pgminsert) {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data of the given object to the current instance.
       *
       *  \param[in] pgminsert Object to copy data from.
       *
       *  \return *this
       */
      PgSQLMappedInsert& operator=(const PgSQLMappedInsert& pgminsert)
      {
	this->MappedInsert<T>::operator=(pgminsert);
	this->PgSQLInsert::operator=(pgminsert);
	return (*this);
      }

     public:
      /**
       *  \brief PgSQLMappedInsert constructor.
       *
       *  Build the PgSQLMappedInsert object. Needs the PostgreSQL connection
       *  object on which the query will be executed.
       *
       *  \param[in] pgconn  PostgreSQL connection object.
       *  \param[in] mapping Object-Relational mapping of the event type T.
       */
                         PgSQLMappedInsert(PGconn* pgconn,
                                           const MappingGetters<T>& mapping)
        : MappedInsert<T>(mapping), PgSQLInsert(pgconn) {}

      /**
       *  \brief PgSQLMappedInsert destructor.
       *
       *  Release acquired ressources.
       */
                         ~PgSQLMappedInsert() {}

      /**
       *  \brief Prepare the query.
       *
       *  Prepare the INSERT query on the PostgreSQL server.
       */
      void               Prepare()
      {
	this->MappedInsert<T>::ExtractGetters();
	this->PgSQLInsert::Prepare();
	return ;
      }
    };
  }
}

#endif /* !DB_POSTGRESQL_INSERT_H_ */
