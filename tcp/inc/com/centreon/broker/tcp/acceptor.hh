/*
** Copyright 2011-2013,2015 Merethis
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

#ifndef CCB_TCP_ACCEPTOR_HH
#  define CCB_TCP_ACCEPTOR_HH

#  include <list>
#  include <memory>
#  include <QMutex>
#  include <QWaitCondition>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/tcp/server_socket.hh"
#  include "com/centreon/broker/tcp/socket_parent.hh"
#  include "com/centreon/broker/tcp/stream.hh"

CCB_BEGIN()

namespace          tcp {
  /**
   *  @class acceptor acceptor.hh "com/centreon/broker/tcp/acceptor.hh"
   *  @brief TCP acceptor.
   *
   *  Accept TCP connections.
   */
  class            acceptor : public io::endpoint, public socket_parent {
  public:
                   acceptor();
                   ~acceptor();

    endpoint*      clone() const;
    void           close();
    void           add_child(stream& child);
    void           listen_on(unsigned short port);
    misc::shared_ptr<io::stream>
                   open();
    misc::shared_ptr<io::stream>
                   open(QString const& id);
    void           remove_child(stream& child);
    void           set_read_timeout(int secs);
    void           set_write_timeout(int secs);
    void           stats(io::properties& tree);

  private:
                   acceptor(acceptor const& other);
    acceptor&      operator=(acceptor const& other);

    QMutex         _childrenm;
    std::list<stream*>
                   _children;
    QMutex         _mutex;
    unsigned short _port;
    int            _read_timeout;
    std::auto_ptr<server_socket>
                   _socket;
    int            _write_timeout;
    bool           _closed;
    QWaitCondition _children_closed_condvar;
  };
}

CCB_END()

#endif // !CCB_TCP_ACCEPTOR_HH
