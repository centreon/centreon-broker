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

#ifndef DB_HAVE_ARGS_H_
# define DB_HAVE_ARGS_H_

# include <ctime>  // for time_t
# include <string>

namespace          CentreonBroker
{
  namespace        DB
  {
    /**
     *  \class HaveArgs have_args.h "db/have_args.h"
     *  \brief Query that can be parameterized.
     *
     *  This class provides an interface to set arguments of a query.
     */
    class          HaveArgs
    {
     protected:
                   HaveArgs() throw ();
                   HaveArgs(const HaveArgs& ha) throw ();
      virtual      ~HaveArgs();
      HaveArgs&    operator=(const HaveArgs& ha) throw ();

     public:
      virtual void SetArg(bool arg) = 0;
      virtual void SetArg(double arg) = 0;
      virtual void SetArg(int arg) = 0;
      virtual void SetArg(short arg) = 0;
      virtual void SetArg(const std::string& arg) = 0;
      virtual void SetArg(time_t arg) = 0;
    };
  }
}

#endif /* !DB_HAVE_ARGS_H_ */
