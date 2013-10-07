/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_PROCESSING_FAILOVER_HH
#  define CCB_PROCESSING_FAILOVER_HH

#  include <climits>
#  include <ctime>
#  include <memory>
#  include <QMutex>
#  include <QReadWriteLock>
#  include <QString>
#  include <QThread>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/processing/feeder.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      // Forward declaration.
      namespace          stats {
        class            worker;
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
          friend class   stats::worker;

         public:
                         failover(
                           misc::shared_ptr<io::endpoint> endp,
                           bool is_out,
                           QString const& name = "(unknown)");
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
          void           statistics(std::string& buffer) const;
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
