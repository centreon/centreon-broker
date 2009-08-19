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
    /**
     *  \class Select select.h "db/select.h"
     *  \brief Interface for SELECT queries.
     *
     *  Interface of SELECT queries. Each DBMS will have to implement a
     *  concrete object subclassing this interface.
     */
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

    // Forward declaration
    template                   <typename T>
    class                      MappingSetters;

    /**
     *  \class MappedSelect select.h "db/select.h"
     *  \brief Interface for object-relational SELECT queries.
     *
     *  Interface to manipulate object-relational SELECT queries.
     */
    template                   <typename T>
    class                      MappedSelect : virtual public Select
    {
     private:
      const MappingSetters<T>& mapping_;
      std::list<boost::function2<void, Select*, T&> >
                               setters_;

     protected:
      /**
       *  \brief MappedSelect constructor.
       *
       *  Initialize members to their default values.
       *
       *  \param[in] mapping The Object-Relational mapping of the object.
       */
                               MappedSelect(const MappingSetters<T>& mapping)
        : mapping_(mapping)
      {
	for (typename std::map<std::string,
                               boost::function2<void,
                                                Select*,
                                                T&> >::const_iterator
               it = this->mapping_.fields_.begin();
	     it != this->mapping_.fields_.end();
	     it++)
	  this->AddField(it->first);
      }

      /**
       *  \brief MappedSelect copy constructor.
       *
       *  Build a new MappedSelect by copying data from the given object.
       *
       *  \param[in] ms Object to copy data from.
       */
                               MappedSelect(const MappedSelect& ms)
        : Select(ms), mapping_(ms.mapping_) {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data from the given object to the current instance.
       *
       *  \param[in] ms Object to copy data from.
       */
      MappedSelect&            operator=(const MappedSelect& ms)
      {
	this->Select::operator=(ms);
	return (*this);
      }

      /**
       *  Extract setters matching fields into a list.
       */
      void                     ExtractSetters()
      {
	for (std::list<std::string>::iterator it = this->fields.begin();
             it != this->fields.end();
             it++)
	  {
	    typename std::map<std::string,
                              boost::function2<void,
                                               Select*,
                                               T&> >::const_iterator s;
	    s = this->mapping_.fields_.find(*it);
	    this->setters_.push_back(s->second);
	  }
	return ;
      }

     public:
      /**
       *  \brief MappedSelect destructor.
       *
       *  Release acquired ressources.
       */
      virtual                  ~MappedSelect() {}

      /**
       *  Get the next object from the database.
       *
       *  \param[out] t Object to put data into.
       */
      virtual void             Get(T& t)
      {
	if (this->setters_.empty())
	  this->ExtractSetters();
	for (typename std::list<boost::function2<void,
                                                 Select*,
                                                 T&> >::iterator
               it = this->setters_.begin();
             it != this->getters_.end();
             it++)
	  (*it).operator()(this, t);
	return ;
      }
    };
  }
}

#endif /* !DB_SELECT_H_ */
