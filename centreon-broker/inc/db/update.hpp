/*
** update.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/29/09 Matthieu Kermagoret
** Last update 05/29/09 Matthieu Kermagoret
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
  };
}

#endif /* !DB_UPDATE_HPP_ */
