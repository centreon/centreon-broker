/*
** insert.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/09/09 Matthieu Kermagoret
** Last update 06/10/09 Matthieu Kermagoret
*/

#ifndef DB_INSERT_HPP_
# define DB_INSERT_HPP_

# include <cassert>
# include "db/have_fields.h"
# include "mapping.hpp"
# include "db/query.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  An INSERT query is based on fields so subclass Query and HaveFields.
     */
    template       <typename ObjectType>
    class          Insert : virtual public Query, virtual public HaveFields
    {
     private:
      /**
       *  Insert copy constructor.
       */
                   Insert(const Insert& insert) : Query(), HaveFields()
      {
	(void)insert;
	assert(false);
      }

      /**
       *  Insert operator= overload.
       */
      Insert&      operator=(const Insert& insert)
      {
	(void)insert;
	assert(false);
	return (*this);
      }

     protected:
      const Mapping<ObjectType>& mapping_;

     public:
      /**
       *  Insert default constructor.
       */
                   Insert(const Mapping<ObjectType>& mapping)
        : mapping_(mapping) {}

      /**
       *  Insert destructor.
       */
      virtual      ~Insert() {}

      /**
       *  Insert an element in the DB.
       */
      virtual void Execute(const ObjectType& obj) throw (DBException) = 0;
    };
  }
}

#endif /* !DB_INSERT_HPP_ */
