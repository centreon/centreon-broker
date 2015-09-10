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

#ifndef CCB_PROCESSING_FAILOVER_HH
#  define CCB_PROCESSING_FAILOVER_HH

#  include <climits>
#  include <ctime>
#  include <memory>
#  include <QMutex>
#  include <QReadWriteLock>
#  include <QString>
#  include <QThread>
#  include <set>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/processing/feeder.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      // Forward declaration.
      namespace          stats {
        class            builder;
      }

      namespace          processing {
        /**
         *  @class failover failover.hh "com/centreon/broker/processing/failover.hh"
         *  @brief Failover thread.
         *
         *  Thread that provide failover on inputs or outputs.
         */
        class            failover : public QThread, public io::stream {
          Q_OBJECT
          friend class   stats::builder;

         public:
                         failover(
                           misc::shared_ptr<io::endpoint> endp,
                           bool is_out,
                           QString const& name = "(unknown)",
                           std::set<unsigned int> const& filters = std::set<unsigned int>());
                         failover(failover const& f);
                         ~failover();
          failover&      operator=(failover const& f);
          time_t         get_buffering_timeout() const throw ();
          double         get_event_processing_speed() const throw ();
          time_t         get_last_event() const throw ();
          time_t         get_read_timeout() const throw ();
          time_t         get_retry_interval() const throw ();
          void           process(bool in = false, bool out = false);
          void           read(misc::shared_ptr<io::data>& d);
          void           read(
                           misc::shared_ptr<io::data>& d,
                           time_t timeout,
                           bool* timed_out = NULL);
          void           run();
          void           set_buffering_timeout(time_t secs);
          void           set_failover(
                           misc::shared_ptr<processing::failover> fo);
          void           set_read_timeout(time_t read_timeout);
          void           set_retry_interval(time_t retry_interval);
          void           statistics(io::properties& tree) const;
          void           update();
          bool           wait(unsigned long time = ULONG_MAX);
          unsigned int   write(misc::shared_ptr<io::data> const& d);

          static time_t const
                         event_window_length = 30;

         signals:
          void           exception_caught();
          void           initial_lock();

         private:
          void           _update_status(std::string const& status);

          // Data that doesn't require locking.
          volatile time_t _buffering_timeout;
          misc::shared_ptr<io::endpoint>
                         _endpoint;
          unsigned int   _events[event_window_length];
          misc::shared_ptr<failover>
                         _failover;
          std::set<unsigned int>
                         _filters;
          bool           _initial;
          bool           _is_out;
          time_t         _last_connect_attempt;
          time_t         _last_connect_success;
          QString        _last_error;
          time_t         _last_event;
          QString        _name;
          time_t         _next_timeout;
          std::list<misc::shared_ptr<io::data> >
                         _processed;
          time_t         _read_timeout;
          volatile time_t _retry_interval;
          std::list<misc::shared_ptr<io::data> >
                         _unprocessed;
          volatile bool  _update;

          // Retained data.
          misc::shared_ptr<io::data>
                         _data;
          mutable QMutex _datam;

          // Exit flag.
          volatile bool  _immediate;
          volatile bool  _should_exit;
          mutable QMutex _should_exitm;

          // Status.
          std::string    _status;
          mutable QMutex _statusm;

          // Stream locking.
          mutable QReadWriteLock _fromm;
          mutable QReadWriteLock _tom;
        };
      }
    }
  }
}

#endif // !CCB_PROCESSING_FAILOVER_HH
