/*
** update.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/29/09 Matthieu Kermagoret
** Last update 06/01/09 Matthieu Kermagoret
*/

#ifndef DB_UPDATE_HPP_
# define DB_UPDATE_HPP_

# include "db/query.hpp"

namespace CentreonBroker
{
  /**
   *  This template encapsulates an UPDATE query.
   */
  template  <typename ObjectType>
  class     Update : public Query<ObjectType>
  {
   private:
    /**
     *  Update copy constructor.
     */
            Update(const Update& update) : Query<ObjectType>(update.mapping_)
    {
      (void)update;
    }

    /**
     *  Update operator= overload.
     */
    Update& operator=(const Update& update)
    {
      (void)update;
      return (*this);
    }

   protected:
    std::map<std::string, const FieldSetter<ObjectType>*> uniques_;

   public:
    /**
     *  Update default constructor.
     */
            Update(const Mapping<ObjectType>& mapping)
      : Query<ObjectType>(mapping) {}

    /**
     *  Update destructor.
     */
    virtual ~Update() {}

    /**
     *  Add a UNIQUE field that will help update the object.
     */
    void    AddUnique(const std::string& field,
                      const FieldSetter<ObjectType>* setter)
    {
      this->uniques_[field] = setter;
      return ;
    }
  };
}

#endif /* !DB_UPDATE_HPP_ */
