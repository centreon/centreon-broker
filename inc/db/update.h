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
    /**
     *  \class Update update.h "db/update.h"
     *  \brief Interface for an UPDATE query.
     *
     *  This class is the interface for UPDATE queries. DBMS will have to have
     *  their own concrete objects subclassing this interface.
     */
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

    /**
     *  \class MappedUpdate update.h "db/update.h"
     *  \brief Interface for an object-relational UPDATE query.
     *
     *  This class is the interface for mapped UPDATE queries. Just like
     *  Update, DBMS will have to have their own concrete objects subclassing
     *  this interface. This class is specific for object-relational updates.
     */
    template               <typename T>
    class                  MappedUpdate : public HaveInFields<T>,
                                          virtual public Update
    {
     protected:
      /**
       *  \brief MappedUpdate constructor.
       *
       *  Initialize members to their default values.
       *
       *  \param[in] mapping The Object-Relational mapping of the object.
       */
                           MappedUpdate(const Mapping<T>& mapping)
        : HaveInFields<T>(mapping) {}

      /**
       *  \brief MappedUpdate copy constructor.
       *
       *  Build a new MappedUpdate by copying data from the given object.
       *
       *  \param[in] mupdate Object to copy data from.
       */
                           MappedUpdate(const MappedUpdate& mupdate)
        : HaveInFields<T>(mupdate), Update(mupdate) {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data from the given object to the current instance.
       *
       *  \param[in] mupdate Object to copy data from.
       *
       *  \return *this
       */
      MappedUpdate&        operator=(const MappedUpdate& mupdate)
      {
	this->HaveInFields<T>::operator=(mupdate);
	this->Update::operator=(mupdate);
	return (*this);
      }

     public:
      /**
       *  \brief MappedUpdate destructor.
       *
       *  Release acquired ressources.
       */
      virtual              ~MappedUpdate() {}
    };
  }
}

#endif /* !DB_UPDATE_H_ */
