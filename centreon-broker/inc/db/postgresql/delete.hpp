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

#ifndef DB_POSTGRESQL_DELETE_HPP_
# define DB_POSTGRESQL_DELETE_HPP_

# include <cassert>
# include "db/delete.hpp"
# include "db/postgresql/have_fields.h"
# include "db/postgresql/query.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    template       <typename ObjectType>
    class          PgSQLDelete : public Delete<ObjectType>,
                                 public PgSQLHaveFields
    {
     private:
      /**
       *  PgSQLDelete copy constructor.
       */
                   PgSQLDelete(const PgSQLDelete& pgdelete)
	: Delete<ObjectType>(pgdelete.mapping_),
	  PgSQLHaveFields(NULL)
      {
	(void)pgdelete;
	assert(false);
      }

      /**
       *  PgSQLDelete operator= overload.
       */
      PgSQLDelete& operator=(const PgSQLDelete& pgdelete)
      {
	(void)pgdelete;
	assert(false);
	return (*this);
      }

     public:
      /**
       *  PgSQLDelete constructor.
       */
                   PgSQLDelete(PGconn* pgconn,
                               const Mapping<ObjectType>& mapping)
	: HaveFields(),
	  Delete<ObjectType>(mapping),
	  PgSQLHaveFields(pgconn)
      {
      }

      /**
       *  PgSQLDelete destructor.
       */
                   ~PgSQLDelete()
      {
      }

      /**
       *  Delete the specified object.
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
	    this->query_ = "DELETE FROM ";
	    this->query_ += this->mapping_.table_;
	    this->query_ += " WHERE ";
	    for (decltype(this->mapping_.fields_.begin()) it =
		   this->mapping_.fields_.begin();
		 it != this->mapping_.fields_.end();
		 it++)
	      {
		this->query_ += (*it).first;
		this->query_ += "=? AND ";
	      }
	    this->query_.resize(this->query_.size() - 5);
	    this->PgSQLQuery::Prepare();
	    this->PgSQLHaveFields::Prepare(this->mystmt_);
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

#endif /* !DB_POSTGRESQL_DELETE_HPP_ */
