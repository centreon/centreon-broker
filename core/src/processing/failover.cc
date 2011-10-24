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
#include <QTimer>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/with_pointer.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
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
  : _is_out(is_out),
    _retry_interval(30),
    _should_exit(false) {
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
     _is_out(f._is_out),
     _retry_interval(f._retry_interval),
     _should_exit(false) {
  {
    QMutexLocker lock(&f._datam);
    _data = f._data;
  }
  {
    QMutexLocker lock(&f._fromm);
    _from = f._from;
  }
  {
    QMutexLocker lock(&f._tom);
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
      QMutexLocker lock(&f._fromm);
      _from = f._from;
    }
    {
      QMutexLocker lock(&f._tom);
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
  _should_exit = (!in || !out);
  {
    QMutexLocker lock(&_fromm);
    if (!_from.isNull())
      _from->process(in, out);
  }
  {
    QMutexLocker lock(&_tom);
    if (!_to.isNull())
      _to->process(in, out);
  }
  if (!in || !out) {
    exit();
    _feeder.exit();
  }
  return ;
}

/**
 *  Read data.
 *
 *  @param[out] data Data buffer.
 *  @param[out] type Data type.
 */
QSharedPointer<io::data> failover::read() {
  QSharedPointer<io::data> data;
  if (isRunning() && QThread::currentThread() != this) {
    logging::debug(logging::low)
      << "failover: reading event from a different thread";
    bool caught(false);
    try {
      {
        QMutexLocker lock(&_tom);
        if (!_to.isNull())
          data = _to->read();
      }
      logging::debug(logging::low)
        << "failover: got event from remote thread";
    }
    catch (...) {
      caught = true;
    }
    if (caught || data.isNull()) {
      logging::debug(logging::low)
        << "failover: could not get event from remote thread";
      logging::info(logging::medium)
        << "failover: requesting failover thread termination";

      // Exit this thread.
      _should_exit = true;
      if (_is_out && _failover.isNull()) {
        QMutexLocker lock(&_fromm);
        _from->process(false, true);
      }
      exit();
      _feeder.exit();

      this->wait();
      {
        QMutexLocker lock(&_tom);
        _to.clear();
      }
      data = this->read();
    }
  }
  else {
    QMutexLocker lock(&_datam);
    if (!_data.isNull()) {
      data = _data;
      _data.clear();
      lock.unlock();
    }
    else {
      QMutexLocker lock(&_fromm);
      data = _from->read();
    }
    logging::debug(logging::low)
      << "failover: got event from thread source";
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
  if (!_failover.isNull()) {
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
  }

  // Failover should continue as long as not exit request was received.
  logging::debug(logging::medium) << "failover: launching loop";
  _should_exit = false;

  while (!_should_exit) {
    try {
      // Close previous endpoint if any and then open it.
      logging::debug(logging::medium) << "failover: opening endpoint";
      QMutex* m;
      QSharedPointer<io::stream>* s;
      if (_is_out) {
        m = &_tom;
        s = &_to;
      }
      else {
        m = &_fromm;
        s = &_from;
      }
      {
        QMutexLocker lock(m);
        emit initial_lock();
        s->clear();
        *s = _endpoint->open();
        if (s->isNull()) { // Retry connection.
          logging::debug(logging::medium)
            << "failover: resulting stream is nul, retrying";
          continue ;
        }
      }

      // Process input and output.
      logging::debug(logging::medium) << "failover: launching feeder";
      {
        QMutexLocker lock1(&_fromm);
        QMutexLocker lock2(&_tom);
        _feeder.prepare(_from, _to);
      }
      _feeder.run(); // Yes run(), we do not want to start another thread.
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
      QMutexLocker lock(&_tom);
      _to.clear();
    }
    else {
      QMutexLocker lock(&_fromm);
      _from.clear();
    }
    if (!_failover.isNull() && !_failover->isRunning() && !_should_exit)
      _failover->start();
    if (!_should_exit) {
      logging::info(logging::medium) << "failover: sleeping "
        << _retry_interval << " seconds before reconnection";
      QTimer::singleShot(_retry_interval * 1000, this, SLOT(quit()));
      exec();
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
    QMutexLocker lock(&_fromm);
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
