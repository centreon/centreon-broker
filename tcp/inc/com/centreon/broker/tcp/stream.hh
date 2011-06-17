/*
** Copyright 2011 Merethis
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

#ifndef CCB_TCP_STREAM_HH_
# define CCB_TCP_STREAM_HH_

# include <QSharedPointer>
# include <QTcpSocket>
# include "com/centreon/broker/io/stream.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        tcp {
        /**
         *  @class stream stream.hh "com/centreon/broker/tcp/stream.hh"
         *  @brief TCP stream.
         *
         *  TCP stream.
         */
        class          stream : public io::stream {
         private:
          QSharedPointer<QTcpSocket>
                       _socket;

         public:
                       stream(QSharedPointer<QTcpSocket> sock);
                       stream(stream const& s);
                       ~stream();
          stream&      operator=(stream const& s);
          QSharedPointer<io::data>
                       read();
          void         write(QSharedPointer<io::data> d);
        };
      }
    }
  }
}

#endif /* !CCB_TCP_STREAM_HH_ */
