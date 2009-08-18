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

#ifndef DB_TRUNCATE_H_
# define DB_TRUNCATE_H_

# include "db/have_table.h"
# include "db/query.h"

namespace       CentreonBroker
{
  namespace     DB
  {
    /**
     *  \class Truncate truncate.h "db/truncate.h"
     *  \brief Interface for TRUNCATE queries.
     *
     *  Each DBMS provides a concrete object subclassing this interface, so
     *  that TRUNCATE queries can be executed independently of the database
     *  type.
     */
    class       Truncate : virtual public HaveTable,
                           virtual public Query
    {
     protected:
                Truncate();
                Truncate(const Truncate& truncate);
      Truncate& operator=(const Truncate& truncate);

     public:
      virtual   ~Truncate();
    };
  }
}

#endif /* !DB_TRUNCATE_H_ */
