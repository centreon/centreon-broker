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

#ifndef DB_MYSQL_DELETE_HPP_
# define DB_MYSQL_DELETE_HPP_

# include <cassert>
# include "db/delete.hpp"
# include "db/mysql/have_fields.h"
# include "db/mysql/query.h"

namespace          CentreonBroker
{
  namespace        DB
  {
    template       <typename ObjectType>
    class          MySQLDelete : public Delete<ObjectType>,
                                 public MySQLQuery,
                                 public MySQLHaveFields
    {
     private:
      /**
       *  MySQLDelete copy constructor.
       */
                   MySQLDelete(const MySQLDelete& mydelete)
	: Delete<ObjectType>(mydelete.mapping_),
	  MySQLQuery(),
	  MySQLHaveFields()
      {
	(void)mydelete;
	assert(false);
      }

      /**
       *  MySQLDelete operator= overload.
       */
      MySQLDelete& operator=(const MySQLDelete& mydelete)
      {
	(void)mydelete;
	assert(false);
	return (*this);
      }

     public:
      /**
       *  MySQLDelete constructor.
       */
                   MySQLDelete(MYSQL* myconn,
                               const Mapping<ObjectType>& mapping)
	: HaveFields(),
	  Delete<ObjectType>(mapping),
	  MySQLQuery(myconn),
	  MySQLHaveFields()
      {
      }

      /**
       *  MySQLDelete destructor.
       */
                   ~MySQLDelete()
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
	if (mysql_stmt_bind_param(this->mystmt_, this->myargs_))
	  throw (DBException(mysql_stmt_errno(this->mystmt_),
			     DBException::QUERY_EXECUTION,
			     mysql_stmt_error(this->mystmt_)));
	this->MySQLQuery::Execute();
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
	    this->MySQLQuery::Prepare();
	    this->MySQLHaveFields::Prepare(this->mystmt_);
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

#endif /* !DB_MYSQL_DELETE_HPP_ */
