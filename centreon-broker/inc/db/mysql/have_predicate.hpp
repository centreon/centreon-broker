/*
** have_predicate.hpp for CentreonBroker in ./inc/db/mysql
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/10/09 Matthieu Kermagoret
** Last update 06/10/09 Matthieu Kermagoret
*/

#ifndef DB_MYSQL_HAVE_PREDICATE_HPP_
# define DB_MYSQL_HAVE_PREDICATE_HPP_

# include <boost/bind.hpp>
# include <boost/function.hpp>
# include <sstream>
# include <vector>
# include "db/have_fields.h"
# include "db/have_predicate.hpp"

namespace                 CentreonBroker
{
  namespace               DB
  {
    template              <typename ObjectType>
    class                 MySQLHavePredicate
      : virtual public HavePredicate<ObjectType>
    {
     private:
      /**
       *  MySQLHavePredicate copy constructor.
       */
                          MySQLHavePredicate(const MySQLHavePredicate& myhp)
        : HavePredicate<ObjectType>()
      {
	(void)myhp;
      }

      /**
       *  MySQLHavePredicate operator= overload.
       */
      MySQLHavePredicate& operator=(const MySQLHavePredicate& myhp)
      {
	(void)myhp;
	return (*this);
      }

     protected:
      std::string         predicate_str_;
      std::vector<boost::function2<void, HaveFields*, const ObjectType&> >
                          dynamic_getters_;

     public:
      /**
       *  MySQLHavePredicate default constructor.
       */
                          MySQLHavePredicate() {}

      /**
       *  MySQLHavePredicate destructor.
       */
      virtual             ~MySQLHavePredicate() {}

      /**
       *  Build the predicate string.
       */
      void                BuildString()
      {
	this->predicate_str_ = " WHERE ";
	this->predicate_->Accept(*this);
	return ;
      }

      /**
       *  And predicate.
       */
      void                Visit(And& a_n_d)
      {
	a_n_d.Left().Accept(*this);
	this->predicate_str_ += " AND ";
	a_n_d.Right().Accept(*this);
	return ;
      }

      /**
       *  Equal predicate.
       */
      void                Visit(Equal &equal)
      {
	equal.Left().Accept(*this);
	this->predicate_str_ += '=';
	equal.Right().Accept(*this);
	return ;
      }

      /**
       *  Field.
       */
      void                Visit(Field& field)
      {
	this->predicate_str_ += field.GetName();
	return ;
      }

      /**
       *  Terminal.
       */
      void                Visit(Terminal& terminal)
      {
	std::stringstream ss;

	switch (terminal.GetType())
	  {
           case 'd':
	    ss << terminal.GetDouble();
	    break ;
           case 'i':
	    ss << terminal.GetInt();
	    break ;
           case 's':
	    ss << terminal.GetShort();
	    break ;
           case 'S':
	    ss << terminal.GetString();
	    break ;
           case 't':
	    ss << terminal.GetTime();
	    break ;
           default:
	    assert(false);
	  }
	this->predicate_str_ += ss.str();
	return ;
      }

      /**
       *  Dynamic double.
       */
      void                Visit(const boost::function1<double,
                                                       const ObjectType&>&
                                  getter)
      {
	this->predicate_str_ += '?';
	this->dynamic_getters_.push_back(boost::bind(&HaveFields::SetDouble,
                                                     _1,
                                                     boost::bind(getter, _2)));
	return ;
      }

      /**
       *  Dynamic int.
       */
      void                Visit(const boost::function1<int,
                                                       const ObjectType&>&
                                  getter)
      {
	this->predicate_str_ += '?';
	this->dynamic_getters_.push_back(boost::bind(&HaveFields::SetInt,
                                                     _1,
                                                     boost::bind(getter, _2)));
	return ;
      }

      /**
       *  Dynamic short.
       */
      void                Visit(const boost::function1<short,
                                                       const ObjectType&>&
                                  getter)
      {
	this->predicate_str_ += '?';
	this->dynamic_getters_.push_back(boost::bind(&HaveFields::SetShort,
                                                     _1,
                                                     boost::bind(getter, _2)));
	return ;
      }

      /**
       *  Dynamic string.
       */
      void                Visit(const boost::function1<const std::string&,
                                                       const ObjectType&>&
                                  getter)
      {
	this->predicate_str_ += '?';
	this->dynamic_getters_.push_back(boost::bind(&HaveFields::SetString,
                                                     _1,
                                                     boost::bind(getter, _2)));
	return ;
      }

      /**
       *  Dynamic time.
       */
      void                Visit(const boost::function1<time_t,
                                                       const ObjectType&>&
                                  getter)
      {
	this->predicate_str_ += '?';
	this->dynamic_getters_.push_back(boost::bind(&HaveFields::SetTime,
                                                     _1,
                                                     boost::bind(getter, _2)));
	return ;
      }
    };
  }
}

#endif /* !DB_MYSQL_HAVE_PREDICATE_HPP_ */
