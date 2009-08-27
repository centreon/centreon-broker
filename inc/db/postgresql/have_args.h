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

#ifndef DB_POSTGRESQL_HAVE_ARGS_H_
# define DB_POSTGRESQL_HAVE_ARGS_H_

# include <ctime>                 // for time_t
# include <libpq-fe.h>
# include "db/have_args.h"
# include "db/postgresql/query.h"

namespace                  CentreonBroker
{
  namespace                DB
  {
    /**
     *  \class PgSQLHaveArgs have_args.h "db/postgresql/have_args.h"
     *  \brief PostgreSQL queries that can have arguments.
     *
     *  PgSQLHaveArgs is subclassed by every query that can have arguments ;
     *  typically queries that have fields or that have predicate.
     *  PgSQLHaveArgs holds an array of arguments that can be sequentially set
     *  using the overloaded SetArg() method.
     */
    class                  PgSQLHaveArgs : virtual public HaveArgs,
                                           public PgSQLQuery
    {
     private:
      unsigned int         arg_;
      unsigned int         args_count_;
      int*                 args_format_;
      int*                 args_length_;
      char**               args_values_;
      void                 CleanArg(int idx);

     protected:
                           PgSQLHaveArgs(PGconn* pgconn);
                           PgSQLHaveArgs(const PgSQLHaveArgs& pha);
      virtual              ~PgSQLHaveArgs();
      PgSQLHaveArgs&       operator=(const PgSQLHaveArgs& pha);

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

#endif /* !DB_POSTGRESQL_HAVE_ARGS_H_ */
