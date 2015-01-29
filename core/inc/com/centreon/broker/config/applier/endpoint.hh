/*
** Copyright 2011-2012,2015 Merethis
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

#ifndef CCB_CONFIG_APPLIER_ENDPOINT_HH
#  define CCB_CONFIG_APPLIER_ENDPOINT_HH

#  include <QList>
#  include <QMap>
#  include <QMutex>
#  include <QObject>
#  include "com/centreon/broker/config/endpoint.hh"
#  include "com/centreon/broker/processing/failover.hh"

namespace                    com {
  namespace                  centreon {
    namespace                broker {
      namespace              config {
        namespace            applier {
          /**
           *  @class endpoint endpoint.hh "com/centreon/broker/config/applier/endpoint.hh"
           *  @brief Apply the configuration of input and output endpoints.
           *
           *  Apply the configuration of the input and output endpoints.
           */
          class              endpoint : public QObject {
            Q_OBJECT

           public:
            typedef          QMap<config::endpoint, processing::failover*>::iterator
                             iterator;
            typedef          QMap<config::endpoint, processing::failover*>::const_iterator
                             const_iterator;

                             ~endpoint();
            void             apply(
                               QList<config::endpoint> const& inputs,
                               QList<config::endpoint> const& outputs,
                               QString const& cache_directory);
            void             discard();
            iterator         input_begin();
            iterator         input_end();
            QMutex&          input_mutex();
            static endpoint& instance();
            static void      load();
            iterator         output_begin();
            iterator         output_end();
            QMutex&          output_mutex();
            static void      unload();

           public slots:
            void             terminated_input();
            void             terminated_output();

           private:
                             endpoint();
                             endpoint(endpoint const& e);
            endpoint&        operator=(endpoint const& e);
            processing::failover*
                             _create_endpoint(
                               config::endpoint& cfg,
                               bool is_input,
                               bool is_output,
                               QList<config::endpoint>& l);
            void             _diff_endpoints(QMap<config::endpoint, processing::failover*>& current,
                               QList<config::endpoint> const& new_endpoints,
                               QList<config::endpoint>& to_create);

            QString          _cache_directory;
            QMap<config::endpoint, processing::failover*>
                             _inputs;
            QMutex           _inputsm;
            QMap<config::endpoint, processing::failover*>
                             _outputs;
            QMutex           _outputsm;
          };
        }
      }
    }
  }
}

#endif // !CCB_CONFIG_APPLIER_ENDPOINT_HH
