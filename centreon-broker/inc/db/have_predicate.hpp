/*
** have_predicate.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/10/09 Matthieu Kermagoret
** Last update 06/11/09 Matthieu Kermagoret
*/

#ifndef DB_HAVE_PREDICATE_HPP_
# define DB_HAVE_PREDICATE_HPP_

# include <boost/smart_ptr.hpp>
# include <cassert>
# include "db/dynamic_predicate.hpp"

namespace CentreonBroker
{
  namespace DB
  {
    /**
     *  When queries accept predicates, they subclass this class.
     */
    template                       <typename ObjectType>
    class                          HavePredicate
      : public DynamicPredicateVisitor<ObjectType>
    {
     private:
      /**
       *  HavePredicate copy constructor.
       */
                                   HavePredicate(const HavePredicate& hp)
        : DynamicPredicateVisitor<ObjectType>()
      {
	(void)hp;
	assert(false);
      }

      /**
       *  HavePredicate operator= overload.
       */
      HavePredicate&               operator=(const HavePredicate& hp)
      {
	this->DynamicPredicateVisitor<ObjectType>::operator=(hp);
	this->predicate_ = hp.predicate_;
	return (*this);
      }

     protected:
      Predicate*                   predicate_;

     public:
      /**
       *  HavePredicate default constructor.
       */
                                   HavePredicate() : predicate_(NULL) {}

      /**
       *  HavePredicate destructor.
       */
      virtual                      ~HavePredicate()
      {
	if (this->predicate_)
	  delete (this->predicate_);
      }

      /**
       *  Set the predicate.
       */
      template                     <typename PredicateType>
      void                         SetPredicate(const PredicateType& predicate)
      {
	if (this->predicate_)
	  {
            delete (this->predicate_);
            this->predicate_ = NULL;
	  }
        this->predicate_ = new PredicateType(predicate);
        return ;
      }
    };
  }
}

#endif /* !DB_HAVE_PREDICATE_HPP_ */
