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

#ifndef DB_QUERY_H_
# define DB_QUERY_H_

# include "db/db_exception.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  This is the root class of all queries. Every query can be prepared
     *  using the appropriate feature of the DBMS.
     */
    class          Query
    {
     private:
                   Query(const Query& query) throw ();
      Query&       operator=(const Query& query) throw ();

     public:
                   Query() throw ();
      virtual      ~Query();
      virtual void Prepare() = 0;
    };
  }
}

#endif /* !DB_QUERY_H_ */
