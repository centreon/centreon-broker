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
# include "db/select.h"

namespace                        CentreonBroker
{
  namespace                      DB
  {
    /**
     *  \class MappingGetters mapping.hpp "db/mapping.hpp"
     *  \brief Mapping to retrieve data from an object.
     *
     *  This class holds a map associating a field name with a boost::function.
     *  This map is used to extract data from an object of type T. The map can
     *  be defined by adding fields and their corresponding getters. Currently
     *  the map is only used with SQL queries defined within CentreonBroker.
     */
    template                     <typename T>
    class                        MappingGetters
    {
     public:
      std::map<std::string,
	       boost::function2<void, HaveArgs*, const T&> >
                                getters;

      /**
       *  MappingGetters default constructor.
       */
                                 MappingGetters() {}

      /**
       *  \brief MappingGetters copy constructor.
       *
       *  Copy the mapping of the given object to the current instance.
       *
       *  \param[in] mapping Object to copy data from.
       */
                                 MappingGetters(const MappingGetters& mapping)
        : getters(mapping.getters) {}

      /**
       *  MappingGetters destructor.
       */
      virtual                    ~MappingGetters() {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy the mapping of the given object to the current instance.
       *
       *  \param[in] mapping Object to copy data from.
       *
       *  \return *this
       */
      MappingGetters&            operator=(const MappingGetters& mapping)
      {
	this->getters = mapping.getters;
	return (*this);
      }

      /**
       *  Add a field of type bool to the mapping.
       *
       *  \param[in] name  The name of the field.
       *  \param[in] value The pointer to member corresponding to the field.
       */
      void                       AddField(const std::string& name,
                                          bool T::* value)
      {
        this->getters[name] = boost::bind(
          static_cast<void (HaveArgs::*)(bool)>(&HaveArgs::SetArg),
          _1,
          boost::bind(value, _2));
	return ;
      }

      /**
       *  Add a field of type double to the mapping.
       *
       *  \param[in] name  The name of the field.
       *  \param[in] value The pointer to member corresponding to the field.
       */
      void                       AddField(const std::string& name,
                                          double T::* value)
      {
        this->getters[name] = boost::bind(
          static_cast<void (HaveArgs::*)(double)>(&HaveArgs::SetArg),
          _1,
          boost::bind(value, _2));
        return ;
      }

      /**
       *  Add a field of type int to the mapping.
       *
       *  \param[in] name  The name of the field.
       *  \param[in] value The pointer to member corresponding to the field.
       */
      void                       AddField(const std::string& name,
                                          int T::* value)
      {
	this->getters[name] = boost::bind(
          static_cast<void (HaveArgs::*)(int)>(&HaveArgs::SetArg),
          _1,
          boost::bind(value, _2));
	return ;
      }

      /**
       *  Add a field of type short to the mapping.
       *
       *  \param[in] name  The name of the field.
       *  \param[in] value The pointer to member corresponding to the field.
       */
      void                       AddField(const std::string& name,
                                          short T::* value)
      {
        this->getters[name] = boost::bind(
          static_cast<void (HaveArgs::*)(short)>(&HaveArgs::SetArg),
          _1,
          boost::bind(value, _2));
	return ;
      }

      /**
       *  Add a field of type string to the mapping.
       *
       *  \param[in] name  The name of the field.
       *  \param[in] value The pointer to member corresponding to the field.
       */
      void                       AddField(const std::string& name,
                                          std::string T::* value)
      {
        this->getters[name] = boost::bind(
          static_cast<void (HaveArgs::*)(const std::string&)>(
            &HaveArgs::SetArg),
          _1,
          boost::bind(value, _2));
	return ;
      }

      /**
       *  Add a field of type time_t to the mapping.
       *
       *  \param[in] name  The name of the field.
       *  \param[in] value The pointer to member corresponding to the field.
       */
      void                       AddField(const std::string& name,
                                          time_t T::* value)
      {
	this->getters[name] = boost::bind(
          static_cast<void (HaveArgs::*)(time_t)>(&HaveArgs::SetArg),
          _1,
          boost::bind(value, _2));
	return ;
      }

      /**
       *  Release memory held internaly.
       */
      void                       Clear()
      {
	this->getters.clear();
	return ;
      }
    };

    // Convenience functions in anonymous namespace.
    namespace
    {
      template <typename T, typename U, typename V>
      void WrapperAssignment(T& t, U T::* member, const V& val)
      {
	t.*member = val;
	return ;
      }

