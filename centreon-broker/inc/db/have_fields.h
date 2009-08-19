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

#ifndef DB_HAVE_FIELDS_H_
# define DB_HAVE_FIELDS_H_

# include <boost/function.hpp>
# include <list>
# include <map>
# include <string>
# include "db/have_args.h"

namespace                    CentreonBroker
{
  namespace                  DB
  {
    /**
     *  \class HaveFields have_fields.h "db/have_fields.h"
     *  \brief Query that executes on specific fields.
     *
     *  This class provides method to include or exclude specific fields.
     */
    class                    HaveFields : virtual public HaveArgs
    {
     protected:
      std::list<std::string> fields;
                             HaveFields();
                             HaveFields(const HaveFields& hf);
      HaveFields&            operator=(const HaveFields& hf);
      virtual                ~HaveFields();

     public:
      void                   AddField(const std::string& field);
      void                   RemoveField(const std::string& field);
    };

    // Forward declaration
    template                   <typename T>
    class                      MappingGetters;

    /**
     *  \class HaveInFields have_fields.h "db/have_fields.h"
     *  \brief Query accepting fields as input parameters.
     *
     *  This class provides object-relational mapping facility for field-based
     *  queries that accept fields as input parameters (INSERT and UPDATE for
     *  example).
     *
     *  \see Insert
     *  \see Update
     */
    template                   <typename T>
    class                      HaveInFields : virtual public HaveFields
    {
     private:
      std::list<boost::function2<void, HaveArgs*, const T&> >
                               getters_;
      const MappingGetters<T>& mapping_;

     protected:
      /**
       *  \brief HaveInFields constructor.
       *
       *  Build the HaveInFields object.
       *
       *  \param[in] mapping Object-Relational mapping of the event type T.
       */
                               HaveInFields(const MappingGetters<T>& mapping)
        : mapping_(mapping)
      {
	for (typename std::map<std::string,
                               boost::function2<void,
                                                HaveArgs*,
                                                const T&> >::const_iterator
               it = this->mapping_.getters.begin();
	     it != this->mapping_.getters.end();
	     it++)
	  this->AddField(it->first);
      }

      /**
       *  \brief HaveInFields copy constructor.
       *
       *  Build a new HaveInFields by copying data from the given object.
       *
       *  \param[in] hif Object to copy data from.
       */
                               HaveInFields(const HaveInFields& hif)
        : HaveFields(hif), mapping_(hif.mapping_) {}


      /**
       *  \brief HaveInFields destructor.
       *
       *  Release acquired ressources.
       */
      virtual                  ~HaveInFields() {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy data of the given HaveInFields object to the current instance.
       *
       *  \param[in] hif Object to copy data from.
       *
       *  \return *this
       */
      HaveInFields             operator=(const HaveInFields& hif)
      {
	this->HaveFields::operator=(hif);
	return (*this);
      }

      /**
       *  Extract getters matching fields into a list.
       */
      void                     ExtractGetters()
      {
	for (std::list<std::string>::iterator it = this->fields.begin();
	     it != this->fields.end();
	     it++)
	  {
	    typename std::map<std::string,
                              boost::function2<void,
                                               HaveArgs*,
                                               const T&> >::const_iterator g;

	    g = this->mapping_.getters.find(*it);
	    this->getters_.push_back(g->second);
	  }
	return ;
      }

     public:
      /**
       *  \brief Set the arguments.
       *
       *  Set the arguments using object-relational mapping.
       *
       *  \param[in] arg Object containing data.
       */
      void                     SetArg(const T& arg)
      {
	if (this->getters_.empty())
	  this->ExtractGetters();
	for (typename std::list<boost::function2<void,
                                                 HaveArgs*,
                                                 const T&> >::iterator
               it = this->getters_.begin();
             it != this->getters_.end();
             it++)
	  (*it).operator()(this, arg);
	return ;
      }
    };
  }
}

#endif /* !DB_HAVE_FIELDS_H_ */
