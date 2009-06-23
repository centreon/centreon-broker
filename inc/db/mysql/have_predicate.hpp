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
