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

#ifndef DB_POSTGRESQL_SELECT_H_
# define DB_POSTGRESQL_SELECT_H_

# include <libpq-fe.h>
# include <string>
# include "db/postgresql/have_args.h"
# include "db/postgresql/have_predicate.h"
# include "db/select.h"
# include "mapping.h"

namespace            CentreonBroker
{
  namespace          DB
  {
    /**
     *  \class PgSQLSelect select.h "db/postgresql/select.h"
     *
     *  PostgreSQL SELECT query
     *
     *  \see Select
     */
    class            PgSQLSelect : virtual public Select,
                                   public PgSQLHaveArgs,
                                   public PgSQLHavePredicate
    {
     private:
      unsigned int   current_col_;
      unsigned int   current_row_;
                     PgSQLSelect(const PgSQLSelect& pgs);
      PgSQLSelect&   operator=(const PgSQLSelect& pgs);
      void           GenerateQueryStart();

     public:
                     PgSQLSelect(PGconn* pgconn);
      virtual        ~PgSQLSelect();
      virtual void   Execute();
      virtual bool   GetBool();
      virtual double GetDouble();
      virtual int    GetInt();
      virtual short  GetShort();
      virtual void   GetString(std::string& str);
      virtual bool   Next();
      virtual void   Prepare();
    };

    /**
     *  \class PgSQLMappedSelect select.h "db/postgresql/select.h"
     *
     *  Object-relational PostgreSQL SELECT query.
     *
     *  \see MappedSelect
     */
    template             <typename T>
    class                PgSQLMappedSelect : public MappedSelect<T>,
                                             public PgSQLSelect
    {
     protected:
      /**
       *  \brief PgSQLMappedSelect copy constructor.
       *
       *  Build the new mapped query by copying data from the given object.
       *
       *  \param[in] pgms Object to copy data from.
       */
                         PgSQLMappedSelect(const PgSQLMappedSelect& pgms)
        : MappedSelect<T>(pgms), PgSQLSelect(pgms) {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data of the given object to the current instance.
       *
       *  \param[in] pgms Object to copy data from.
       *
       *  \return *this
       */
      PgSQLMappedSelect& operator=(const PgSQLMappedSelect& pgms)
      {
	this->MappedSelect<T>::operator=(pgms);
	this->PgSQLSelect::operator=(pgms);
	return (*this);
      }

     public:
      /**
       *  \brief PgSQLMappedSelect constructor.
       *
       *  Build the PgSQLMappedSelect object. Needs the PostgreSQL connection
       *  object on which the query will be executed.
       *
       *  \param[in] pgconn  PgSQL connection object.
       *  \param[in] mapping Object-Relational mapping of the event type T.
       */
                         PgSQLMappedSelect(PGconn* pgconn,
                                           const MappingSetters<T>& mapping)
	: MappedSelect<T>(mapping), PgSQLSelect(pgconn) {}

      /**
       *  \brief PgSQLMappedInsert destructor.
       *
       *  Release acquired ressources.
       */
                         ~PgSQLMappedSelect() {}
    };
  }
}

#endif /* !DB_POSTGRESQL_SELECT_H_ */
