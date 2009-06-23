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

# include <string>
# include "db/query.h"

namespace                CentreonBroker
{
  namespace              DB
  {
    /**
     *  Represents a table truncation.
     */
    class                Truncate : virtual public Query
    {
     private:
                         Truncate(const Truncate& truncate) throw ();
      Truncate&          operator=(const Truncate& truncate) throw ();

     protected:
      std::string        table_;

     public:
                         Truncate() throw ();
      virtual            ~Truncate();
      virtual void       Execute() = 0;
      const std::string& GetTable() const throw ();
      void               SetTable(const std::string& table);
    };
  }
}

#endif /* !DB_TRUNCATE_H_ */
