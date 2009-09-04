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

#ifndef DB_MYSQL_HAVE_PREDICATE_H_
# define DB_MYSQL_HAVE_PREDICATE_H_

# include <string>
# include "db/have_predicate.h"
# include "db/predicate.h"

namespace                 CentreonBroker
{
  namespace               DB
  {
    /**
     *  \class MySQLHavePredicate have_predicate.h "db/mysql/have_predicate.h"
     *  \brief MySQL query with a predicate.
     *
     *  This class subclass PredicateVisitor to expand the predicate set in the
     *  query to a MySQL-specific string.
     */
    class                 MySQLHavePredicate : virtual public HavePredicate,
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
                          MySQLHavePredicate();
                          MySQLHavePredicate(const MySQLHavePredicate& myhp);
      virtual             ~MySQLHavePredicate();
      MySQLHavePredicate& operator=(const MySQLHavePredicate& myhp);

     public:
      void                PreparePredicate(std::string& query);
      void                ProcessPredicate(std::string& query);
    };
  }
}

#endif /* !DB_MYSQL_HAVE_PREDICATE_H_ */
