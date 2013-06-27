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

#ifndef CCB_LOCAL_STREAM_HH
#  define CCB_LOCAL_STREAM_HH

#  include <QLocalSocket>
#  include <QMutex>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        local {
  /**
   *  @class stream stream.hh "com/centreon/broker/local/stream.hh"
   *  @brief Local socket stream.
   *
   *  Local socket stream.
   */
  class          stream : public io::stream {
  public:
                 stream(misc::shared_ptr<QLocalSocket> sock);
                 stream(
                   misc::shared_ptr<QLocalSocket> sock,
                   misc::shared_ptr<QMutex> mutex);
                 ~stream();
    void         process(bool in = false, bool out = true);
    void         read(misc::shared_ptr<io::data>& d);
    void         set_timeout(int msecs);
    unsigned int write(misc::shared_ptr<io::data> const& d);

   private:
                 stream(stream const& s);
    stream&      operator=(stream const& s);

    misc::shared_ptr<QMutex>
                 _mutex;
    bool         _process_in;
    bool         _process_out;
    misc::shared_ptr<QLocalSocket>
                 _socket;
    int          _timeout;
  };
}

CCB_END()

#endif // !CCB_LOCAL_STREAM_HH
