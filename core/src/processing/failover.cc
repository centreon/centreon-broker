/*
** Copyright 2011-2015 Merethis
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

#include <QTimer>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
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
    _failover_launched(false),
    _name(name),
    _next_timeout((time_t)-1),
    _read_timeout((time_t)-1),
    _retry_interval(30),
    _subscriber(sbscrbr),
    _update(false) {}

/**
 *  Destructor.
 */
failover::~failover() {}

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
 *  Exit failover thread.
 */
void failover::exit() {
  thread::exit();
  QMutexLocker acceptor_lock(&_acceptorm);
  if (_acceptor.get())
    _acceptor->exit();
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
  // The read() method is used by external objects to read from the main
  // stream contained in this object. Typically this method is called
  // by another failover object that uses this object as its failover
  // endpoint.

  // First we try to read from the main stream.
  QMutexLocker stream_lock(&_streamm);
  if (!_stream.isNull()) {
    try {
      _stream->read(data, timeout, timed_out);
    }
    catch (std::exception const& e) {
      // In the run() method, it is guaranteed that no more write will
      // occur on the stream if thread exit was requested. It is
      // important to unlock the stream lock only after setting the exit
      // flag.
      if (isRunning()) {
        exit();
        stream_lock.unlock();
        wait();
      }
      else {
        _stream.clear();
        stream_lock.unlock();
      }
      logging::info(logging::high)
        << "failover: endpoint '" << _name
        << "' main stream cannot be read anymore, will try failover: "
        << e.what();

      // Now that the stream is cleared and mutex released, try to read
      // from failover.
      read(data, timeout, timed_out);
    }
  }
  // If the main stream was not ready to provide events, try the
  // failover thread.
  else {
    stream_lock.unlock();
    if (!_failover.isNull())
      _failover->read(data, timeout, timed_out);
    else
      throw (io::exceptions::shutdown(true, true)
             << "failover: endpoint '" << _name
             << "' does not have further events");
  }
  return ;
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
  do {
    // This try/catch block handle any errors of the current thread
    // objects. In case of an exception, it is responsible to launch
    // failovers of this failover.
    try {
      // Acceptor.
      if (_endpoint->is_acceptor()) {
        // First discard all events of the failover (if necessary).
        if (_failover_launched) {
          try {
            while (true) {
              misc::shared_ptr<io::data> d;
              _failover->read(d, (time_t)-1, NULL);
            }
          }
          catch (...) { // Silently discard errors, they're of no use.
            _failover->exit();
            _failover->wait();
            _failover_launched = false;
          }
        }

        // Create and run acceptor loop.
        {
          QMutexLocker acceptor_lock(&_acceptorm);
          _acceptor.reset(new processing::acceptor(
                                            _endpoint,
                                            processing::acceptor::out,
                                            _name.toStdString()));
        }
        try {
          while (!should_exit())
            _acceptor->accept();
        }
        catch (...) {
          QMutexLocker acceptor_lock(&_acceptorm);
          _acceptor.reset();
          throw ;
        }
        {
          QMutexLocker acceptor_lock(&_acceptorm);
          _acceptor.reset();
        }
      }
      // Connector.
      else {
        // Attempt to open endpoint.
        _update_status("opening endpoint");
        {
          misc::shared_ptr<io::stream> s(_endpoint->open());
          {
            QMutexLocker stream_lock(&_streamm);
            _stream = s;
          }
        }
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
          do {
            QTimer::singleShot(1000, this, SLOT(quit()));
            exec();
          } while (!should_exit() && (time(NULL) < valid_time));
          _update_status("");
        }

        // Open secondaries.
        _update_status("initializing secondaries");
        std::vector<misc::shared_ptr<io::stream> > secondaries;
        for (std::vector<misc::shared_ptr<io::endpoint> >::iterator
               it(_secondary_endpoints.begin()),
               end(_secondary_endpoints.end());
             it != end;
             ++it)
          try {
            misc::shared_ptr<io::stream> s((*it)->open());
            if (!s.isNull())
              secondaries.push_back(s);
            else
              logging::error(logging::medium)
                << "failover: could not open a secondary of endpoint '"
                << _name << ": secondary returned a null stream";
          }
          catch (std::exception const& e) {
            logging::error(logging::medium)
              << "failover: error occured while opening a secondary "
              << "of endpoint '" << _name << "': " << e.what();
          }
        _update_status("");

        // Event processing loop.
        logging::debug(logging::medium)
          << "failover: launching event loop of endpoint '"
          << _name << "'";
        while (!should_exit()) {
          // Check for update.
          if (_update) {
            QMutexLocker stream_lock(&_streamm);
            _stream->update();
            _update = false;
          }

          // Read next event that should be processed by the stream.
          bool timed_out(false);
          misc::shared_ptr<io::data> d;
          _get_next_event(d, _next_timeout, &timed_out);

          // If timeout occured, set next timeout.
          if (timed_out && (_read_timeout != (time_t)-1))
            _next_timeout = time(NULL) + _read_timeout;

          // Write data to the stream and secondaries,
          // and acknowledge processed events.
          _update_status("writing event");
          unsigned int written(0);
          {
            // Here we need to check the exit flag to avoid writing in a
            // stream that could be wished to be terminated (normal exit
            // or by read(). It is very important to lock the stream
            // first and then the exit flag. It prevents deadlock that
            // could occur with the read() method.
            QMutexLocker stream_lock(&_streamm);
            if (!should_exit())
              written = _stream->write(d);
          }
          for (std::vector<misc::shared_ptr<io::stream> >::iterator
                 it(secondaries.begin()),
                 end(secondaries.end());
               it != end;) {
            try {
              (*it)->write(d);
              ++it;
            }
            catch (std::exception const& e) {
              logging::error(logging::medium) << "failover: error "
                << "occurred while writing to a secondary of endpoint '"
                << _name << "' (secondary will be removed): "
                << e.what();
              it = secondaries.erase(it);
            }
          }
        }
      }
    }
    // Some real error occured.
    catch (std::exception const& e) {
      logging::error(logging::high) << e.what();
      {
        QMutexLocker stream_lock(&_streamm);
        _stream.clear();
      }
      _launch_failover();
    }
    catch (...) {
      logging::error(logging::high) << "failover: endpoint '" << _name
        << "' encountered an unknown exception, this is likely a "
        << "software bug that should be reported to Centreon Broker developers";
      {
        QMutexLocker stream_lock(&_streamm);
        _stream.clear();
      }
      _launch_failover();
    }

    // Clear stream.
    {
      QMutexLocker stream_lock(&_streamm);
      _stream.clear();
    }

    // Sleep a while before attempting a reconnection.
    _update_status("sleeping before reconnection");
    time_t valid_time(time(NULL) + _retry_interval);
    while (!should_exit() && (time(NULL) < valid_time)) {
      QTimer::singleShot(1000, this, SLOT(quit()));
      exec();
    }
    _update_status("");

  } while (!should_exit());

  // Clear stream.
  {
    QMutexLocker stream_lock(&_streamm);
    _stream.clear();
  }

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
  // Check that failover finished.
  bool finished;
  if (!_failover.isNull())
    finished = _failover->wait(time);
  else
    finished = true;

  // If there was no failover or failover finished we
  // can safely wait for ourselves.
  if (finished)
    finished = thread::wait(time);
  // Otherwise we're not finished yet.
  else
    finished = false;
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
 *  @brief Get next event to process.
 *
 *  This is an internal method used to request the next event that
 *  should be processed by the main stream in the run() method.
 *
 *  @param[out] d          Next event.
 *  @param[in]  timeout    Timeout time.
 *  @param[out] timed_out  Set to true if read operation timed out.
 */
void failover::_get_next_event(
                 misc::shared_ptr<io::data>& d,
                 time_t timeout,
                 bool* timed_out) {
  // Try to read from failover thread first.
  if (_failover_launched) {
    try {
      _failover->read(d, timeout, timed_out);
    }
    catch (std::exception const& e) {
      // Exiting failover thread is optional, it should be done in
      // read() but we're never too careful.
      _failover->exit();
      _failover->wait();
      _failover_launched = false;

      // Recursive read will read from subscriber.
      _get_next_event(d, timeout, timed_out);
    }
  }
  // If it's not possible, read from subscriber.
  else
    _subscriber->read(d, timeout, timed_out);
  return ;
}

/**
 *  Launch failover of this endpoint.
 */
void failover::_launch_failover() {
  if (!_failover.isNull() && !_failover_launched) {
    _failover_launched = true;
    _failover->start();
  }
  return ;
}

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
