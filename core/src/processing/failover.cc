/*
** Copyright 2011-2014 Merethis
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

#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/processing/failover.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in]     endp     Failover thread endpoint.
 *  @param[in]     is_out   true if the failover thread is an output
 *                          thread.
 *  @param[in]     name     The failover name.
 *  @param[in]     filters  Event filters.
 */
failover::failover(
            misc::shared_ptr<io::endpoint> endp,
            misc::shared_ptr<multiplexing::subscriber> sbscrbr,
            QString const& name)
  : _buffering_timeout(0),
    _endpoint(endp),
    _name(name),
    _next_timeout((time_t)-1),
    _read_timeout((time_t)-1),
    _retry_interval(30),
    _subscriber(sbscrbr),
    _update(false),
    _should_exit(false) {}

/**
 *  Destructor.
 */
failover::~failover() { QThread::wait(); }

/**
 *  Add secondary endpoint to this failover thread.
 *
 *  @param[in] endp  New secondary endpoint.
 */
void failover::add_secondary_endpoint(
                 misc::shared_ptr<io::endpoint> endp) {
  _secondary_endpoints.push_back(endp);
  return ;
}

/**
 *  Get buffering timeout.
 *
 *  @return Failover thread buffering timeout.
 */
time_t failover::get_buffering_timeout() const throw () {
  return (_buffering_timeout);
}

/**
 *  Get read timeout.
 *
 *  @return Failover thread read timeout.
 */
time_t failover::get_read_timeout() const throw () {
  return (_read_timeout);
}

/**
 *  Get retry interval.
 *
 *  @return Failover thread retry interval.
 */
time_t failover::get_retry_interval() const throw () {
  return (_retry_interval);
}

/**
 *  Read data.
 *
 *  @param[out] d Data.
 */
void failover::read(misc::shared_ptr<io::data>& d) {
  this->read(d, (time_t)-1);
  return ;
}

/**
 *  Read data.
 *
 *  @param[out] data      Data.
 *  @param[in]  timeout   Read timeout.
 *  @param[out] timed_out Set to true if read timed out.
 */
void failover::read(
                 misc::shared_ptr<io::data>& data,
                 time_t timeout,
                 bool* timed_out) {
  // XXX
}

/**
 *  Thread core function.
 */
void failover::run() {
  // Initial log.
  logging::debug(logging::high)
    << "failover: thread of endpoint '" << _name << "' is starting";

  // Check endpoint.
  if (_endpoint.isNull()) {
    logging::error(logging::high)
      << "failover: thread of endpoint '" << _name << "' has no endpoint"
      << " object, this is likely a software bug that should be reported"
      << " to Centreon Broker developers";
    return ;
  }

  // Initial launch of failovers (to read retained data).
  logging::debug(logging::medium)
    << "failover: initializing failovers of endpoint '" << _name << "'";
  _update_status("initializing failovers");
  _launch_failover();
  _update_status("");

  // Thread should be aware of external exit requests.
  QMutexLocker exit_lock(&_should_exitm);
  do {
    exit_lock.unlock();

    // This try/catch block handle any errors of the current thread
    // objects. In case of an exception, it is responsible to launch
    // failovers of this failover.
    try {
      // Attempt to open endpoint.
      _update_status("opening endpoint");
      _stream = _endpoint->open();
      _update_status("");

      // Buffering.
      if (_buffering_timeout > 0) {
        // Status.
        logging::debug(logging::medium)
          << "failover: buffering data for endpoint '" << _name
          << "' (" << _buffering_timeout << "s)";
        _update_status("buffering data");

        // Wait loop.
        time_t valid_time(time(NULL) + _buffering_timeout);
        exit_lock.relock();
        QTimer::singleShot(
                  _buffering_timeout * 1000,
                  this,
                  SLOT(quit()));
        do {
          exec();
        } while (!_should_exit && (valid_time < time(NULL)));
        _update_status("");
      }

      // Event processing loop.
      logging::debug(logging::medium)
        << "failover: launching event loop of endpoint '"
        << _name << "'";
      exit_lock.relock();
      while (!_should_exit) {
        exit_lock.unlock();

        // Read next event that should be processed by the stream.
        misc::shared_ptr<io::data> d;
        _get_next_event(d);

        // Write data to the stream and acknowledge processed events.
        _update_status("writting event");
        unsigned int written(_stream->write(d));
        // XXX : write to secondaries
        _processed_events(written);

        // Relock exit lock to check exit flag.
        exit_lock.relock();
      }
      exit_lock.unlock();
    }
    // Some real error occured.
    catch (std::exception const& e) {
      logging::error(logging::high) << e.what();
      _stream.clear();
      _launch_failover();
    }
    catch (...) {
      logging::error(logging::high) << "failover: endpoint '" << _name
        << "' encountered an unknown exception, this is likely a "
        << "software bug that should be reported to Centreon Broker developers";
      _stream.clear();
      _launch_failover();
    }

    // Relock exit lock to check exit flag.
    exit_lock.relock();

    // Sleep a while before attempting a reconnection.
    if (!_should_exit) {
      exit_lock.unlock();
      _stream.clear();
      _update_status("sleeping before reconnection");
      QTimer::singleShot(_retry_interval * 1000, this, SLOT(quit()));
      exec();
      _update_status("");
      exit_lock.relock();
    }
  } while (!_should_exit);
  exit_lock.unlock();

  // Exit log.
  logging::debug(logging::high)
    << "failover: thread of endpoint '" << _name << "' is exiting";

  return ;
}

