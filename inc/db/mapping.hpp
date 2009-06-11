/*
** mapping.h for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/05/09 Matthieu Kermagoret
** Last update 06/11/09 Matthieu Kermagoret
*/

#ifndef DB_MAPPING_H_
# define DB_MAPPING_H_

# include <boost/bind.hpp>
# include <boost/function.hpp>
# include <map>
# include <string>
# include "db/have_fields.h"

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
                                HaveFields*,
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
      void                       AddDoubleField(const std::string& field,
						const boost::function1<double,
                                                  const ObjectType&>& getter)
      {
	this->fields_[field] = boost::bind(&HaveFields::SetDouble,
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
	this->fields_[field] = boost::bind(&HaveFields::SetInt,
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
	this->fields_[field] = boost::bind(&HaveFields::SetShort,
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
	this->fields_[field] = boost::bind(&HaveFields::SetString,
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
	this->fields_[field] = boost::bind(&HaveFields::SetTime,
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
