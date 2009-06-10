/*
** update.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
*/

#ifndef DB_UPDATE_HPP_
# define DB_UPDATE_HPP_

# include "db/have_fields.h"
# include "db/have_predicate.hpp"
# include "db/mapping.hpp"
# include "db/query.h"

namespace CentreonBroker
{
  namespace DB
  {
    /**
     *  UPDATE query.
     */
    template       <typename ObjectType>
    class          Update
      : virtual public Query,
        virtual public HaveFields,
        public HavePredicate<ObjectType>
    {
     public:
      /**
       *  Update constructor.
       */
                   Update(const Mapping<ObjectType>& mapping)
        : HaveFields() {}

      /**
       *  Update copy constructor.
       */
                   Update(const Update& update)
        : Query(),
          HaveFields(),
          HavePredicate<ObjectType>() {}

      /**
       *  Update destructor.
       */
      virtual      ~Update() {}

      /**
       *  Update operator= overload.
       */
      Update&      operator=(const Update& update)
      {
	return (*this);
      }

      /**
       *  Update an element in the DB.
       */
      virtual void Execute(const ObjectType& obj);
    };
}
}

#endif /* !DB_UPDATE_HPP_ */