/**
 *  Set buffering timeout.
 *
 *  @param[in] secs Buffering timeout in seconds.
 */
void failover::set_buffering_timeout(time_t secs) {
  _buffering_timeout = secs;
  return ;
}

/**
 *  Set the thread's failover.
 *
 *  @param[in] fo Thread's failover.
 */
void failover::set_failover(misc::shared_ptr<failover> fo) {
  _failover = fo;
  return ;
}

/**
 *  Set the read timeout.
 *
 *  @param[in] read_timeout Read timeout.
 */
void failover::set_read_timeout(time_t read_timeout) {
  // A read_timeout of zero should never exist.
  if (read_timeout == 0)
    read_timeout = (time_t)-1;
  _read_timeout = read_timeout;
  if (_read_timeout != (time_t)-1)
    _next_timeout = time(NULL) + _read_timeout;
  else
    _next_timeout = (time_t)-1;
  return ;
}

/**
 *  Set the connection retry interval.
 *
 *  @param[in] retry_interval Time to wait between two connection
 *                            attempts.
 */
void failover::set_retry_interval(time_t retry_interval) {
  _retry_interval = retry_interval;
  return ;
}

/**
 *  Get statistics of the failover.
 *
 *  @param[out] tree  Properties tree.
 */
void failover::statistics(io::properties& tree) const {
  // XXX
  return ;
}

/**
 *  Configuration update request.
 */
void failover::update() {
  _update = true;
  return ;
}

/**
 *  Wait for this thread to terminate along with other failovers.
 *
 *  @param[in] time  Maximum time to wait for thread termination.
 *
 *  @return True if thread exited.
 */
bool failover::wait(unsigned long time) {
  if (!_failover.isNull())
    _failover->wait(time);
  bool finished(this->QThread::wait(time));
  if (finished)
    _should_exit = false;
  return (finished);
}

/**
 *  Write data.
 *
 *  @param[in] d  Unused.
 *
 *  @return Does not return, throw an exception.
 */
unsigned int failover::write(misc::shared_ptr<io::data> const& d) {
  (void)d;
  throw (exceptions::msg() << "cannot write to endpoint '"
         << _name << "'");
  return (0);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Update status message.
 *
 *  @param[in] status New status.
 */
void failover::_update_status(std::string const& status) {
  QMutexLocker lock(&_statusm);
  _status = status;
  return ;
}
