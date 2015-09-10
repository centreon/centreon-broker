/*
** Copyright 2011-2013 Centreon
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
