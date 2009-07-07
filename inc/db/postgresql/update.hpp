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

#ifndef DB_POSTGRESQL_UPDATE_HPP_
# define DB_POSTGRESQL_UPDATE_HPP_

# include <cassert>
# include <cstdlib>
# include "db/postgresql/have_fields.h"
# include "db/postgresql/have_predicate.hpp"
# include "db/update.hpp"

namespace          CentreonBroker
{
  namespace        DB
  {
    template       <typename ObjectType>
    class          PgSQLUpdate : virtual public Update<ObjectType>,
                     public PgSQLHaveFields,
                     public PgSQLHavePredicate<ObjectType>
    {
     private:
      /**
       *  PgSQLUpdate copy constructor.
       */
                   PgSQLUpdate(const PgSQLUpdate& pgupdate)
	: Update<ObjectType>(),
          PgSQLHaveFields(),
          PgSQLHavePredicate<ObjectType>()
      {
	(void)pgupdate;
      }

      /**
       *  PgSQLUpdate operator= overload.
       */
      PgSQLUpdate& operator=(const PgSQLUpdate& pgupdate)
      {
	(void)pgupdate;
	return (*this);
      }

     public:
      /**
       *  PgSQLUpdate constructor.
       */
                   PgSQLUpdate(PGconn* pgconn,
                               const Mapping<ObjectType>& mapping)
        : Update<ObjectType>(mapping),
          PgSQLHaveFields(pgconn),
          PgSQLHavePredicate<ObjectType>()
      {
      }

      /**
       *  PgSQLUpdate destructor.
       */
                   ~PgSQLUpdate()
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
	this->PgSQLQuery::Execute();
	return ;
      }

      /**
       *  Get the number of elements updated bu the last query execution.
       */
      int          GetUpdateCount()
      {
	char*      count;

	count = PQcmdTuples(this->result_);
	if (!count)
	  throw (DBException(0,
			     DBException::QUERY_EXECUTION,
			     "Could not allocate memory."));
	return (atoi(count));
      }

      /**
       *  Prepare the query.
       */
      void         Prepare()
      {
	int arg;

	arg = 0;
	this->query_ = "UPDATE ";
	this->query_ += this->mapping_.table_;
	this->query_ += " SET ";
	for (decltype(this->mapping_.fields_.begin()) it =
               this->mapping_.fields_.begin();
             it != this->mapping_.fields_.end();
	     it++)
	  {
	    std::stringstream ss;

	    this->query_ += (*it).first;
	    this->query_ += "=$";
	    ss << ++arg;
	    this->query_ += ss.str();
	    this->query_ += ", ";
	  }
	this->query_.resize(this->query_.size() - 2);
	this->PgSQLHavePredicate<ObjectType>::BuildString(arg);
	this->query_ += this->predicate_str_;
	this->predicate_str_.resize(0);
	this->PgSQLQuery::Prepare();
	this->PgSQLHaveFields::Prepare();
	return ;
      }
    };
  }
}

#endif /* !DB_POSTGRESQL_UPDATE_HPP_ */
