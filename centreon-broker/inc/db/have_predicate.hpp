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
