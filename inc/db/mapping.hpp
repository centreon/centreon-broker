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

#ifndef DB_MAPPING_H_
# define DB_MAPPING_H_

# include <boost/bind.hpp>
# include <boost/function.hpp>
# include <map>
# include <string>
# include "db/have_args.h"

namespace                        CentreonBroker
{
  namespace                      DB
  {
    class                        HaveFields;

    template                     <typename ObjectType>
    class                        Mapping
    {
     private:
      /**
       *  Make a copy of all internal data structures of the Mapping object to
       *  the current instance.
       */
      void                       InternalCopy(const Mapping& mapping)
      {
	this->fields_ = mapping.fields_;
	this->table_ = mapping.table_;
	return ;
      }

     public:
      std::map<std::string,
	       boost::function2<void,
                                HaveArgs*,
                                const ObjectType&>
                               > fields_;
      std::string                table_;

      /**
       *  Mapping default constructor.
       */
                                 Mapping() {}

      /**
       *  Mapping copy constructor.
       */
                                 Mapping(const Mapping& mapping)
      {
	this->InternalCopy(mapping);
      }

      /**
       *  Mapping destructor.
       */
      virtual                    ~Mapping() {}

      /**
       *  Mapping operator= overload.
       */
      Mapping&                   operator=(const Mapping& mapping)
      {
	this->InternalCopy(mapping);
	return (*this);
      }

      /**
       *  Add a field and its associated getter to the field list.
       */
      void                       AddBoolField(const std::string& field,
					      const boost::function1<bool,
					      const ObjectType&>& getter)
      {
        this->fields_[field] = boost::bind(
          static_cast<void (HaveArgs::*)(bool)>(&HaveArgs::SetArg),
          _1,
          boost::bind(getter, _2));
	return ;
      }

      /**
       *  Add a field and its associated getter to the field list.
       */
      void                       AddDoubleField(const std::string& field,
						const boost::function1<double,
                                                  const ObjectType&>& getter)
      {
	this->fields_[field] = boost::bind(
          static_cast<void (HaveArgs::*)(double)>(&HaveArgs::SetArg),
          _1,
          boost::bind(getter, _2));
	return ;
      }

      /**
       *  Add a field and its associated getter to the field list.
       */
      void                       AddIntField(const std::string& field,
					     const boost::function1<int,
                                               const ObjectType&>& getter)
      {
	this->fields_[field] = boost::bind(
          static_cast<void (HaveArgs::*)(int)>(&HaveArgs::SetArg),
          _1,
          boost::bind(getter, _2));
	return ;
      }

      /**
       *  Add a field and its associated getter to the field list.
       */
      void                       AddShortField(const std::string& field,
					       const boost::function1<short,
                                                 const ObjectType&>& getter)
      {
        this->fields_[field] = boost::bind(
          static_cast<void (HaveArgs::*)(short)>(&HaveArgs::SetArg),
          _1,
          boost::bind(getter, _2));
	return ;
      }

      /**
       *  Add a field and its associated getter to the field list.
       */
      void                       AddStringField(const std::string& field,
						const boost::function1<
						  const std::string&,
                                                  const ObjectType&>& getter)
      {
	this->fields_[field] = boost::bind(
          static_cast<void (HaveArgs::*)(const std::string&)>(
            &HaveArgs::SetArg),
          _1,
          boost::bind(getter, _2));
	return ;
      }

      /**
       *  Add a field and its associated getter to the field list.
       */
      void                       AddTimeField(const std::string& field,
					      const boost::function1<time_t,
                                                  const ObjectType&>& getter)
      {
	this->fields_[field] = boost::bind(
          static_cast<void (HaveArgs::*)(time_t)>(&HaveArgs::SetArg),
          _1,
          boost::bind(getter, _2));
	return ;
      }

      /**
       *  Get the table name.
       */
      const std::string&         GetTable() const throw ()
      {
	return (this->table_);
      }

      /**
       *  Set the table name.
       */
      void                       SetTable(const std::string& table)
      {
	this->table_ = table;
	return ;
      }
    };
  }
}

#endif /* !DB_MAPPING_H_ */
