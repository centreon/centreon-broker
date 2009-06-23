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

# include <string>
# include <sys/types.h>
# include "connection_status.h"

namespace              CentreonBroker
{
  class                Connection : public ConnectionStatus
  {
   private:
    enum               String
    {
      AGENT_NAME = 0,
      AGENT_VERSION,
      CONNECT_SOURCE,
      CONNECT_TYPE,
      STRING_NB
    };
    enum               TimeT
    {
      CONNECT_TIME = 0,
      DATA_START_TIME,
      TIMET_NB
    };
    std::string        strings_[STRING_NB];
    time_t             timets_[TIMET_NB];
    void               InternalCopy(const Connection& c);

   public:
                       Connection() throw ();
                       Connection(const Connection& c);
		       Connection(const ConnectionStatus& cs);
                       ~Connection() throw ();
    Connection&        operator=(const Connection& c);
    // Getters
    const std::string& GetAgentName() const throw ();
    const std::string& GetAgentVersion() const throw ();
    const std::string& GetConnectSource() const throw ();
    time_t             GetConnectTime() const throw ();
    const std::string& GetConnectType() const throw ();
    time_t             GetDataStartTime() const throw ();
    int                GetType() const throw ();
    // Setters
    void               SetAgentName(const std::string& an);
    void               SetAgentVersion(const std::string& av);
    void               SetConnectSource(const std::string& cs);
    void               SetConnectTime(time_t ct) throw ();
    void               SetConnectType(const std::string& ct);
    void               SetDataStartTime(time_t dst) throw ();
  };
}

#endif /* !CONNECTION_H_ */
