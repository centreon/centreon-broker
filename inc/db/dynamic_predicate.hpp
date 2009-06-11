/*
** dynamic_predicate.hpp for CentreonBroker in ./inc/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/08/09 Matthieu Kermagoret
** Last update 06/11/09 Matthieu Kermagoret
*/

#ifndef DB_DYNAMIC_PREDICATE_H_
# define DB_DYNAMIC_PREDICATE_H_

# include <boost/function.hpp>
# include <cassert>
# include <string>
# include "db/predicate.h"

namespace               CentreonBroker
{
  namespace             DB
  {
    // The DynamicPredicateVisitor will be defined later.
    template            <typename ObjectType>
    class               DynamicPredicateVisitor;

    /**************************************************************************
    *                                                                         *
    *                                                                         *
    *                            DynamicPredicate                             *
    *                                                                         *
    *                                                                         *
    **************************************************************************/

    /**
     *  This class represents a dynamic predicate, which is a predicate that
     *  has to be evaluated on query execution.
     */
    template            <typename ObjectType>
    class               DynamicPredicate : public Predicate
    {
     private:
      /**
       *  DynamicPredicate copy constructor.
       */
                        DynamicPredicate(const DynamicPredicate& dp) throw ()
	: Predicate()
      {
	(void)dp;
	assert(false);
      }

      /**
       *  DynamicPredicate operator= overload.
       */
      DynamicPredicate& operator=(const DynamicPredicate& dp) throw ()
      {
	(void)dp;
	assert(false);
	return (*this);
      }

     public:
      /**
       *  DynamicPredicate default constructor.
       */
                        DynamicPredicate() throw () {}

      /**
       *  DynamicPredicate destructor.
       */
      virtual           ~DynamicPredicate() {}
    };


    /**************************************************************************
    *                                                                         *
    *                                                                         *
    *                             DynamicDouble                               *
    *                                                                         *
    *                                                                         *
    **************************************************************************/

    /**
     *  This class represents something like a terminal except that it has to
     *  evaluated at query execution and not before.
     */
    template        <typename ObjectType>
    class           DynamicDouble : public DynamicPredicate<ObjectType>
    {
     private:
      const boost::function1<double, const ObjectType&> getter_;

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
        : DynamicPredicate<ObjectType>()
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
	this->InternalCopy(dd);
	return (*this);
      }

      /**
       *  Accept a Visitor and show him the inside.
       */
      void          Accept(PredicateVisitor& visitor)
      {
	dynamic_cast<DynamicPredicateVisitor<ObjectType>&>(visitor).Visit(
          this->getter_);
	return ;
      }
    };


    /**************************************************************************
    *                                                                         *
    *                                                                         *
    *                                DynamicInt                               *
    *                                                                         *
    *                                                                         *
    **************************************************************************/

    /**
     *  This class represents something like a terminal except that it has to
     *  evaluated at query execution and not before.
     */
    template        <typename ObjectType>
    class           DynamicInt : public DynamicPredicate<ObjectType>
    {
     private:
      const boost::function1<int, const ObjectType&> getter_;

      /**
       *  DynamicInt operator= overload.
       */
      DynamicInt &operator=(const DynamicInt& dd)
      {
	(void)dd;
	assert(false);
	return (*this);
      }

     public:
      /**
       *  DynamicInt default constructor.
       */
                    DynamicInt(const boost::function1<int,
                                                      const ObjectType&>&
                                    getter)
        : getter_(getter) {}

      /**
       *  DynamicInt copy constructor.
       */
                    DynamicInt(const DynamicInt& dd)
        : DynamicPredicate<ObjectType>(), getter_(dd.getter_)
      {
      }

      /**
       *  DynamicInt destructor.
       */
                    ~DynamicInt() {}

      /**
       *  Accept a Visitor and show him the inside.
       */
      void          Accept(PredicateVisitor& visitor)
      {
	dynamic_cast<DynamicPredicateVisitor<ObjectType>&>(visitor).Visit(
          this->getter_);
	return ;
      }
    };


    /**************************************************************************
    *                                                                         *
    *                                                                         *
    *                             DynamicString                               *
    *                                                                         *
    *                                                                         *
    **************************************************************************/

    /**
     *  This class represents something like a terminal except that it has to
     *  evaluated at query execution and not before.
     */
    template        <typename ObjectType>
    class           DynamicString : public DynamicPredicate<ObjectType>
    {
     private:
      const boost::function1<const std::string&, const ObjectType&> getter_;

      /**
       *  DynamicString operator= overload.
       */
      DynamicString &operator=(const DynamicString& dd)
      {
	(void)dd;
	assert(false);
	return (*this);
      }

     public:
      /**
       *  DynamicString default constructor.
       */
                    DynamicString(const boost::function1<const std::string&,
                                                         const ObjectType&>&
                                    getter)
        : getter_(getter) {}

      /**
       *  DynamicString copy constructor.
       */
                    DynamicString(const DynamicString& dd)
        : DynamicPredicate<ObjectType>(), getter_(dd.getter_)
      {
      }

      /**
       *  DynamicString destructor.
       */
                    ~DynamicString() {}

      /**
       *  Accept a Visitor and show him the inside.
       */
      void          Accept(PredicateVisitor& visitor)
      {
	dynamic_cast<DynamicPredicateVisitor<ObjectType>&>(visitor).Visit(
          this->getter_);
	return ;
      }
    };


    /**************************************************************************
    *                                                                         *
    *                                                                         *
    *                         DynamicPredicateVisitor                         *
    *                                                                         *
    *                                                                         *
    **************************************************************************/

    /**
     *  Objects that want to unroll dynamic predicates will have to subclass
     *  this class.
     */
    template                   <typename ObjectType>
    class                      DynamicPredicateVisitor
      : public PredicateVisitor
    {
    private:
      /**
       *  DynamicPredicateVisitor copy constructor.
       */
                               DynamicPredicateVisitor(
                                 const DynamicPredicateVisitor& dpv) throw ()
        : PredicateVisitor()
      {
	(void)dpv;
	assert(false);
      }

      /**
       *  DynamicPredicateVisitor operator= overload.
       */
      DynamicPredicateVisitor& operator=(const DynamicPredicateVisitor& dpv)
	throw ()
      {
	(void)dpv;
	assert(false);
	return (*this);
      }

     public:
      /**
       *  DynamicPredicateVisitor default constructor.
       */
                               DynamicPredicateVisitor() throw () {}


      /**
       *  DynamicPredicateVisitor destructor.
       */
      virtual                  ~DynamicPredicateVisitor()
      {
      }

      /**
       *  Overloaded function that'll have to be overriden by subclass.
       */
      virtual void             Visit(const boost::function1<double,
                                 const ObjectType&>& func) = 0;
      virtual void             Visit(const boost::function1<int,
                                 const ObjectType&>& func) = 0;
      virtual void             Visit(const boost::function1<short,
                                 const ObjectType&>& func) = 0;
      virtual void             Visit(const boost::function1<const std::string&,
                                 const ObjectType&>& func) = 0;
      virtual void             Visit(const boost::function1<time_t,
                                 const ObjectType&>& func) = 0;
    };
  }
}

#endif /* !DB_DYNAMIC_PREDICATE_H_ */
