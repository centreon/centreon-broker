/*
** update.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/10/09 Matthieu Kermagoret
*/

#ifndef DB_UPDATE_HPP_
# define DB_UPDATE_HPP_

# include <cassert>
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
        virtual public HavePredicate<ObjectType>
    {
     private:
      /**
       *  Update copy constructor.
       */
                   Update(const Update& update)
        : Query(),
          HaveFields(),
          HavePredicate<ObjectType>()
      {
	(void)update;
	assert(false);
      }

      /**
       *  Update operator= overload.
       */
      Update&      operator=(const Update& update)
      {
	(void)update;
	assert(false);
	return (*this);
      }

     protected:
      const Mapping<ObjectType>& mapping_;

     public:
      /**
       *  Update constructor.
       */
                   Update(const Mapping<ObjectType>& mapping)
        : HaveFields(), mapping_(mapping) {}

      /**
       *  Update destructor.
       */
      virtual      ~Update() {}

      /**
       *  Update an element in the DB.
       */
      virtual void Execute(const ObjectType& obj) = 0;

      /**
       *  Get the number of elements updated by the last query execution.
       */
      virtual int  GetUpdateCount() = 0;
    };
}
}

#endif /* !DB_UPDATE_HPP_ */
