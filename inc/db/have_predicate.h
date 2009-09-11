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

#ifndef DB_HAVE_PREDICATE_H_
# define DB_HAVE_PREDICATE_H_

# include <memory>
# include "db/have_args.h"
# include "db/predicate.h"

namespace                      CentreonBroker
{
  namespace                    DB
  {
    /**
     *  \class HavePredicate have_predicate.h "db/have_predicate.h"
     *  \brief Queries having a predicate subclass this class.
     *
     *  All queries having a predicate subclass this class which provides an
     *  interface for setting the query predicate. How the predicate will be
     *  expanded is up to the concrete class.
     *
     *  \see Predicate
     */
    class                      HavePredicate : virtual public HaveArgs
    {
     protected:
      std::auto_ptr<Predicate> predicate;
                               HavePredicate() throw ();
                               HavePredicate(const HavePredicate& hp);
      virtual                  ~HavePredicate();
      HavePredicate&           operator=(const HavePredicate& hp);

     public:
      void                     SetPredicate(const Predicate& pred);
    };
  }
}

#endif /* !DB_HAVE_PREDICATE_H_ */
