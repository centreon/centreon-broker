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

#ifndef CONNECTION_H_
# define CONNECTION_H_

# include <ctime>
# include <string>
# include "events/connection_status.h"

namespace                CentreonBroker
{
  namespace              Events
  {
    class                Connection : public ConnectionStatus
    {
     private:
      void               InternalCopy(const Connection& c);
      void               ZeroInitialize();

     public:
      std::string        agent_name;
      std::string        agent_version;
      std::string        connect_source;
      time_t             connect_time;
      std::string        connect_type;
      time_t             data_start_time;
                         Connection();
                         Connection(const Connection& c);
		         Connection(const ConnectionStatus& cs);
                         ~Connection();
      Connection&        operator=(const Connection& c);
      int                GetType() const throw ();
    };
  }
}

#endif /* !CONNECTION_H_ */
