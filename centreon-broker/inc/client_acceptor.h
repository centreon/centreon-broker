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

#ifndef CLIENT_ACCEPTOR_H_
# define CLIENT_ACCEPTOR_H_

# include <boost/thread.hpp>
# include "io/io.h"

namespace           CentreonBroker
{
  class             ClientAcceptor
  {
   private:
    IO::Acceptor*   acceptor_;
    boost::thread*  thread_;
                    ClientAcceptor(const ClientAcceptor& ca);
    ClientAcceptor& operator=(const ClientAcceptor& ca);

   public:
                    ClientAcceptor();
                    ~ClientAcceptor();
    void            operator()();
    void            Run(IO::Acceptor* acceptor);
  };
}

#endif /* !CLIENT_ACCEPTOR_H_ */
