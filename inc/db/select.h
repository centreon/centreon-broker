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

#ifndef DB_SELECT_H_
# define DB_SELECT_H_

# include <string>
# include "db/have_fields.h"
# include "db/have_predicate.h"
# include "db/have_table.h"
# include "db/query.h"

namespace              CentreonBroker
{
  namespace            DB
  {
    class              Select : public HaveFields,
                                virtual public HavePredicate,
                                public HaveTable,
                                virtual public Query
      {
       protected:
                       Select();
                       Select(const Select& select);
        Select&        operator=(const Select& select);

       public:
        virtual        ~Select();
        virtual bool   GetBool() = 0;
        virtual double GetDouble() = 0;
        virtual int    GetInt() = 0;
        virtual short  GetShort() = 0;
        virtual void   GetString(std::string& str) = 0;
        virtual bool   Next() = 0;
      };

    template           <typename T>
    class              MappedSelect : virtual public Select
    {
     protected:
                       MappedSelect();
                       MappedSelect(const MappedSelect& sm);
      MappedSelect&    operator=(const MappedSelect& sm);

     public:
      virtual          ~MappedSelect();
      void             Get(T& t);
    };
  }
}

#endif /* !DB_SELECT_H_ */
