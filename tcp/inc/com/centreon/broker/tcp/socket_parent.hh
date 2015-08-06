/*
** Copyright 2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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

    virtual void   add_child(stream& child) = 0;
    virtual void   remove_child(stream& child) = 0;
  };
}

CCB_END()

#endif // !CCB_TCP_SOCKET_PARENT_HH
