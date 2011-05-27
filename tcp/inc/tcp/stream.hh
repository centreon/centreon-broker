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
# include "io/stream.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        tcp {
        /**
         *  @class stream stream.hh "tcp/stream.hh"
         *  @brief TCP stream.
         *
         *  TCP stream.
         */
        class          stream : public com::centreon::broker::io::stream {
         private:
          QSharedPointer<QTcpSocket>
                       _socket;

         public:
                       stream(QSharedPointer<QTcpSocket> sock);
                       stream(stream const& s);
                       ~stream();
          stream&      operator=(stream const& s);
          unsigned int read(void* data, unsigned int size);
          unsigned int write(void const* data, unsigned int size);
        };
      }
    }
  }
}

#endif /* !CCB_TCP_STREAM_HH_ */
