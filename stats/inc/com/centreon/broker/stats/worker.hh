/*
** Copyright 2012-2013,2017 Centreon
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

#ifndef CCB_STATS_WORKER_HH
#  define CCB_STATS_WORKER_HH

#  include <QThread>
#  include <string>
#  include "com/centreon/broker/stats/config.hh"

namespace               com {
  namespace             centreon {
    namespace           broker {
      // Forward declarations.
      namespace         io {
        class           properties;
      }

      namespace         stats {
        /**
         *  @class worker worker.hh "com/centreon/broker/stats/worker.hh"
         *  @brief Statistics worker thread.
         *
         *  The worker thread will wait for readability on a FIFO file
         *  and write to it statistics when available.
         */
        class           worker : public QThread {
          Q_OBJECT

         public:
                        worker();
                        ~worker() throw ();
          void          exit();
          void          run(QString const& fifo_file, config::fifo_type type);

         private:
                        worker(worker const& right);
          worker&       operator=(worker const& right);
          void          _close();
          bool          _open();
          void          run();

          std::string   _buffer;
          int           _fd;
          std::string   _fifo;
          config::fifo_type
                        _type;
          volatile bool _should_exit;
        };
      }
    }
  }
}

#endif // !CCB_STATS_WORKER_HH
