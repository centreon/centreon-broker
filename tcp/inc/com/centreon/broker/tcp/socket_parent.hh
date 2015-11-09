/*
** Copyright 2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_TCP_SOCKET_PARENT_HH
#  define CCB_TCP_SOCKET_PARENT_HH

#  include <QTcpServer>
#  include <queue>
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          tcp {
  // Forward declaration.
  class          stream;
  /**
   *  @class socket_parent socket_parent.hh "com/centreon/broker/tcp/socket_parent.hh"
   *  @brief Interface for parent of a socket stream.
   *
   *  Connectors and acceptors need to be notified
   *  of the liveness of their child stream. This interface represent
   *  a class that need to be notified.
   */
  class            socket_parent {
  public:
    virtual        ~socket_parent() {}

    virtual void   add_child(tcp::stream& child) = 0;
    virtual void   remove_child(tcp::stream& child) = 0;
  };
}

CCB_END()

#endif // !CCB_TCP_SOCKET_PARENT_HH
