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
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/with_pointer.hh"
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
  : _is_out(is_out), _retry_interval(30), _should_exit(false) {
  if (_is_out)
    _source = QSharedPointer<io::stream>(new multiplexing::subscriber);
  else
    _destination = QSharedPointer<io::stream>(new multiplexing::publisher);
}

/**
 *  Copy constructor.
 *
 *  @param[in] f Object to copy.
 */
failover::failover(failover const& f)
  :  QThread(),
     io::stream(f),
     _data(f._data),
     _destination(f._destination),
     _endpoint(f._endpoint),
     _failover(f._failover),
     _is_out(f._is_out),
     _retry_interval(f._retry_interval),
     _should_exit(false),
     _source(f._source) {}

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
  _data = f._data;
  _destination = f._destination;
  _endpoint = f._endpoint;
  _failover = f._failover;
  _is_out = f._is_out;
  _retry_interval = f._retry_interval;
  _source = f._source;
  return (*this);
}

/**
 *  Stop or start processing.
 *
 *  @param[in] in  true to process inputs.
 *  @param[in] out true to process outputs.
 */
void failover::process(bool in, bool out) {
  if (!_failover.isNull() && _failover->isRunning())
    _failover->process(in, out);
  _should_exit = true;
  _feeder.exit();
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
    logging::debug << logging::LOW << "failover: reading event from a different thread";
    bool caught(false);
    try {
      {
        QMutexLocker lock(&_streamm);
        if (!_stream.isNull())
          data = _stream->read();
      }
      logging::debug << logging::LOW << "failover: got event from remote thread";
    }
    catch (...) {
      caught = true;
    }
    if (caught || data.isNull()) {
      logging::debug << logging::LOW << "failover: could not get event from remote thread";
      logging::info << logging::MEDIUM << "failover: requesting failover thread termination";
      this->exit();
      this->wait();
      {
        QMutexLocker lock(&_streamm);
        _stream.clear();
      }
      data = this->read();
    }
  }
  else {
    if (!_data.isNull()) {
      data = _data;
      _data.clear();
    }
    else
      data = _source->read();
    logging::debug << logging::LOW << "failover: got event from thread source";
  }
  return (data);
}

/**
 *  Thread core function.
 */
void failover::run() {
  // Failover should continue as long as not exit request was received.
  logging::debug << logging::MEDIUM << "failover: launching loop";
  _should_exit = false;
  while (!_should_exit) {
    // Check endpoint.
    if (_endpoint.isNull()) {
      logging::error << logging::HIGH << "failover: attempt to run a " \
        "thread with a non-existent endpoint";
      return ;
    }

    try {
      // Close previous endpoint if any.
      {
        QMutexLocker lock(&_streamm);
        _stream.clear();
      }

      // Open endpoint.
      logging::debug << logging::MEDIUM << "failover: opening endpoint";
      {
        QMutexLocker lock(&_streamm);
        _stream = _endpoint->open();
        if (_stream.isNull()) { // Retry connection.
          lock.unlock();
          logging::debug << logging::MEDIUM << "failover: resulting stream is nul, retrying";
          continue ;
        }
        if (_is_out)
          _destination = _stream;
        else
          _source = _stream;
      }

      // Process input and output.
      logging::debug << logging::MEDIUM << "failover: launching feeder";
      _feeder.prepare(_source, _destination);
      _feeder.run(); // Yes run(), we do not want to start another thread.
    }
    catch (exceptions::with_pointer const& e) {
      logging::error << logging::HIGH << e.what();
      if (!e.ptr().isNull() && !_failover.isNull() && !_failover->isRunning())
        _failover->write(e.ptr());
    }
    catch (exceptions::msg const& e) {
      logging::error << logging::HIGH << e.what();
    }
    catch (std::exception const& e) {
      logging::error << logging::HIGH << "failover: standard library error: " << e.what();
    }
    catch (...) {
      logging::error << logging::HIGH << "failover: unknown error caught in processing thread";
     }
    if (_is_out)
      _destination.clear();
    else
      _source.clear();
    if (!_failover.isNull() && !_failover->isRunning())
      _failover->start();
    if (!_should_exit) {
      logging::info << logging::MEDIUM << "failover: sleeping "
        << _retry_interval << " seconds before reconnection";
      QThread::sleep(_retry_interval);
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
  if (!fo.isNull() && _is_out) // failover object will act as input for output threads.
    _source = _failover;
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
  if (!_failover.isNull() && _failover->isRunning())
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
  _data = d;
  return ;
}
