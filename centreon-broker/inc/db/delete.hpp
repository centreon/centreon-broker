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

#ifndef DB_DELETE_HPP_
# define DB_DELETE_HPP_

# include <cassert>
# include "db/have_fields.h"
# include "db/mapping.hpp"
# include "db/query.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  A DELETE query is based on fields so subclass Query and HaveFields.
     */
    template       <typename ObjectType>
    class          Delete : virtual public Query, virtual public HaveFields
    {
     private:
      /**
       *  Delete copy constructor.
       */
                   Delete(const Delete& del) : Query(), HaveFields()
      {
	(void)del;
	assert(false);
      }

      /**
       *  Delete operator= overload.
       */
      Delete&      operator=(const Delete& del)
      {
	(void)del;
	assert(false);
	return (*this);
      }

     protected:
      const        Mapping<ObjectType>& mapping_;

     public:
      /**
       *  Delete default constructor.
       */
                   Delete(const Mapping<ObjectType>& mapping)
        : mapping_(mapping) {}

      /**
       *  Delete destructor.
       */
      virtual      ~Delete() {}

      /**
       *  Delete an element in the DB.
       */
      virtual void Execute(const ObjectType& obj) throw (DBException) = 0;
    };
  }
}

#endif /* !DB_DELETE_HPP_ */
