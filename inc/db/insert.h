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
    /**
     *  \class Insert insert.h "db/insert.h"
     *  \brief Interface for an INSERT query.
     *
     *  This interface is subclassed by concrete objects related to a specific
     *  DBMS. Users can then manipulate every INSERT query object in an
     *  abstract manner.
     */
    class          Insert : virtual public HaveFields,
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

    /**
     *  \brief MappedInsert insert.h "db/insert.h"
     *  \class Interface for a mapped INSERT query.
     *
     *  Interface to manipulate object-relational INSERT query. Each DBMS
     *  provides its own concrete implementation of this interface.
     */
    template       <typename T>
    class          MappedInsert : public HaveInFields<T>,
                                  virtual public Insert
    {
     protected:
      /**
       *  \brief MappedInsert constructor.
       *
       *  Initialize members to their default values.
       *
       *  \param[in] mapping The Object-Relational mapping of the object.
       */
                   MappedInsert(const MappingGetters<T>& mapping)
		     : HaveInFields<T>(mapping) {}

      /**
       *  \brief MappedInsert copy constructor.
       *
       *  Build the new MappedInsert object by copying data from the given
       *  object.
       *
       *  \param[in] minsert Object to copy data from.
       */
                   MappedInsert(const MappedInsert<T>& minsert)
        : HaveInFields<T>(minsert), Insert(minsert) {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data from the given object to the current instance.
       *
       *  \param[in] minsert Object to copy data from.
       *
       *  \return *this
       */
      MappedInsert<T>& operator=(const MappedInsert<T>& minsert)
      {
	this->HaveInFields<T>::operator=(minsert);
	this->Insert::operator=(minsert);
	return (*this);
      }

     public:
      /**
       *  \brief MappedInsert destructor.
       *
       *  Release acquired ressources.
       */
      virtual       ~MappedInsert() {}
    };
  }
}

#endif /* !DB_INSERT_H_ */
