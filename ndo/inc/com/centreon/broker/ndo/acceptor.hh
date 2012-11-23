/*
** Copyright 2011-2012 Merethis
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

#  include <memory>
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
                          bool is_out,
                          io::endpoint const* temporary);
                        acceptor(acceptor const& a);
                        ~acceptor();
          acceptor&     operator=(acceptor const& a);
          io::endpoint* clone() const;
          void          close();
          misc::shared_ptr<io::stream>
                        open();

         private:
          bool          _is_out;
          std::auto_ptr<io::endpoint>
                        _temporary;
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
