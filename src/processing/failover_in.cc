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
#include <stdlib.h>
#include <unistd.h>
#include "concurrency/lock.hh"
#include "config/factory.hh"
#include "config/interface.hh"
#include "logging/logging.hh"
#include "processing/failover_in.hh"

using namespace processing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  As failover_in is not copyable, any attempt to use the copy
 *  constructor will result in a call to abort().
 *
 *  @param[in] fi Unused.
 */
failover_in::failover_in(failover_in const& fi)
  : interface::source(),
    interface::destination(),
    feeder(),
    interface::source_destination() {
  (void)fi;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  As failover_in is not copyable, any attempt to use the assignment
 *  operator will result in a call to abort().
 *
 *  @param[in] fi Unused.
 *
 *  @return This object.
 */
failover_in& failover_in::operator=(failover_in const& fi) {
  (void)fi;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
failover_in::failover_in() : _reconnect_interval(30) {}

/**
 *  Destructor.
 */
failover_in::~failover_in() {
  try {
    this->exit();
    join();
  }
  catch (...) {}
}

/**
 *  FailoverIn thread entry point.
 */
void failover_in::operator()() {
  while (!_should_exit) {
    try {
      logging::info << logging::MEDIUM << "connecting input";
      connect();
      logging::info << logging::MEDIUM << "input connection successful";
      feed(this, this);
    }
    catch (std::exception const& e) {
      logging::error << logging::HIGH
                     << "error while processing input: " << e.what();
      logging::info << logging::MEDIUM
                    << "waiting "
                    << static_cast<unsigned int>(_reconnect_interval)
                    << " seconds before attempting reconnection";
      sleep(_reconnect_interval);
    }
    catch (...) {
      logging::error << logging::HIGH
                     << "input processing thread caught an unknown exception";
      logging::info << logging::HIGH
                    << "input processing thread will now exit";
      break ;
    }
  }
  return ;
}

/**
 *  Close.
 */
void failover_in::close() {
  // Close source.
  if (_source.get())
    _source->close();

  return ;
}

/**
 *  Connect source stream.
 */
void failover_in::connect() {
  concurrency::lock l(_sourcem);

  // Close before reopening.
  _source.reset();
  _source.reset(config::factory::build_source(*_source_conf));
  return ;
}

/**
 *  Get next available event.
 */
events::event* failover_in::event() {
  concurrency::lock l(_sourcem);
  return (_source->event());
}

/**
 *  Store event.
 *
 *  @param[in] e Event to store.
 */
void failover_in::event(events::event* e) {
  concurrency::lock l(_destm);
  _dest->event(e);
  return ;
}

/**
 *  Tell the thread that processing should stop.
 */
void failover_in::exit() {
  // Set exit flag.
  thread::exit();

  // Close.
  this->close();

  return ;
}

/**
 *  Set the reconnection interval.
 */
void failover_in::reconnect_interval(time_t ri) {
  _reconnect_interval = ri;
  return ;
}

/**
 *  Run failover thread.
 */
void failover_in::run(config::interface const& source_conf,
                      interface::destination* dest,
                      concurrency::thread_listener* tl) {
  {
    concurrency::lock l(_sourcem);
    _source_conf.reset(new config::interface(source_conf));
  }
  {
    concurrency::lock l(_destm);
    _dest.reset(dest);
  }
  reconnect_interval(source_conf.reconnect_interval);
  concurrency::thread::run(tl);
  return ;
}
