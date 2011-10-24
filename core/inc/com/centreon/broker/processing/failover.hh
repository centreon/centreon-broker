/*
** Copyright 2011 Merethis
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

#ifndef CCB_PROCESSING_FAILOVER_HH_
# define CCB_PROCESSING_FAILOVER_HH_

# include <QMutex>
# include <QSharedPointer>
# include <QThread>
# include <time.h>
# include "com/centreon/broker/io/endpoint.hh"
# include "com/centreon/broker/io/stream.hh"
# include "com/centreon/broker/processing/feeder.hh"

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         processing {
        /**
         *  @class failover failover.hh "com/centreon/broker/processing/failover.hh"
         *  @brief Failover thread.
         *
         *  Thread that provide failover on inputs or outputs.
         */
        class           failover : public QThread, public io::stream {
          Q_OBJECT

         private:
          QSharedPointer<io::data>
                        _data;
          QSharedPointer<io::stream>
                        _destination;
          QSharedPointer<io::endpoint>
                        _endpoint;
          QSharedPointer<failover>
                        _failover;
          feeder        _feeder;
          bool          _is_out;
          time_t        _retry_interval;
          volatile bool _should_exit;
          QSharedPointer<io::stream>
                        _source;
          QSharedPointer<io::stream>
                        _stream;
          QMutex        _streamm;

         public:
                        failover(bool is_out);
                        failover(failover const& f);
                        ~failover();
          failover&     operator=(failover const& f);
          time_t        get_retry_interval() const throw ();
          void          process(bool in = false, bool out = false);
          QSharedPointer<io::data>
                        read();
          void          run();
          void          set_endpoint(QSharedPointer<io::endpoint> endp);
          void          set_failover(QSharedPointer<processing::failover> fo);
          void          set_retry_interval(time_t retry_interval);
          void          wait();
          void          write(QSharedPointer<io::data> d);

         signals:
          void          exception_caught();
          void          initial_lock();
        };
      }
    }
  }
}

#endif /* !CCB_PROCESSING_FAILOVER_HH_ */
