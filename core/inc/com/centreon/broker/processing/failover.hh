/*
** Copyright 2011-2013,2015 Merethis
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
#  include <QMutex>
#  include <QString>
#  include <QVector>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/multiplexing/subscriber.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/processing/acceptor.hh"
#  include "com/centreon/broker/processing/thread.hh"

CCB_BEGIN()

// Forward declarations.
namespace           io {
  class             properties;
}
namespace           stats {
  class             builder;
}

namespace           processing {
  /**
   *  @class failover failover.hh "com/centreon/broker/processing/failover.hh"
   *  @brief Failover thread.
   *
   *  Thread that provide failover on output endpoints.
   *
   *  Multiple failover can be forwarded.
   */
  class             failover : public thread, public io::stream {
    friend class    stats::builder;

  public:
                    failover(
                      misc::shared_ptr<io::endpoint> endp,
                      misc::shared_ptr<multiplexing::subscriber> sbscrbr,
                      QString const& name,
                      std::string const& temp_dir);
                    ~failover();
    void            add_secondary_endpoint(
                      misc::shared_ptr<io::endpoint> endp);
    void            exit();
    time_t          get_buffering_timeout() const throw ();
    time_t          get_read_timeout() const throw ();
    time_t          get_retry_interval() const throw ();
    void            read(misc::shared_ptr<io::data>& d);
    void            read(
                      misc::shared_ptr<io::data>& d,
                      time_t timeout,
                      bool* timed_out = NULL);
    void            run();
    void            set_buffering_timeout(time_t secs);
    void            set_failover(
                      misc::shared_ptr<processing::failover> fo);
    void            set_read_timeout(time_t read_timeout);
    void            set_retry_interval(time_t retry_interval);
    void            statistics(io::properties& tree) const;
    void            update();
    bool            wait(unsigned long time = ULONG_MAX);
    unsigned int    write(misc::shared_ptr<io::data> const& d);

  private:
                    failover(failover const& other);
    failover&       operator=(failover const& other);
    void            _get_next_event(
                      misc::shared_ptr<io::data>& d,
                      time_t timeout,
                      bool* timed_out = NULL);
    void            _launch_failover();
    void            _update_status(std::string const& status);

    // Data that doesn't require locking.
    volatile time_t _buffering_timeout;
    misc::shared_ptr<io::endpoint>
                    _endpoint;
    std::vector<misc::shared_ptr<io::endpoint> >
                    _secondary_endpoints;
    misc::shared_ptr<failover>
                    _failover;
    bool            _failover_launched;
    QString         _name;
    time_t          _next_timeout;
    time_t          _read_timeout;
    volatile time_t _retry_interval;
    misc::shared_ptr<multiplexing::subscriber>
                    _subscriber;
    std::string     _temp_dir;
    volatile bool   _update;

    // Acceptor.
    std::auto_ptr<processing::acceptor>
                    _acceptor;
    mutable QMutex  _acceptorm;

    // Status.
    std::string     _status;
    mutable QMutex  _statusm;

    // Stream.
    misc::shared_ptr<io::stream>
                    _stream;
    mutable QMutex  _streamm;
  };
}

CCB_END()

#endif // !CCB_PROCESSING_FAILOVER_HH
