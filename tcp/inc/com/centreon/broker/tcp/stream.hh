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

#ifndef CCB_TCP_STREAM_HH
#  define CCB_TCP_STREAM_HH

#  include <memory>
#  include <QTcpSocket>
#  include <string>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        tcp {
  // Forward declaration.
  class          acceptor;

  /**
   *  @class stream stream.hh "com/centreon/broker/tcp/stream.hh"
   *  @brief TCP stream.
   *
   *  TCP stream.
   */
  class          stream : public io::stream {
  public:
                 stream(QTcpSocket* sock, std::string const& name);
                 stream(int socket_descriptor);
                 ~stream();
    bool         read(
                   misc::shared_ptr<io::data>& d,
                   time_t deadline);
    void         set_parent(acceptor* parent);
    void         set_read_timeout(int secs);
    void         set_write_timeout(int secs);
    unsigned int write(misc::shared_ptr<io::data> const& d);

  private:
                 stream(stream const& other);
    stream&      operator=(stream const& other);
    void         _initialize_socket();
    void         _set_socket_options();

    std::string  _name;
    acceptor*    _parent;
    int          _read_timeout;
    std::auto_ptr<QTcpSocket>
                 _socket;
    int          _socket_descriptor;
    int          _write_timeout;
  };
}

CCB_END()

#endif // !CCB_TCP_STREAM_HH
