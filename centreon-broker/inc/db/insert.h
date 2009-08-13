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

#ifndef DB_INSERT_H_
# define DB_INSERT_H_

# include "db/have_fields.h"
# include "db/have_table.h"
# include "db/query.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    class          Insert : public HaveFields,
                            public HaveTable,
                            virtual public Query
    {
     protected:
                   Insert();
                   Insert(const Insert& insert);
      Insert&      operator=(const Insert& insert);

     public:
      virtual      ~Insert();
    };
  }
}

#endif /* !DB_INSERT_H_ */
