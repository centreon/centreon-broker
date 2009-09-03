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

#ifndef DB_POSTGRESQL_UPDATE_H_
# define DB_POSTGRESQL_UPDATE_H_

# include <ctime>
# include <string>
# include "db/postgresql/have_args.h"
# include "db/postgresql/have_predicate.h"
# include "db/update.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  \class PgSQLUpdate update.h "db/postgresql/update.h"
     *
     *  PostgreSQL UPDATE query.
     *
     *  \see Update
     */
    class          PgSQLUpdate : public PgSQLHaveArgs,
                                 public PgSQLHavePredicate,
                                 virtual public Update
    {
     private:
      std::list<std::string>::iterator
                   field_;
      void         CheckArg();
      void         GenerateQueryBeginning();

     protected:
                   PgSQLUpdate(const PgSQLUpdate& pgupdate);
      PgSQLUpdate& operator=(const PgSQLUpdate& pgupdate);

     public:
                   PgSQLUpdate(PGconn* pgconn);
      virtual      ~PgSQLUpdate();
      void         Execute();
      unsigned int GetUpdateCount();
      virtual void Prepare();
      virtual void SetArg(bool arg);
      virtual void SetArg(double arg);
      virtual void SetArg(int arg);
      virtual void SetArg(short arg);
      virtual void SetArg(const std::string& arg);
      virtual void SetArg(time_t arg);
    };

    /**
     *  \class PgSQLMappedUpdate update.h "db/postgresql/update.h"
     *
     *  Object-relational PostgreSQL UPDATE query.
     *
     *  \see MappedUpdate
     */
    template             <typename T>
    class                PgSQLMappedUpdate : public MappedUpdate<T>,
                                             public PgSQLUpdate
    {
     protected:
      /**
       *  \brief PgSQLMappedUpdate copy constructor.
       *
       *  Build the new mapped query by copying data from the given object.
       *
       *  \param[in] pgmupdate Object to copy data from.
       */
                         PgSQLMappedUpdate(const PgSQLMappedUpdate& pgmupdate)
	: MappedUpdate<T>(pgmupdate), PgSQLUpdate(pgmupdate) {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data of the given object to the current instance.
       *
       *  \param[in] pgmupdate Object to copy data from.
       *
       *  \return *this
       */
      PgSQLMappedUpdate& operator=(const PgSQLMappedUpdate& pgmupdate)
      {
        this->MappedUpdate<T>::operator=(pgmupdate);
        this->PgSQLUpdate::operator=(pgmupdate);
        return (*this);
      }

     public:
      /**
       *  \brief PgSQLMappedUpdate constructor.
       *
       *  Build the PgSQLMappedUpdate object. Needs the PostgreSQL connection
       *  object on which the query will be executed.
       *
       *  \param[in] pgconn  PostgreSQL connection object.
       *  \param[in] mapping Object-Relational mapping of the event type T.
       */
                         PgSQLMappedUpdate(PGconn* pgconn,
                                           const MappingGetters<T>& mapping)
        : MappedUpdate<T>(mapping), PgSQLUpdate(pgconn) {}

      /**
       *  \brief PgSQLMappedUpdate destructor.
       *
       *  Release acquired ressources.
       */
                         ~PgSQLMappedUpdate() {}
    };
  }
}

#endif /* !DB_POSTGRESQL_UPDATE_H_ */
