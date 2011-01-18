/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include <assert.h>
#include <memory>
#include <stdlib.h>
#include "concurrency/lock.hh"
#include "interface/ndo/destination.hh"
#include "interface/ndo/source.hh"
#include "interface/xml/destination.hh"
#include "multiplexing/publisher.hh"
#include "multiplexing/subscriber.hh"
#include "processing/feeder.hh"
#include "processing/listener.hh"

using namespace processing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  listener is not copyable. Therefore any attempt to use the copy
 *  constructor will result in a call to abort().
 *
 *  @param[in] l Unused.
 */
listener::listener(listener const& l) : concurrency::thread() {
  (void)l;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  listener is not copyable. Therefore any attempt to use the
 *  assignment operator will result in a call to abort().
 *
 *  @param[in] l Unused.
 *
 *  @return This object.
 */
listener& listener::operator=(listener const& l) {
  (void)l;
  assert(false);
  abort();
  return (*this);
}

/**
 *  Run a thread on an input stream with NDO protocol.
 */
void listener::run_ndo_in(io::stream* is) {
  std::auto_ptr<io::stream> s(is);
  std::auto_ptr<feeder_once> f(new feeder_once);
  std::auto_ptr<multiplexing::publisher> publishr(
    new multiplexing::publisher);
  std::auto_ptr<interface::ndo::source> source(
    new interface::ndo::source(s.get()));
  s.release();
  f->run(source.get(),
    publishr.get(),
    _listener);
  source.release();
  publishr.release();
  f.release();
  return ;
}

/**
 *  Run a thread on an output stream with NDO protocol.
 */
void listener::run_ndo_out(io::stream* is) {
  std::auto_ptr<io::stream> s(is);
  std::auto_ptr<feeder_once> f(new feeder_once);
  std::auto_ptr<multiplexing::subscriber> subscribr(
    new multiplexing::subscriber);
  std::auto_ptr<interface::ndo::destination> destination(
    new interface::ndo::destination(s.get()));
  s.release();
  f->run(subscribr.get(),
    destination.get(),
    _listener);
  subscribr.release();
  destination.release();
  f.release();
  return ;
}

/**
 *  Run a thread on an output stream with XML protocol.
 */
void listener::run_xml_out(io::stream* is) {
  std::auto_ptr<io::stream> s(is);
  std::auto_ptr<feeder_once> f(new feeder_once);
  std::auto_ptr<multiplexing::subscriber> subscribr(
    new multiplexing::subscriber);
  std::auto_ptr<interface::xml::destination> destination(
    new interface::xml::destination(s.get()));
  s.release();
  f->run(subscribr.get(),
    destination.get(),
    _listener);
  subscribr.release();
  destination.release();
  f.release();
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
listener::listener() : _init(false) {}

/**
 *  Destructor.
 */
listener::~listener() {
  try {
    concurrency::lock l(_initm);
    if (_init) {
      _init = false;
      if (_acceptor.get())
        _acceptor->close();
      join();
    }
  }
  catch (...) {}
}

/**
 *  @brief Overload of the parenthesis operator.
 *
 *  This method is used as the entry point of the processing thread
 *  which listens on incoming connections.
 */
void listener::operator()() {
  try {
    // Wait for initial connection.
    std::auto_ptr<io::stream> s(_acceptor->accept());
    while (s.get()) {
      // Process stream.
      (this->*(_run_thread))(s.release());

      // Wait for new connection.
      s.reset(_acceptor->accept());
    }
  }
  catch (...) {}
  try {
    // Mutex already locked == destructor being run.
    if (_initm.try_lock()) {
      _init = false;
      _initm.unlock();
    }
  }
  catch (...) {}
  return ;
}

/**
 *  Ask thread to exit ASAP.
 */
void listener::exit() {
  // Set exit flag.
  thread::exit();

  // Close acceptor.
  if (_acceptor.get())
    _acceptor->close();

  return ;
}

/**
 *  @brief Launch processing thread.
 *
 *  Launch the thread waiting on incoming connections. Upon successful
 *  return from this method, the acceptor will be owned by the listener.
 *
 *  @param[in] acceptor Acceptor on which incoming clients will be awaited.
 *  @param[in] proto    Protocol to use on new connections.
 *  @param[in] io       Should we act on input or output objects ?
 *  @param[in] tl       Listener for all created threads.
 */
void listener::init(io::acceptor* acceptor,
                    listener::protocol proto,
                    listener::INOUT io,
                    concurrency::thread_listener* tl) {
  concurrency::lock l(_initm);
  try {
    _acceptor.reset(acceptor);
    _init = true;
    if ((NDO == proto)) {
      if (OUT == io)
	_run_thread = &processing::listener::run_ndo_out;
      else
	_run_thread = &processing::listener::run_ndo_in;
    }
    else {
      if (OUT == io)
	_run_thread = &processing::listener::run_xml_out;
      /*
	else
	this->run_thread_ = &Processing::Listener::RunXMLIn;*/
    }
    run(tl);
  }
  catch (...) {
    _acceptor.release();
    _init = false;
    throw ;
  }
  return ;
}
