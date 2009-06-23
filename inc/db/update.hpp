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

#ifndef DB_UPDATE_HPP_
# define DB_UPDATE_HPP_

# include <cassert>
# include "db/have_fields.h"
# include "db/have_predicate.hpp"
# include "db/mapping.hpp"
# include "db/query.h"

namespace CentreonBroker
{
  namespace DB
  {
    /**
     *  UPDATE query.
     */
    template       <typename ObjectType>
    class          Update
      : virtual public Query,
        virtual public HaveFields,
        virtual public HavePredicate<ObjectType>
    {
     private:
      /**
       *  Update copy constructor.
       */
                   Update(const Update& update)
        : Query(),
          HaveFields(),
          HavePredicate<ObjectType>()
      {
	(void)update;
	assert(false);
      }

      /**
       *  Update operator= overload.
       */
      Update&      operator=(const Update& update)
      {
	(void)update;
	assert(false);
	return (*this);
      }

     protected:
      const Mapping<ObjectType>& mapping_;

     public:
      /**
       *  Update constructor.
       */
                   Update(const Mapping<ObjectType>& mapping)
        : HaveFields(), mapping_(mapping) {}

      /**
       *  Update destructor.
       */
      virtual      ~Update() {}

      /**
       *  Update an element in the DB.
       */
      virtual void Execute(const ObjectType& obj) = 0;

      /**
       *  Get the number of elements updated by the last query execution.
       */
      virtual int  GetUpdateCount() = 0;
    };
}
}

#endif /* !DB_UPDATE_HPP_ */
