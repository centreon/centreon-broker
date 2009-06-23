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

#ifndef DB_MYSQL_UPDATE_HPP_
# define DB_MYSQL_UPDATE_HPP_

# include <cassert>
# include "db/mysql/have_fields.h"
# include "db/mysql/have_predicate.hpp"
# include "db/mysql/query.h"
# include "db/update.hpp"

namespace          CentreonBroker
{
  namespace        DB
  {
    template       <typename ObjectType>
    class          MySQLUpdate : virtual public Update<ObjectType>,
                     public MySQLQuery,
                     public MySQLHaveFields,
                     public MySQLHavePredicate<ObjectType>
    {
     private:
      /**
       *  MySQLUpdate copy constructor.
       */
                   MySQLUpdate(const MySQLUpdate& myupdate)
	: Update<ObjectType>(),
	  MySQLQuery(),
	  MySQLHaveFields(),
	  MySQLHavePredicate<ObjectType>()
      {
	(void)myupdate;
      }

      /**
       *  MySQLUpdate operator= overload.
       */
      MySQLUpdate& operator=(const MySQLUpdate& myupdate)
      {
	(void)myupdate;
	return (*this);
      }

     public:
      /**
       *  MySQLUpdate constructor.
       */
                   MySQLUpdate(MYSQL* myconn,
                               const Mapping<ObjectType>& mapping)
        : Update<ObjectType>(mapping),
          MySQLQuery(myconn),
          MySQLHaveFields(),
          MySQLHavePredicate<ObjectType>()
      {
      }

      /**
       *  MySQLUpdate destructor.
       */
                   ~MySQLUpdate()
      {
      }

      /**
       *  Update an object.
       */
      void         Execute(const ObjectType& object)
      {
	// Reset argument count.
	this->Reset();
	// Browse all args.
	for (decltype(this->mapping_.fields_.begin()) it =
               this->mapping_.fields_.begin();
             it != this->mapping_.fields_.end();
             it++)
	  (*it).second(this, object);
	// Browse predicate arguments.
	for (decltype(this->dynamic_getters_.begin()) it =
               this->dynamic_getters_.begin();
             it != this->dynamic_getters_.end();
             it++)
          (*it)(this, object);
	if (mysql_stmt_bind_param(this->mystmt_, this->myargs_))
	  throw (DBException(mysql_stmt_errno(this->mystmt_),
                             DBException::QUERY_EXECUTION,
                             mysql_stmt_error(this->mystmt_)));
	this->MySQLQuery::Execute();
	return ;
      }

      /**
       *  Get the number of elements updated bu the last query execution.
       */
      int          GetUpdateCount()
      {
	// XXX : generate an exception in case of error
	return (mysql_stmt_affected_rows(this->mystmt_));
      }

      /**
       *  Prepare the query.
       */
      void         Prepare()
      {
	this->query_ = "UPDATE ";
	this->query_ += this->mapping_.table_;
	this->query_ += " SET ";
	for (decltype(this->mapping_.fields_.begin()) it =
               this->mapping_.fields_.begin();
             it != this->mapping_.fields_.end();
	     it++)
	  {
	    this->query_ += (*it).first;
	    this->query_ += "=?, ";
	  }
	this->query_.resize(this->query_.size() - 2);
	this->MySQLHavePredicate<ObjectType>::BuildString();
	this->query_ += this->predicate_str_;
	this->predicate_str_.resize(0);
	this->MySQLQuery::Prepare();
	this->MySQLHaveFields::Prepare(this->mystmt_);
	return ;
      }
    };
  }
}

#endif /* !DB_MYSQL_UPDATE_HPP_ */
