/*
** Copyright 2011-2013 Merethis
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

#  include <QTcpSocket>
#  include <QMutex>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        tcp {
  /**
   *  @class stream stream.hh "com/centreon/broker/tcp/stream.hh"
   *  @brief TCP stream.
   *
   *  TCP stream.
   */
  class          stream : public io::stream {
  public:
                 stream(misc::shared_ptr<QTcpSocket> sock);
                 stream(
                   misc::shared_ptr<QTcpSocket> sock,
                   misc::shared_ptr<QMutex> mutex);
                 ~stream();
    void         process(bool in = false, bool out = true);
    void         read(misc::shared_ptr<io::data>& d);
    void         set_timeout(int msecs);
    void         set_write_timeout(int msecs);
    unsigned int write(misc::shared_ptr<io::data> const& d);

  private:
                 stream(stream const& s);
    stream&      operator=(stream const& s);

    misc::shared_ptr<QMutex>
                 _mutex;
    bool         _process_in;
    bool         _process_out;
    misc::shared_ptr<QTcpSocket>
                 _socket;
    int          _timeout;
    int          _write_timeout;
  };
}

CCB_END()

#endif // !CCB_TCP_STREAM_HH
