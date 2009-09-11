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

#ifndef DB_MYSQL_HAVE_ARGS_H_
# define DB_MYSQL_HAVE_ARGS_H_

# include <ctime>            // for time_t
# include <mysql.h>          // for MYSQL_BIND
# include "db/have_args.h"
# include "db/mysql/query.h"

namespace                  CentreonBroker
{
  namespace                DB
  {
    /**
     *  \class MySQLHaveArgs have_args.h "db/mysql/have_args.h"
     *  \brief MySQL queries that can have arguments.
     *
     *  MySQLHaveArgs is subclassed by every query that can have arguments ;
     *  typically queries that have fields or that have predicate.
     *  MySQLHaveArgs holds an array of arguments that can be sequentially set
     *  using the overloaded SetArg() method.
     */
    class                  MySQLHaveArgs : virtual public HaveArgs,
                                           public MySQLQuery
    {
     private:
      unsigned int         arg_;
      MYSQL_BIND*          args_;
      unsigned int         args_count_;
      void                 Clean();
      void                 CleanArg(MYSQL_BIND* bind);
      void                 InternalCopy(const MySQLHaveArgs& mha);

     protected:
                           MySQLHaveArgs(MYSQL* mysql);
                           MySQLHaveArgs(const MySQLHaveArgs& mha);
      virtual              ~MySQLHaveArgs();
      MySQLHaveArgs&       operator=(const MySQLHaveArgs& mha);
      virtual unsigned int GetArgCount() throw () = 0;

     public:
      virtual void         Execute();
      virtual void         Prepare();
      virtual void         SetArg(bool arg);
      virtual void         SetArg(double arg);
      virtual void         SetArg(int arg);
      virtual void         SetArg(short arg);
      virtual void         SetArg(const std::string& arg);
      virtual void         SetArg(time_t arg);
    };
  }
}

#endif /* !DB_MYSQL_HAVE_ARGS_H_ */
