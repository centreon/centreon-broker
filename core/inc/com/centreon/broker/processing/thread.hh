/*
** Copyright 2015 Centreon
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

#ifndef CCB_PROCESSING_THREAD_HH
#  define CCB_PROCESSING_THREAD_HH

#  include <climits>
#  include <string>
#  include <QMutex>
#  include <QThread>
#  include "com/centreon/broker/processing/stat_visitable.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      // Forward declaration.
      namespace          io {
        class            properties;
      }

      namespace          processing {
        /**
         *  @class thread thread.hh "com/centreon/broker/processing/thread.hh"
         *  @brief Processing thread interface.
         *
         *  All processing threads respect this interface.
         *
         *  @see acceptor
         *  @see failover
         *  @see feeder
         */
        class            thread : protected QThread, public stat_visitable {
          Q_OBJECT

        public:
                         thread(std::string const& name = std::string());
                         ~thread();
          virtual void   exit();
          bool           should_exit() const;
          void           start();
          virtual void   update();
          virtual bool   wait(unsigned long timeout_ms = ULONG_MAX);

        protected:
          bool           _should_exit;
          mutable QMutex _should_exitm;

        private:
                         thread(thread const& other);
          thread&        operator=(thread const& other);
        };
      }
    }
  }
}

#endif // !CCB_PROCESSING_THREAD_HH
