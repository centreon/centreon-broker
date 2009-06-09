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

namespace CentreonBroker
{
namespace DB
{
    /**
     *  UPDATE query.
     */
    template       <typename ObjectType>
    class          Update
      : public Query,
        public HaveFields<ObjectType>,
        public HavePredicate<ObjectType>
    {
     public:
      /**
       *  Update constructor.
       */
                   Update(const Mapping<ObjectType>& mapping)
        : HaveFields<ObjectType>(mapping) {}

      /**
       *  Update copy constructor.
       */
                   Update(const Update& update)
        : Query(update),
          HaveFields<ObjectType>(update),
          HavePredicate<ObjectType>(update) {}

      /**
       *  Update destructor.
       */
      virtual      ~Update() {}

      /**
       *  Update operator= overload.
       */
      Update&      operator=(const Update& update)
      {
	this->Query::operator=(update);
	this->HaveFields<ObjectType>::operator=(update);
	this->HavePredicate<ObjectType>::operator=(update);
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
