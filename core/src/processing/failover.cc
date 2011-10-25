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

#include <QMutexLocker>
#include <QReadLocker>
#include <QTimer>
#include <QWriteLocker>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/with_pointer.hh"
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
 *  @param[in] is_out true if the failover thread is an output thread.
 */
failover::failover(bool is_out)
  : _initial(true),
    _is_out(is_out),
    _retry_interval(30),
    _should_exit(false),
    _should_exitm(QMutex::Recursive) {
  if (_is_out)
    _from = QSharedPointer<io::stream>(new multiplexing::subscriber);
  else
    _to = QSharedPointer<io::stream>(new multiplexing::publisher);
}

/**
 *  Copy constructor.
 *
 *  @param[in] f Object to copy.
 */
failover::failover(failover const& f)
  :  QThread(),
     io::stream(),
     _endpoint(f._endpoint),
     _failover(f._failover),
     _initial(true),
     _is_out(f._is_out),
     _retry_interval(f._retry_interval),
     _should_exit(false),
     _should_exitm(QMutex::Recursive) {
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
failover::~failover() {}

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
  _should_exit = (!in || !out);

  // Full delayed shutdown.
  if (!in && !out) {
    _endpoint->close();
    bool tweaked_in(_failover.isNull());
    QReadLocker lock(&_fromm);
    if (!_from.isNull())
      _from->process(tweaked_in, false);
  }
  // Single delayed shutdown.
  else if (in && !out) {
    _endpoint->close();
    QReadLocker lock(&_fromm);
    if (!_from.isNull())
      _from->process(true, false);
  }
  // Immediate shutdown.
  else if (!in && out) {
    _endpoint->close();
    QList<QSharedPointer<QMutexLocker> > locks;
    bool all_failover_not_running(true);
    failover* last(this);
    for (QSharedPointer<failover> fo = _failover;
         !fo.isNull();
         fo = fo->_failover) {
      QSharedPointer<QMutexLocker>
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
    process(true, true);
  }
  // Reinitialization.
  else {
    QReadLocker rl(&_fromm);
    if (!_from.isNull())
      _from->process(true, true);
  }

  // Quit event loop.
  quit();

  return ;
}

/**
 *  Read data.
 *
 *  @param[out] data Data buffer.
 *  @param[out] type Data type.
 */
QSharedPointer<io::data> failover::read() {
  // Read retained data.
  QSharedPointer<io::data> data;
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
        data = _to->read();
      logging::debug(logging::low)
        << "failover: got retained event from failover thread";
    }
    catch (...) {
      caught = true;
    }

    // End of destination is reached, shutdown this thread.
    if (caught || data.isNull()) {
      logging::debug(logging::low)
        << "failover: could not get event from failover thread";
      logging::info(logging::medium)
        << "failover: requesting failover thread termination";

      // Reset lock.
      _to.clear();
      _tom.unlock();

      // Exit this thread immediately.
      process(false, true);

      // Recursive data reading.
      data = this->read();
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
      data = _from->read();
    }
    logging::debug(logging::low)
      << "failover: got event from normal source";
  }
  return (data);
}

/**
 *  Thread core function.
 */
void failover::run() {
  // Check endpoint.
  if (_endpoint.isNull()) {
    logging::error(logging::high) << "failover: attempt to run a " \
      "thread with a non-existent endpoint";
    return ;
  }

  // Launch subfailover to fetch retained data.
  if (_initial && !_failover.isNull()) {
    connect(&*_failover, SIGNAL(exception_caught()), SLOT(quit()));
    connect(&*_failover, SIGNAL(initial_lock()), SLOT(quit()));
    connect(&*_failover, SIGNAL(finished()), SLOT(quit()));
    connect(&*_failover, SIGNAL(terminated()), SLOT(quit()));
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
  logging::debug(logging::medium) << "failover: launching loop";
  QMutexLocker exit_lock(&_should_exitm);
  _should_exit = false;

  while (!_should_exit) {
    exit_lock.unlock();
    try {
      // Close previous endpoint if any and then open it.
      logging::debug(logging::medium) << "failover: opening endpoint";
      QReadWriteLock* rwl;
      QSharedPointer<io::stream>* s;
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
        QSharedPointer<io::stream> tmp(_endpoint->open());
        wl.relock();
        emit initial_lock();
        *s = tmp;
        if (s->isNull()) { // Retry connection.
          logging::debug(logging::medium)
            << "failover: resulting stream is nul, retrying";
          exit_lock.relock();
          continue ;
        }
      }

      // Process input and output.
      logging::debug(logging::medium) << "failover: launching feeding";
      QSharedPointer<io::data> data;
      exit_lock.relock();
      while (!_should_exit) {
        exit_lock.unlock();
        try {
          {
            QReadLocker lock(&_fromm);
            if (!_from.isNull())
              data = _from->read();
          }
          if (data.isNull()) {
            exit_lock.relock();
            _should_exit = true;
            exit_lock.unlock();
          }
          else {
            QWriteLocker lock(&_tom);
            if (!_to.isNull())
              _to->write(data);
          }
        }
        catch (exceptions::msg const& e) {
          try {
            throw (exceptions::with_pointer(e, data));
          }
          catch (exceptions::with_pointer const& e) {
            throw ;
          }
          catch (...) {}
          throw ;
        }
        data.clear();
        exit_lock.relock();
      }
    }
    catch (exceptions::with_pointer const& e) {
      logging::error(logging::high) << e.what();
      if (!e.ptr().isNull() && !_failover.isNull() && !_failover->isRunning())
        _failover->write(e.ptr());
    }
    catch (io::exceptions::shutdown const& e) {
      logging::info(logging::medium)
        << "failover: a stream has shutdown: " << e.what();
    }
    catch (exceptions::msg const& e) {
      logging::error(logging::high) << e.what();
    }
    catch (std::exception const& e) {
      logging::error(logging::high)
        << "failover: standard library error: " << e.what();
    }
    catch (...) {
      logging::error(logging::high)
        << "failover: unknown error caught in processing thread";
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

    if (!_failover.isNull() && !_failover->isRunning() && !_should_exit)
      _failover->start();
    if (!_should_exit) {
      // Unlock thread lock.
      exit_lock.unlock();
      logging::info(logging::medium) << "failover: sleeping "
        << _retry_interval << " seconds before reconnection";
      QTimer::singleShot(_retry_interval * 1000, this, SLOT(quit()));
      exec();
      // Relock thread lock.
      exit_lock.relock();
    }
  }
  return ;
}

/**
 *  Set thread endpoint.
 *
 *  @param[in] endp Thread endpoint.
 */
void failover::set_endpoint(QSharedPointer<io::endpoint> endp) {
  _endpoint = endp;
  return ;
}

/**
 *  Set the thread's failover.
 *
 *  @param[in] fo Thread's failover.
 */
void failover::set_failover(QSharedPointer<failover> fo) {
  _failover = fo;
  if (!fo.isNull() && _is_out) { // failover object will act as input for output threads.
    QWriteLocker lock(&_fromm);
    _from = _failover;
  }
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
 *  Wait for this thread to terminate along with other failovers.
 */
void failover::wait() {
  if (!_failover.isNull())
    _failover->wait();
  this->QThread::wait();
  return ;
}

/**
 *  Write data.
 *
 *  @param[in] d Unused.
 *
 *  @return Does not return, throw an exception.
 */
void failover::write(QSharedPointer<io::data> d) {
  QMutexLocker lock(&_datam);
  _data = d;
  return ;
}
