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

#ifndef DB_ORACLE_HAVE_ARGS_H_
# define DB_ORACLE_HAVE_ARGS_H_

# include <ctime>            // for time_t
# include "db/have_args.h"
# include "db/oracle/query.h"

namespace                  CentreonBroker
{
  namespace                DB
  {
    /**
     *  \class OracleHaveArgs have_args.h "db/oracle/have_args.h"
     *  \brief Oracle queries that can have arguments.
     *
     *  OracleHaveArgs is subclassed by every query that can have arguments ;
     *  typically queries that have fields or that have predicate.
     *  OracleHaveArgs holds an array of arguments that can be sequentially set
     *  using the overloaded SetArg() method.
     */
    class                  OracleHaveArgs : virtual public HaveArgs,
                                            public OracleQuery
    {
     private:
      unsigned int         arg_;
      char**               args_;
      unsigned int         args_count_;

     protected:
                           OracleHaveArgs(OCI_Connection* oconn);
                           OracleHaveArgs(const OracleHaveArgs& oha);
      virtual              ~OracleHaveArgs();
      OracleHaveArgs&      operator=(const OracleHaveArgs& oha);
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

#endif /* !DB_ORACLE_HAVE_ARGS_H_ */
