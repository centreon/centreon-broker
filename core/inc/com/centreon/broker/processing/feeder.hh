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

#ifndef CCB_PROCESSING_FEEDER_HH
#  define CCB_PROCESSING_FEEDER_HH

#  include <QThread>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         processing {
        /**
         *  @class feeder feeder.hh "com/centreon/broker/processing/feeder.hh"
         *  @brief Feed events from a source to a destination.
         *
         *  Take events from a source and send them to a destination.
         */
        class           feeder : public QThread {
          Q_OBJECT

         public:
                        feeder();
                        feeder(feeder const& other);
                        ~feeder();
          feeder&       operator=(feeder const& other);
          void          exit();
          void          prepare(
                          misc::shared_ptr<io::stream> in,
                          misc::shared_ptr<io::stream> out);
          void          run();

         private:
          misc::shared_ptr<io::stream>
                        _in;
          misc::shared_ptr<io::stream>
                        _out;
          volatile bool _should_exit;
        };
      }
    }
  }
}

#endif // !CCB_PROCESSING_FEEDER_HH
