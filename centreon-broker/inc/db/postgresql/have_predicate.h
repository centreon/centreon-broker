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

#ifndef DB_POSTGRESQL_HAVE_PREDICATE_H_
# define DB_POSTGRESQL_HAVE_PREDICATE_H_

# include <string>
# include "db/have_predicate.h"
# include "db/predicate.h"

namespace                 CentreonBroker
{
  namespace               DB
  {
    /**
     *  \class PgSQLHavePredicate have_predicate.h "db/postgresql/have_predicate.h"
     *  \brief PostgreSQL query with a predicate.
     *
     *  This class subclass PredicateVisitor to expand the predicate set in the
     *  query to a PostgreSQL-specific string.
     */
    class                 PgSQLHavePredicate : virtual public HavePredicate,
                                               private PredicateVisitor
    {
     private:
      std::string*        query_;
      void                Visit(const And& a_n_d);
      void                Visit(const Equal& equal);
      void                Visit(const Field& field);
      void                Visit(const Placeholder& placeholder);
      void                Visit(const Terminal& terminal);

     protected:
      unsigned int        placeholders;
                          PgSQLHavePredicate();
                          PgSQLHavePredicate(const PgSQLHavePredicate& pghp);
      virtual             ~PgSQLHavePredicate();
      PgSQLHavePredicate& operator=(const PgSQLHavePredicate& pghp);

     public:
      void                PreparePredicate(std::string& query);
      void                ProcessPredicate(std::string& query);
    };
  }
}

#endif /* !DB_POSTGRESQL_HAVE_PREDICATE_H_ */
