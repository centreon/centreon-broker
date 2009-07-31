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

#ifndef NETWORK_INPUT_H_
# define NETWORK_INPUT_H_

# include <boost/thread.hpp>
# include <string>
# include "events/connection_status.h"
# include "io/io.h"

namespace                    CentreonBroker
{
  /**
   *  The NetworkInput class treats data coming from a client and parse it to
   *  generate appropriate Events.
   */
  class                      NetworkInput
  {
    friend class             ClientAcceptor;

   private:
    // Line processing parameters
    char                     buffer_[1024];
    unsigned long            bytes_processed_;
    size_t                   discard_;
    time_t                   last_checkin_time_;
    size_t                   length_;
    unsigned long            lines_processed_;
    // Other parameters
    Events::ConnectionStatus conn_status_;
    std::string              instance_;
    IO::Stream*              socket_;
    boost::thread*           thread_;
                             NetworkInput(IO::Stream* stream);
                             NetworkInput(const NetworkInput& ni);
    NetworkInput&            operator=(const NetworkInput& ni);
    void                     HandleAcknowledgement();
    void                     HandleComment();
    void                     HandleDowntime();
    void                     HandleHost();
    void                     HandleHostGroup();
    void                     HandleHostStatus();
    void                     HandleInitialization();
    void                     HandleLog();
    void                     HandleProgramStatus();
    void                     HandleService();
    void                     HandleServiceStatus();

   public:
                             ~NetworkInput() throw ();
    void                     operator()();
    // XXX : GetLine should be private
    char*                    GetLine();
  };
}

#endif /* !NETWORK_INPUT_H_ */
