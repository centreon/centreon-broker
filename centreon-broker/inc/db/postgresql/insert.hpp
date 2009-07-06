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

#ifndef DB_POSTGRESQL_INSERT_HPP_
# define DB_POSTGRESQL_INSERT_HPP_

# include <boost/bind.hpp>
# include <cassert>
# include <mysql.h>
# include <sstream>
# include "db/insert.hpp"
# include "db/postgresql/have_fields.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    template       <typename ObjectType>
    class          PgSQLInsert : public Insert<ObjectType>,
                                 public PgSQLHaveFields
    {
     private:
      /**
       *  PgSQLInsert copy constructor.
       */
                   PgSQLInsert(const PgSQLInsert& pginsert)
        : Insert<ObjectType>(pginsert.mapping_),
          PgSQLHaveFields(NULL)
      {
	(void)pginsert;
	assert(false);
      }

      /**
       *  PgSQLInsert operator= overload.
       */
      PgSQLInsert& operator=(const PgSQLInsert& pginsert)
      {
	(void)pginsert;
	assert(false);
	return (*this);
      }

     public:
      /**
       *  PgSQLInsert constructor.
       */
                   PgSQLInsert(PGconn* pgconn,
                               const Mapping<ObjectType>& mapping)
        : HaveFields(),
          Insert<ObjectType>(mapping),
          PgSQLHaveFields(pgconn)
      {
      }

      /**
       *  PgSQLInsert destructor.
       */
                   ~PgSQLInsert()
      {
      }

      /**
       *  Insert the specified object.
       */
      void         Execute(const ObjectType& object) throw (DBException)
      {
	// Reset argument count.
	this->Reset();
	// Browse all args.
	for (decltype(this->mapping_.fields_.begin()) it =
               this->mapping_.fields_.begin();
             it != this->mapping_.fields_.end();
	     it++)
	  (*it).second(this, object);
	this->PgSQLQuery::Execute();
	return ;
      }

      /**
       *  Prepare the query for execution.
       */
      void         Prepare() throw (DBException)
      {
	try
	  {
	    int arg;
	    std::string parameters;

	    arg = 0;
	    this->query_ = "INSERT INTO ";
	    this->query_ += this->mapping_.table_;
	    this->query_ += "(";
	    parameters = "VALUES(";
	    for (decltype(this->mapping_.fields_.begin()) it =
		   this->mapping_.fields_.begin();
		 it != this->mapping_.fields_.end();
		 it++)
	      {
		std::stringstream ss;

		this->query_ += '"';
		this->query_ += (*it).first;
		this->query_ += "\", ";

		parameters += '$';
		ss << ++arg;
		parameters += ss.str();
		parameters += ", ";
	      }
	    this->query_.resize(this->query_.size() - 2);
	    this->query_ += ") ";
	    parameters.resize(parameters.size() - 2);
	    parameters += ')';
	    this->query_ += parameters;
	    this->PgSQLQuery::Prepare();
	    this->PgSQLHaveFields::Prepare();
	  }
	catch (DBException& e)
	  {
	    throw ;
	  }
	catch (std::exception& e)
	  {
	    throw (DBException(0,
                               DBException::QUERY_PREPARATION,
                               e.what()));
	  }
	return ;
      }
    };
  }
}

#endif /* !DB_POSTGRESQL_INSERT_HPP_ */
