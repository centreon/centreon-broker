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

#include <cstring>
#include <ctime>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QReadLocker>
#include <QTimer>
#include <QWriteLocker>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
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
 *  @param[in] is_out    true if the failover thread is an output
 *                       thread.
 *  @param[in] name      The failover name.
 */
failover::failover(bool is_out, QString const& name)
  : _buffering_timeout(0),
    _initial(true),
    _is_out(is_out),
    _last_connect_attempt(0),
    _last_connect_success(0),
    _last_event(0),
    _name(name),
    _next_timeout((time_t)-1),
    _read_timeout((time_t)-1),
    _retry_interval(30),
    _update(false),
    _immediate(true),
    _should_exit(false),
    _should_exitm(QMutex::Recursive) {
  if (_is_out)
    _from = misc::shared_ptr<io::stream>(new multiplexing::subscriber(name));
  else
    _to = misc::shared_ptr<io::stream>(new multiplexing::publisher);
}

/**
 *  Copy constructor.
 *
 *  @param[in] f Object to copy.
 */
failover::failover(failover const& f)
  :  QThread(),
     io::stream(),
     _buffering_timeout(f._buffering_timeout),
     _endpoint(f._endpoint),
     _failover(f._failover),
     _initial(true),
     _is_out(f._is_out),
     _last_connect_attempt(f._last_connect_attempt),
     _last_connect_success(f._last_connect_success),
     _last_event(f._last_event),
     _name(f._name),
     _next_timeout(f._next_timeout),
     _read_timeout(f._read_timeout),
     _retry_interval(f._retry_interval),
     _update(false),
     _immediate(true),
     _should_exit(false),
     _should_exitm(QMutex::Recursive) {
  memcpy(_events, f._events, sizeof(_events));
  {
    QMutexLocker lock(&f._datam);
    _data = f._data;
  }
  {
    QWriteLocker lock(&f._fromm);
    _from = f._from;
  }
  {
    QWriteLocker lock(&f._tom);
    _to = f._to;
  }
}

/**
 *  Destructor.
 */
failover::~failover() { QThread::wait(); }

/**
 *  Assignment operator.
 *
 *  @param[in] f Object to copy.
 *
 *  @return This object.
 */
