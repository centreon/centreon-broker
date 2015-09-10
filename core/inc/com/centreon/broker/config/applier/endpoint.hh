/*
** Copyright 2011-2012 Centreon
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

                             ~endpoint();
            void             apply(
                               QList<config::endpoint> const& inputs,
                               QList<config::endpoint> const& outputs);
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
                               QList<config::endpoint>& l,
                               std::set<std::string> const& filters);
            void             _diff_endpoints(
                               QMap<config::endpoint,
                               processing::failover*>& current,
                               QList<config::endpoint> const& new_endpoints,
                               QList<config::endpoint>& to_create);

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
