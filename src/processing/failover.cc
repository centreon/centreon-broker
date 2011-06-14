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
failover::failover(bool is_out) : _is_out(is_out), _should_exit(false) {
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
  _destination = f._destination;
  _endpoint = f._endpoint;
  _failover = f._failover;
  _is_out = f._is_out;
  _source = f._source;
  return (*this);
}

/**
 *  Request thread termination.
 */
void failover::exit() {
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
      data = _stream->read();
      logging::debug << logging::LOW << "failover: got event from remote thread";
    }
    catch (...) {
      caught = true;
    }
    if (caught || data.isNull() || !data->size()) {
      logging::debug << logging::LOW << "failover: could not get event from remote thread";
      logging::info << logging::MEDIUM << "failover: requesting failover thread termination";
      this->exit();
      this->wait();
      _stream.clear();
      data = this->read();
    }
  }
  else {
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
      logging::error << logging::HIGH << "attempt to run a failover " \
        "thread with a non-existent endpoint";
      return ;
    }

    try {
      // Close previous endpoint if any.
      _stream.clear();

      // Open endpoint.
      logging::debug << logging::MEDIUM << "failover: opening endpoint";
      _stream = _endpoint->open();
      if (_stream.isNull()) { // Retry connection.
        logging::debug << logging::MEDIUM << "failover: resulting stream is nul, retrying";
        continue ;
      }
      if (_is_out)
        _destination = _stream;
      else
        _source = _stream;

      // Process input and output.
      logging::debug << logging::MEDIUM << "failover: launching feeder";
      _feeder.prepare(_source, _destination);
      _feeder.run(); // Yes run(), we do not want to start another thread.
    }
    catch (exceptions::basic const& e) {
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
    logging::info << logging::MEDIUM << "failover: sleeping a while before reconnection";
    QThread::sleep(5);
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