      template <typename T>
      std::string& WrapperString(T& t, std::string T::* member)
      {
	return (t.*member);
      }
    }

    /**
     *  \class MappingSetters mapping.hpp "db/mapping.hpp"
     *  \brief Mapping to store data within an object.
     *
     *  This class holds a map associating a field name with a boost::function.
     *  This map is used to extract data from an object of type T. The map can
     *  be defined by adding fields and their corresponding getters. Currently
     *  the map is only used with SQL queries defined within CentreonBroker.
     */
    template             <typename T>
    class                MappingSetters
    {
     public:
      std::map<std::string,
               boost::function2<void, Select*, T&> >
                         setters;

      /**
       *  MappingSetters default constructor.
       */
                         MappingSetters() {}

      /**
       *  \brief MappingSetters copy constructor.
       *
       *  Copy the mapping of the given object to the current instance.
       *
       *  \param[in] mapping Object to copy data from.
       */
                         MappingSetters(const MappingSetters& mapping)
	: setters(mapping.setters) {}

      /**
       *  MappingSetters destructor.
       */
      virtual            ~MappingSetters() {}

      /**
       *  \brief Overload of the assignment operator.
       *
       *  Copy the mapping of the given object to the current instance.
       *
       *  \param[in] mapping Object to copy data from.
       *
       *  \return *this
       */
      MappingSetters<T>& operator=(const MappingSetters<T>& mapping)
      {
	this->setters = mapping.setters;
	return (*this);
      }

      /**
       *  Add a field of type bool to the mapping.
       *
       *  \param[in] name  The name of the field.
       *  \param[in] value The pointer to member corresponding to the field.
       */
      void               AddField(const std::string& name,
                                  bool T::* value)
      {
        this->setters[name] = boost::bind(
          &CentreonBroker::DB::WrapperAssignment<T, bool, bool>,
          _2,
          value,
          boost::bind(&CentreonBroker::DB::Select::GetBool, _1));
        return ;
      }

      /**
       *  Add a field of type double to the mapping.
       *
       *  \param[in] name  The name of the field.
       *  \param[in] value The pointer to member corresponding to the field.
       */
      void               AddField(const std::string& name,
                                  double T::* value)
      {
        this->setters[name] = boost::bind(
          &CentreonBroker::DB::WrapperAssignment<T, double, double>,
          _2,
          value,
          boost::bind(&CentreonBroker::DB::Select::GetDouble, _1));
	return ;
      }

      /**
       *  Add a field of type int to the mapping.
       *
       *  \param[in] name  The name of the field.
       *  \param[in] value The pointer to member corresponding to the field.
       */
      void               AddField(const std::string& name,
                                  int T::* value)
      {
	this->setters[name] = boost::bind(
          &CentreonBroker::DB::WrapperAssignment<T, int, int>,
          _2,
          value,
          boost::bind(&CentreonBroker::DB::Select::GetInt, _1));
	return ;
      }

      /**
       *  Add a field of type short to the mapping.
       *
       *  \param[in] name  The name of the field.
       *  \param[in] value The pointer to member corresponding to the field.
       */
      void               AddField(const std::string& name,
                                  short T::* value)
      {
	this->setters[name] = boost::bind(
          &CentreonBroker::DB::WrapperAssignment<T, short, short>,
          _2,
          value,
          boost::bind(&CentreonBroker::DB::Select::GetShort, _1));
	return ;
      }

      /**
       *  Add a field of type string to the mapping.
       *
       *  \param[in] name  The name of the field.
       *  \param[in] value The pointer to member corresponding to the field.
       */
      void               AddField(const std::string& name,
                                  std::string T::* value)
      {
	this->setters[name] = boost::bind(
          &CentreonBroker::DB::Select::GetString,
          _1,
          boost::bind(&CentreonBroker::DB::WrapperString<T>, _2, value));
	return ;
      }

      /**
       *  Add a field of type time_t to the mapping.
       *
       *  \param[in] name  The name of the field.
       *  \param[in] value The pointer to member corresponding to the field.
       */
      void       AddField(const std::string& name,
                          time_t T::* value)
      {
	this->setters[name] = boost::bind(
          &CentreonBroker::DB::WrapperAssignment<T, time_t, int>,
          _2,
          value,
          boost::bind(&CentreonBroker::DB::Select::GetInt, _1));
	return ;
      }

      /**
       *  Release memory help internaly.
       */
      void       Clear()
      {
	this->setters.clear();
	return ;
      }
    };
  }
}

#endif /* !DB_MAPPING_H_ */
