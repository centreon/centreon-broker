/*
** dynamic_predicate.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/08/09 Matthieu Kermagoret
** Last update 06/08/09 Matthieu Kermagoret
*/

#ifndef DB_DYNAMIC_PREDICATE_H_
# define DB_DYNAMIC_PREDICATE_H_

# include <boost/function.hpp>
# include "db/predicate.h"

namespace CentreonBroker
{
  namespace DB
  {
    // Forward declaration
    template <typename ObjectType> class DynamicPredicateVisitor;

    /**
     *  This class represents a dynamic predicate, which is a predicate that
     *  has to be evaluated on query execution.
     */
    template            <typename ObjectType>
    class               DynamicPredicate : public Predicate
    {
     public:
      /**
       *  DynamicPredicate default constructor.
       */
                        DynamicPredicate() throw () {}

      /**
       *  DynamicPredicate copy constructor.
       */
                        DynamicPredicate(const DynamicPredicate& dp) throw ()
	: Predicate(dp)
      {
	(void)dp;
      }

      /**
       *  DynamicPredicate destructor.
       */
      virtual           ~DynamicPredicate() {}

      /**
       *  DynamicPredicate operator= overload.
       */
      DynamicPredicate& operator=(const DynamicPredicate& dp) throw ()
      {
	this->Predicate::operator=(dp);
	return (*this);
      }
    };


    /**
     *  This class represents something like a terminal except that it has to
     *  evaluated at query execution and not before.
     */
    template        <typename ObjectType>
    class           DynamicDouble : public DynamicPredicate<ObjectType>
    {
     private:
      boost::function1<double, const ObjectType&> getter_;

      /**
       *  This method will copy all internal data of the given object to the
       *  current instance.
       */
      void          InternalCopy(const DynamicDouble& dd)
      {
	this->getter_ = dd.getter_;
	return ;
      }

     public:
      /**
       *  DynamicDouble default constructor.
       */
                    DynamicDouble(const boost::function1<double,
                                                         const ObjectType&>&
                                    getter)
        : getter_(getter) {}

      /**
       *  DynamicDouble copy constructor.
       */
                    DynamicDouble(const DynamicDouble& dd)
        : DynamicPredicate<ObjectType>(dd)
      {
	this->InternalCopy(dd);
      }

      /**
       *  DynamicDouble destructor.
       */
                    ~DynamicDouble() {}

      /**
       *  DynamicDouble operator= overload.
       */
      DynamicDouble &operator=(const DynamicDouble& dd)
      {
	this->DynamicPredicate<ObjectType>::operator=(dd);
	this->InternalCopy(dd);
	return (*this);
      }

      /**
       *  Accept a Visitor and show him the inside.
       */
      void          Accept(PredicateVisitor& visitor)
      {
	static_cast<DynamicPredicateVisitor<ObjectType>&>(visitor).Visit(
          this->getter_);
	return ;
      }
    };

    /**
     *  Objects that want to unroll dynamic predicates will have to subclass
     *  this class.
     */
    template                   <typename ObjectType>
    class                      DynamicPredicateVisitor : public PredicateVisitor
    {
     public:
      /**
       *  DynamicPredicateVisitor default constructor.
       */
                               DynamicPredicateVisitor() throw () {}

      /**
       *  DynamicPredicateVisitor copy constructor.
       */
                               DynamicPredicateVisitor(
                                 const DynamicPredicateVisitor& dpv)
        throw () : PredicateVisitor(dpv)
      {
      }

      /**
       *  DynamicPredicateVisitor destructor.
       */
      virtual                  ~DynamicPredicateVisitor()
      {
      }

      /**
       *  DynamicPredicateVisitor operator= overload.
       */
      DynamicPredicateVisitor& operator=(const DynamicPredicateVisitor& dpv)
	throw ()
      {
	this->PredicateVisitor::operator=(dpv);
	return (*this);
      }

      /**
       *  Overloaded function that'll have to be overriden by subclass.
       */
      virtual void             Visit(const boost::function1<double,
                                 const ObjectType&>& func) = 0;
    };
  }
}

#endif /* !DB_DYNAMIC_PREDICATE_H_ */
