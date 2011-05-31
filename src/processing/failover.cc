/*
** Copyright 2011 Merethis
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

#include "exceptions/basic.hh"
#include "logging/logging.hh"
#include "multiplexing/publisher.hh"
#include "multiplexing/subscriber.hh"
#include "processing/failover.hh"
#include "processing/feeder.hh"

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
failover::failover(bool is_out) : _is_out(is_out) {
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
     _destination(f._destination),
     _endpoint(f._endpoint),
     _failover(f._failover),
     _is_out(f._is_out),
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
  _destination = f._destination;
  _endpoint = f._endpoint;
  _failover = f._failover;
  _is_out = f._is_out;
  _source = f._source;
  return (*this);
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
    try {
      data = _stream->read();
      if (data.isNull() || !data->size()) {
        // XXX : quit thread
        _stream.clear();
        this->wait();
      }
    }
    catch (...) {
      data = _source->read();
    }
  }
  else
    data = _source->read();
  return (data);
}

/**
 *  Thread core function.
 */
void failover::run() {
  // Failover should continue as long as not exit request was received.
  while (true) {
    // Check endpoint.
    if (_endpoint.isNull()) {
      logging::error << logging::HIGH << "attempt to run a failover " \
        "thread with a non-existent endpoint";
      return ;
    }

    try {
      // Open endpoint.
      _stream = _endpoint->open();
      if (_stream.isNull()) // Retry connection.
        continue ;
      if (_is_out)
        _destination = _stream;
      else
        _source = _stream;

      // Process input and output.
      feeder f;
      f.prepare(_source, _destination);
      f.run(); // Yes run(), we do not want to start another thread.
    }
    catch (...) {
      if (!_failover.isNull() && !_failover->isRunning())
        _failover->start();
      sleep(5); // XXX : should be configurable
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
  if (_is_out) // failover object will act as input for output threads.
    _source = _failover;
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
  (void)d;
  throw (exceptions::basic() << "attempt to use a failover thread as a destination (software bug)");
  return ;
}
