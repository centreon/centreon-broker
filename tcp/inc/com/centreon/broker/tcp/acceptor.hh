/*
** Copyright 2011-2013 Centreon
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
