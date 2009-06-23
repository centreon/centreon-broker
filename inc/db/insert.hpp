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

#ifndef DB_INSERT_HPP_
# define DB_INSERT_HPP_

# include <cassert>
# include "db/have_fields.h"
# include "mapping.hpp"
# include "db/query.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  An INSERT query is based on fields so subclass Query and HaveFields.
     */
    template       <typename ObjectType>
    class          Insert : virtual public Query, virtual public HaveFields
    {
     private:
      /**
       *  Insert copy constructor.
       */
                   Insert(const Insert& insert) : Query(), HaveFields()
      {
	(void)insert;
	assert(false);
      }

      /**
       *  Insert operator= overload.
       */
      Insert&      operator=(const Insert& insert)
      {
	(void)insert;
	assert(false);
	return (*this);
      }

     protected:
      const Mapping<ObjectType>& mapping_;

     public:
      /**
       *  Insert default constructor.
       */
                   Insert(const Mapping<ObjectType>& mapping)
        : mapping_(mapping) {}

      /**
       *  Insert destructor.
       */
      virtual      ~Insert() {}

      /**
       *  Insert an element in the DB.
       */
      virtual void Execute(const ObjectType& obj) throw (DBException) = 0;
    };
  }
}

#endif /* !DB_INSERT_HPP_ */
