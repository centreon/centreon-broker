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

#  include <ctime>
#  include <QList>
#  include <QMutex>
#  include <QThread>
#  include "com/centreon/broker/io/endpoint.hh"

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         bbdo {
        // Forward declaration.
        class           stream;

        /**
         *  @class acceptor acceptor.hh "com/centreon/broker/bbdo/acceptor.hh"
         *  @brief BBDO acceptor.
         *
         *  Accept incoming BBDO connections.
         */
        class           acceptor
          : public QObject, public io::endpoint {
          Q_OBJECT

        private:
          class         helper : public QThread {
          public:
                        helper(
                          acceptor* accptr,
                          misc::shared_ptr<io::stream> s);
            void        run();

          private:
            acceptor*   _acceptor;
            misc::shared_ptr<io::stream>
                        _stream;
          };
          friend class  helper;

        public:
                        acceptor(
                          QString const& name,
                          bool is_out,
                          bool negociate,
                          QString const& extensions,
                          time_t timeout,
                          bool one_peer_retention_mode = false,
                          bool coarse = false);
                        acceptor(acceptor const& right);
                        ~acceptor();
          acceptor&     operator=(acceptor const& right);
          io::endpoint* clone() const;
          void          close();
          misc::shared_ptr<io::stream>
                        open();
          misc::shared_ptr<io::stream>
                        open(QString const& id);
          void          stats(std::string& buffer);

        private:
          void          _negociate_features(
                           misc::shared_ptr<io::stream> stream,
                           misc::shared_ptr<bbdo::stream> my_bbdo);
          misc::shared_ptr<io::stream>
                        _open(misc::shared_ptr<io::stream> stream);

          QList<QThread*>
                        _clients;
          bool          _coarse;
          QString       _extensions;
          bool          _is_out;
          QString       _name;
          bool          _negociate;
          bool          _one_peer_retention_mode;
          QList<QThread*>
                        _threads;
          QMutex        _threadsm;
          time_t        _timeout;

        private slots:
          void          _on_thread_termination();
        };
      }
    }
  }
}

#endif // !CCB_BBDO_ACCEPTOR_HH
