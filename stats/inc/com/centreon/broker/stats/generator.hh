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

#ifndef CCB_STATS_GENERATOR_HH
#  define CCB_STATS_GENERATOR_HH

#  include <QThread>
#  include <map>
#  include <utility>

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         stats {
        // Forward declaration.
        class           config;

        /**
         *  @class generator generator.hh "com/centreon/broker/stats/generator.hh"
         *  @brief Generate statistics about Broker.
         *
         *  Standalone thread that generates statistics about Broker at
         *  regular interval.
         */
        class           generator : public QThread {
          Q_OBJECT

        public:
                        generator();
                        ~generator() throw ();
          void          exit();
          void          run(
                          config const& cfg,
                          unsigned int instance_id);

        private:
                        generator(generator const& right);
          generator&    operator=(generator const& right);
          void          run();

          unsigned int  _instance_id;
          volatile bool _should_exit;
        };
      }
    }
  }
}

#endif // !CCB_STATS_GENERATOR_HH
