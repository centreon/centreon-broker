/*
** mapping.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/29/09 Matthieu Kermagoret
** Last update 05/30/09 Matthieu Kermagoret
*/

#ifndef DB_MAPPING_HPP_
# define DB_MAPPING_HPP_

# include <map>
# include <string>

namespace CentreonBroker
{
  // Forward declarations.
  template <typename ObjectType> class FieldSetter;
  template <typename ObjectType> class Query;

  /**
   *  This class contains the definition of an object mapping.
   */
  template <typename ObjectType>
  struct     Mapping
  {
    friend class Query<ObjectType>;

   public:
    std::string table_;
    std::map<std::string, const FieldSetter<ObjectType>*> setters_;

    /**
     *  Mapping default constructor.
     */
             Mapping() {}

    /**
     *  Mapping copy constructor.
     */
             Mapping(const Mapping& mapping)
    {
      this->operator=(mapping);
    }

    /**
     *  Mapping destructor.
     */
             ~Mapping() {}

    /**
     *  Mapping operator= overload.
     */
    Mapping& operator=(const Mapping& mapping)
    {
      this->table_ = mapping.table_;
      this->setters_ = mapping.setters_;
      return (*this);
    }

    /**
     *  Remove a field from the mapping.
     */
    Mapping& operator>>(const std::string& field)
    {
      // XXX : does this really works ?
      decltype(this->setters_.find(field)) it = this->setters_.find(field);
      if (it != this->setters_.end())
	this->setters_.erase(it);
      return (*this);
    }

    /**
     *  Add a field and its mapped value to the mapping.
     */
    void     AddField(const std::string& field,
                      const FieldSetter<ObjectType>* setter)
    {
      this->setters_[field] = setter;
      return ;
    }

    /**
     *  Set the table name.
     */
    void     SetTable(const std::string& table)
    {
      this->table_ = table;
      return ;
    }
  };
}

#endif /* !DB_MAPPING_HPP_ */
