/*
** insert.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/29/09 Matthieu Kermagoret
** Last update 05/29/09 Matthieu Kermagoret
*/

#ifndef DB_INSERT_HPP_
# define DB_INSERT_HPP_

# include "db/query.hpp"

namespace   CentreonBroker
{
  /**
   *  This template encapsulates an INSERT query.
   */
  template  <typename ObjectType>
  class     Insert : public Query<ObjectType>
  {
   private:
    /**
     *  Insert copy constructor.
     */
            Insert(const Insert& insert) : Query<ObjectType>(insert.mapping_)
    {
      (void)insert;
    }

    /**
     *  Insert operator= overload.
     */
    Insert& operator=(const Insert& insert)
    {
      (void)insert;
      return (*this);
    }

   public:
    /**
     *  Insert default constructor.
     */
            Insert(const Mapping<ObjectType>& mapping)
      : Query<ObjectType>(mapping) {}

    /**
     *  Insert destructor.
     */
    virtual ~Insert() {}
  };
}

#endif /* !DB_INSERT_HPP_ */
