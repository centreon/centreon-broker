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

#ifndef CCB_NDO_ACCEPTOR_HH
#  define CCB_NDO_ACCEPTOR_HH

#  include <QThread>
#  include <QVector>
#  include "com/centreon/broker/io/endpoint.hh"

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         ndo {
        /**
         *  @class acceptor acceptor.hh "com/centreon/broker/ndo/acceptor.hh"
         *  @brief NDO acceptor.
         *
         *  Accept incoming NDO connections.
         */
        class           acceptor
          : public QObject, public io::endpoint {
          Q_OBJECT

         public:
                        acceptor(
                          QString const& name,
                          bool is_out,
                          bool one_peer_retention_mode = false);
                        acceptor(acceptor const& a);
                        ~acceptor();
          acceptor&     operator=(acceptor const& a);
          io::endpoint* clone() const;
          void          close();
          misc::shared_ptr<io::stream>
                        open();
          misc::shared_ptr<io::stream>
                        open(QString const& id);
          void          stats(io::properties& tree);

         private:
          misc::shared_ptr<io::stream>
                        _open(misc::shared_ptr<io::stream> stream);

          bool          _is_out;
          QString       _name;
          bool          _one_peer_retention_mode;
          QVector<QThread*>
                        _threads;

         private slots:
          void          _on_thread_termination();
        };
      }
    }
  }
}

#endif // !CCB_NDO_ACCEPTOR_HH