failover& failover::operator=(failover const& f) {
  if (this != &f) {
    _endpoint = f._endpoint;
    _failover = f._failover;
    _is_out = f._is_out;
    _name = f._name;
    _next_timeout = f._next_timeout;
    _read_timeout = f._read_timeout;
    _retry_interval = f._retry_interval;
    {
      QMutexLocker lock(&f._datam);
      _data = f._data;
    }
    {
      QWriteLocker lock(&f._fromm);
      _from = f._from;
    }
    {
      QWriteLocker lock(&f._tom);
      _to = f._to;
    }
  }
  return (*this);
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
 *  Get event processing speed.
 *
 *  @return Number of events processed per second.
 */
double failover::get_event_processing_speed() const throw () {
  time_t now(time(NULL));
  unsigned int events(0);
  if (now >= _last_event) {
    int limit(event_window_length - now + _last_event);
    for (int i(0);
         i < limit;
         ++i)
      events += _events[i];
  }
  return (static_cast<double>(events) / event_window_length);
}

/**
 *  Get the time at which the last event was processed.
 *
 *  @return Time at which the last event was processed.
 */
time_t failover::get_last_event() const throw () {
  return (_last_event);
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
 *  Stop or start processing.
 *
 *  @param[in] in  true to process inputs.
 *  @param[in] out true to process outputs.
 */
void failover::process(bool in, bool out) {
  // Set exit flag.
  QMutexLocker lock(&_should_exitm);
  bool should_exit = _should_exit;
  _immediate = (!in && out);
  _should_exit = (!in || !out);

  // Quit event loop.
  if ((!in || !out) && isRunning()) {
    QTimer::singleShot(0, this, SLOT(quit()));
    QCoreApplication::processEvents();
  }

  // Full delayed shutdown.
  if (!in && !out) {
    logging::info(logging::low) << "failover: " << _name
      << " is performing a full delayed shutdown";
    _endpoint->close();
    bool tweaked_in(_failover.isNull());
    QReadLocker lock(&_fromm);
    if (!_from.isNull())
      _from->process(tweaked_in, false);
  }
  // Single delayed shutdown.
  else if (in && !out) {
    logging::info(logging::low) << "failover: " << _name
      << " is performing a single delayed shutdown";
    _endpoint->close();
    QReadLocker lock(&_fromm);
    if (!_from.isNull())
      _from->process(true, false);
  }
  // Immediate shutdown.
  else if (!in && out) {
    logging::info(logging::low) << "failover: " << _name
      << " is performing an immediate shutdown";
    _endpoint->close();
    QList<misc::shared_ptr<QMutexLocker> > locks;
    bool all_failover_not_running(true);
    failover* last(this);
    for (misc::shared_ptr<failover> fo = _failover;
         !fo.isNull();
         fo = fo->_failover) {
      misc::shared_ptr<QMutexLocker>
        lock(new QMutexLocker(&fo->_should_exitm));
      all_failover_not_running
        = all_failover_not_running && !fo->isRunning();
      locks.push_back(lock);
      last = fo.data();
    }

    // Shutdown subscriber.
    if (all_failover_not_running) {
      QReadLocker rl(&last->_fromm);
      if (!last->_from.isNull())
        last->_from->process(false, true);
    }

    // Wait for thread to terminate.
    locks.clear();
    lock.unlock();
    QThread::wait();
    logging::debug(logging::medium) << "failover: thread "
      << _name << "(" << this << ") terminated";
    lock.relock();
    process(true, !should_exit);
    lock.unlock();
  }
  // Reinitialization.
  else {
    logging::info(logging::low) << "failover: " << _name
      << " is restoring processing";
    QReadLocker rl(&_fromm);
    if (!_from.isNull())
      _from->process(true, true);
  }

  return ;
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
  // Read retained data.
  data.clear();
  if (timed_out)
    *timed_out = false;
  QMutexLocker exit_lock(&_should_exitm);
  if (isRunning() && QThread::currentThread() != this) {
    // Release thread lock.
    exit_lock.unlock();

    // Read data from destination.
    logging::debug(logging::low)
      << "failover: reading retained data from failover thread";
    bool caught(false);
    QReadLocker tom(&_tom);
    try {
      if (!_to.isNull())
        _to->read(data, timeout, timed_out);
      else
        throw (io::exceptions::shutdown(true, true) << "destination of "
               << _name << " is not currently open");
      logging::debug(logging::low)
        << "failover: got retained event from failover thread";
    }
    catch (...) {
      caught = true;
    }

    // End of destination is reached, shutdown this thread.
    if (caught) {
      logging::debug(logging::low)
        << "failover: could not get event from failover thread "
        << _name << " (" << this << ")";
      logging::info(logging::medium)
        << "failover: requesting failover thread " << _name
        << " termination";

      // Reset lock.
      _to.clear();
      _tom.unlock();

      // Reread should exit.
      exit_lock.relock();
      bool should_exit(_should_exit);
      exit_lock.unlock();

      // Exit this thread immediately.
      process(false, true);

      // Recursive data reading.
      if (!should_exit) {
        failover* th(static_cast<failover*>(QThread::currentThread()));
        logging::info(logging::medium) << "failover: " << _name
          << " is buffering data before recursive read ("
          << _buffering_timeout << "s)";
        QTimer::singleShot(
          _buffering_timeout * 1000,
          th,
          SLOT(quit()));
        th->exec();
        this->read(data, timeout, timed_out);
      }
    }
  }
  // Fetch next available event.
  else {
    // Release thread lock.
    exit_lock.unlock();

    // Try the one retained event.
    QMutexLocker lock(&_datam);
    if (!_data.isNull()) {
      data = _data;
      _data.clear();
      lock.unlock();
    }
    // Read from source.
    else {
      lock.unlock();
      QReadLocker fromm(&_fromm);
      _from->read(data, timeout, timed_out);
    }
    logging::debug(logging::low)
      << "failover: got event from normal source";
  }
  return ;
}

/**
 *  Thread core function.
 */
void failover::run() {
  // Check endpoint.
  if (_endpoint.isNull()) {
    logging::error(logging::high) << "failover: attempt to run "
      << _name << " with a non-existent endpoint";
    return ;
  }

  // Log message.
  logging::info(logging::high) << "failover: " << _name
    << " is starting";
  memset(_events, 0, sizeof(_events));
  _last_event = 0;

  // Launch subfailover to fetch retained data.
  if (_initial && !_failover.isNull()) {
    connect(&*_failover, SIGNAL(exception_caught()), SLOT(quit()));
    connect(&*_failover, SIGNAL(initial_lock()), SLOT(quit()));
    connect(&*_failover, SIGNAL(finished()), SLOT(quit()));
    connect(&*_failover, SIGNAL(terminated()), SLOT(quit()));
    logging::info(logging::medium) << "failover: launching failover "
      << _failover->_name;
    _failover->start();
    exec();
    disconnect(
      &*_failover,
      SIGNAL(exception_caught()),
      this,
      SLOT(quit()));
    disconnect(
      &*_failover,
      SIGNAL(initial_lock()),
      this,
      SLOT(quit()));
    disconnect(
      &*_failover,
      SIGNAL(finished()),
      this,
      SLOT(quit()));
    disconnect(
      &*_failover,
      SIGNAL(terminated()),
      this,
      SLOT(quit()));
    _initial = false;
  }

  // Failover should continue as long as not exit request was received.
  logging::debug(logging::medium) << "failover: "
    << _name << " is launching loop";
  QMutexLocker exit_lock(&_should_exitm);

  time_t buffering(0);
  while (!_should_exit) {
    misc::shared_ptr<io::stream> copy_handler;
    exit_lock.unlock();
    try {
      // Close previous endpoint if any and then open it.
      logging::debug(logging::medium) << "failover: "
        << _name << " is opening its endpoint";
      QReadWriteLock* rwl;
      misc::shared_ptr<io::stream>* s;
      if (_is_out) {
        rwl = &_tom;
        s = &_to;
      }
      else {
        rwl = &_fromm;
        s = &_from;
      }
      {
        QWriteLocker wl(rwl);
        s->clear();
        wl.unlock();
        _last_connect_attempt = time(NULL);
        _update = false;
        misc::shared_ptr<io::stream> tmp(_endpoint->open());
        buffering = 0;
        wl.relock();
        emit initial_lock();
        *s = tmp;
        if (s->isNull()) { // Retry connection.
          logging::debug(logging::medium)
            << "failover: resulting stream for "
            << _name << " is nul, retrying";
          exit_lock.relock();
          continue ;
        }
        copy_handler = *s;
        _last_connect_success = time(NULL);
      }

      // Initial buffering.
      {
        logging::info(logging::medium)
          << "failover: initialy buffering data in " << _name
          << " (" << _buffering_timeout << "s)";
        QTimer::singleShot(
                  _buffering_timeout * 1000,
                  this,
                  SLOT(quit()));
        exec();
      }

      // Reprocess unacknowledged events.
      _unprocessed.splice(_unprocessed.begin(), _processed);

      // Process input and output.
      logging::debug(logging::medium) << "failover: "
        << _name << " is launching feeding";
      exit_lock.relock();
      while (!_should_exit || !_immediate) {
        exit_lock.unlock();
        bool timed_out(false);
        if (_unprocessed.empty()) {
          QReadLocker lock(&_fromm);
          if (!_from.isNull()) {
            if (_update && !_is_out) {
              _update = false;
              _from->update();
            }
            {
              misc::shared_ptr<io::data> data;
              _from->read(data, _next_timeout, &timed_out);
              _unprocessed.push_back(data);
            }
            if (timed_out && (_read_timeout != (time_t)-1))
              _next_timeout = time(NULL) + _read_timeout;
          }
        }
        QWriteLocker lock(&_tom);
        if (!_to.isNull()) {
          if (_update && _is_out) {
            _update = false;
            _to->update();
          }
          unsigned int written(_to->write(_unprocessed.front()));
          time_t now(time(NULL));
          if (!_unprocessed.front().isNull()) {
            if (now > _last_event) {
              time_t limit(now - _last_event);
              if (limit > event_window_length)
                limit = event_window_length;
              memmove(
                _events + limit,
                _events,
                (event_window_length - limit) * sizeof(*_events));
              memset(_events, 0, limit * sizeof(*_events));
              _last_event = now;
            }
            else if (now < _last_event) {
              memset(_events, 0, event_window_length * sizeof(*_events));
              _last_event = now;
            }
            ++_events[0];
          }
          _processed.push_back(_unprocessed.front());
          _unprocessed.pop_front();
          for (unsigned int i(0);
               (i < written) && !_processed.empty();
               ++i)
            _processed.pop_front();
        }
        exit_lock.relock();
      }
    }
    catch (io::exceptions::shutdown const& e) {
      logging::info(logging::medium)
        << "failover: a stream has shutdown in "
        << _name << ": " << e.what();
      exit_lock.relock();
      _immediate = true;
      _last_error = e.what();
      _should_exit = true;
      exit_lock.unlock();
    }
    catch (exceptions::msg const& e) {
      logging::error(logging::high) << e.what();
      _last_error = e.what();
    }
    catch (std::exception const& e) {
      logging::error(logging::high)
        << "failover: standard library error in "
        << _name << ": " << e.what();
      _last_error = e.what();
    }
    catch (...) {
      logging::error(logging::high)
        << "failover: unknown error caught in processing thread "
        << _name;
      _last_error = "(unknown)";
    }
    emit exception_caught();
    if (_is_out) {
      QWriteLocker lock(&_tom);
      _to.clear();
    }
    else {
      QWriteLocker lock(&_fromm);
      _from.clear();
    }

    // Relock thread lock.
    exit_lock.relock();

    // Buffering.
    if (!_should_exit) {
      time_t now(time(NULL));
      if (!buffering)
        buffering = now + _buffering_timeout;
      if (buffering > now) {
        time_t diff(buffering - now);
        if (diff > _retry_interval)
          diff = _retry_interval;
        logging::info(logging::medium)
          << "failover: buffering data in " << _name
          << " before launching failover (" << diff << "s)";
        QTimer::singleShot(diff * 1000, this, SLOT(quit()));
        exit_lock.unlock();
        exec();
        exit_lock.relock();
        continue ;
      }
    }

    if (!_failover.isNull() && !_failover->isRunning() && !_should_exit) {
      connect(&*_failover, SIGNAL(exception_caught()), SLOT(quit()));
      connect(&*_failover, SIGNAL(initial_lock()), SLOT(quit()));
      connect(&*_failover, SIGNAL(finished()), SLOT(quit()));
      connect(&*_failover, SIGNAL(terminated()), SLOT(quit()));
      logging::info(logging::medium) << "failover: " << _name
        << " is launching failover thread " << _failover->_name;
      _failover->start();
      exec();
      disconnect(
        &*_failover,
        SIGNAL(exception_caught()),
        this,
        SLOT(quit()));
      disconnect(
        &*_failover,
        SIGNAL(initial_lock()),
        this,
        SLOT(quit()));
      disconnect(
        &*_failover,
        SIGNAL(finished()),
        this,
        SLOT(quit()));
      disconnect(
        &*_failover,
        SIGNAL(terminated()),
        this,
        SLOT(quit()));
    }
    if (!_should_exit) {
      // Unlock thread lock.
      exit_lock.unlock();
      logging::info(logging::medium) << "failover: " << _name
        << " is sleeping " << _retry_interval
        << " seconds before reconnection";
      QTimer::singleShot(_retry_interval * 1000, this, SLOT(quit()));
      exec();
      // Relock thread lock.
      exit_lock.relock();
    }
  }
  logging::info(logging::high) << "failover: "
    << _name << " is exiting";
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
 *  Set thread endpoint.
 *
 *  @param[in] endp Thread endpoint.
 */
void failover::set_endpoint(misc::shared_ptr<io::endpoint> endp) {
  _endpoint = endp;
  return ;
}

/**
 *  Set the thread's failover.
 *
 *  @param[in] fo Thread's failover.
 */
void failover::set_failover(misc::shared_ptr<failover> fo) {
  _failover = fo;
  if (!fo.isNull() && _is_out) { // failover object will act as input for output threads.
    QWriteLocker lock(&_fromm);
    _from = _failover.staticCast<io::stream>();
  }
  return ;
}

/**
 *  Set the read timeout.
 *
 *  @param[in] read_timeout Read timeout.
 */
void failover::set_read_timeout(time_t read_timeout) {
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
 *  Get statistics of the subscriber/publisher.
 *
 *  @param[out] buffer Output buffer.
 */
void failover::statistics(std::string& buffer) const {
  if (_is_out) {
    QReadLocker rl(&_fromm);
    if (!_from.isNull())
      _from->statistics(buffer);
  }
  else {
    QReadLocker rl(&_tom);
    if (!_to.isNull())
      _to->statistics(buffer);
  }
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
 *  @param[in] time Maximum time to wait for thread termination.
 */
bool failover::wait(unsigned long time) {
  if (!_failover.isNull())
    _failover->wait(time);
  return (this->QThread::wait(time));
}

/**
 *  Write data.
 *
 *  @param[in] d Unused.
 *
 *  @return Does not return, throw an exception.
 */
unsigned int failover::write(misc::shared_ptr<io::data> const& d) {
  (void)d;
  throw (exceptions::msg() << "cannot write to failover '"
         << _name << "'");
  return (0);
}
