/*
** Copyright 2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_BBDO_ACCEPTOR_HH
#  define CCB_BBDO_ACCEPTOR_HH

#  include <ctime>
#  include <QList>
#  include <QMutex>
#  include <QThread>
#  include <memory>
#  include "com/centreon/broker/processing/feeder.hh"
#  include "com/centreon/broker/io/endpoint.hh"

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         bbdo {
        // Forward declaration.
        class           stream;
        class           acceptor;

        /**
         *  @class helper helper.hh "com/centreon/broker/bbdo/acceptor.hh"
         *  @brief help class
         */
        class         helper : public QThread {
          Q_OBJECT

        public:
                      helper(
                        acceptor* accptr,
                        misc::shared_ptr<io::stream> s);
          void        run();
          void        set_feeder(processing::feeder& feeder);
          void        exit();

        private:
          acceptor*   _acceptor;
          misc::shared_ptr<io::stream>
                      _stream;
          std::auto_ptr<processing::feeder>
                      _feeder;
        };

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
          void          stats(io::properties& tree);

        private:
          void          _negociate_features(
                           misc::shared_ptr<io::stream> stream,
                           misc::shared_ptr<bbdo::stream> my_bbdo);
          misc::shared_ptr<io::stream>
                        _open(misc::shared_ptr<io::stream> stream,
                              helper& thread);

          QMutex        _clientsm;
          QList<QThread*>
                        _clients;
          bool          _coarse;
          QString       _extensions;
          bool          _is_out;
          QString       _name;
          bool          _negociate;
          bool          _one_peer_retention_mode;
          time_t        _timeout;

        private slots:
          void          _on_thread_termination();
        };
      }
    }
  }
}

#endif // !CCB_BBDO_ACCEPTOR_HH
