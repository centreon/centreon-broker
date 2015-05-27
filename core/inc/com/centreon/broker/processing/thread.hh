/*
** Copyright 2015 Merethis
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

#ifndef CCB_PROCESSING_THREAD_HH
#  define CCB_PROCESSING_THREAD_HH

#  include <climits>
#  include <QMutex>
#  include <QThread>

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          processing {
        /**
         *  @class thread thread.hh "com/centreon/broker/processing/thread.hh"
         *  @brief Processing thread interface.
         *
         *  All processing threads respect this interface.
         *
         *  @see acceptor
         *  @see failover
         *  @see input
         */
        class            thread : protected QThread {
          Q_OBJECT

        public:
                         thread();
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
