/*
** Copyright 2013 Merethis
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

#ifndef CCB_BBDO_ACCEPTOR_HH
#  define CCB_BBDO_ACCEPTOR_HH

#  include <memory>
#  include <QList>
#  include <QThread>
#  include "com/centreon/broker/io/endpoint.hh"

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         bbdo {
        /**
         *  @class acceptor acceptor.hh "com/centreon/broker/bbdo/acceptor.hh"
         *  @brief BBDO acceptor.
         *
         *  Accept incoming BBDO connections.
         */
        class           acceptor
          : public QObject, public io::endpoint {
          Q_OBJECT

        public:
                        acceptor(
                          bool is_out,
                          bool negociate,
                          QString const& extensions,
                          io::endpoint const* temporary);
                        acceptor(acceptor const& right);
                        ~acceptor();
          acceptor&     operator=(acceptor const& right);
          io::endpoint* clone() const;
          void          close();
          misc::shared_ptr<io::stream>
                        open();

        private:
          QString       _extensions;
          bool          _is_out;
          bool          _negociate;
          std::auto_ptr<io::endpoint>
                        _temporary;
          QList<QThread*>
                        _threads;

        private slots:
          void          _on_thread_termination();
        };
      }
    }
  }
}

#endif // !CCB_BBDO_ACCEPTOR_HH
