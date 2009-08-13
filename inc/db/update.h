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

#ifndef DB_UPDATE_H_
# define DB_UPDATE_H_

# include "db/have_fields.h"
# include "db/have_predicate.h"
# include "db/have_table.h"
# include "db/query.h"

namespace                  CentreonBroker
{
  namespace                DB
  {
    class                  Update : virtual public HaveFields,
                                    virtual public HavePredicate,
                                    public HaveTable,
                                    virtual public Query
    {
     protected:
                           Update();
                           Update(const Update& update);
      Update&              operator=(const Update& update);

     public:
      virtual              ~Update();
      virtual unsigned int GetUpdateCount() = 0;
    };
  }
}

#endif /* !DB_UPDATE_H_ */
