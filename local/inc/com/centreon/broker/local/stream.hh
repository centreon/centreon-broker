/*
** Copyright 2011 Merethis
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

#ifndef CCB_LOCAL_STREAM_HH_
# define CCB_LOCAL_STREAM_HH_

# include <QLocalSocket>
# include <QMutex>
# include <QSharedPointer>
# include "com/centreon/broker/io/stream.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace   local {
  /**
   *  @class stream stream.hh "com/centreon/broker/local/stream.hh"
   *  @brief Local socket stream.
   *
   *  Local socket stream.
   */
  class     stream : public io::stream {
   private:
    QSharedPointer<QMutex>
            _mutex;
    bool    _process_in;
    bool    _process_out;
    QSharedPointer<QLocalSocket>
            _socket;
    int     _timeout;
            stream(stream const& s);
    stream& operator=(stream const& s);

   public:
            stream(QSharedPointer<QLocalSocket> sock);
            stream(
              QSharedPointer<QLocalSocket> sock,
              QSharedPointer<QMutex> mutex);
            ~stream();
    void    process(bool in = false, bool out = true);
    QSharedPointer<io::data>
            read();
    void    set_timeout(int msecs);
    void    write(QSharedPointer<io::data> d);
  };
}

CCB_END()

#endif /* !CCB_LOCAL_STREAM_HH_ */
