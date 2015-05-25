/*
** Copyright 2015 Merethis
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
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/processing/input.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

/**
 *  Constructor.
 *
 *  @param[in] endp  Endpoint.
 *  @param[in] name  Name of this endpoint.
 */
input::input(
         misc::shared_ptr<io::endpoint> endp,
         std::string const& name)
  : _endp(endp),
    _name(name),
    _retry_interval(30) {}

/**
 *  Destructor.
 */
input::~input() {}

/**
 *  Request thread to exit.
 */
void input::exit() {
  thread::exit();
  _endp->close();
  return ;
}

/**
 *  @brief Thread entry point.
 *
 *  Read from the input endpoint and publish it.
 */
void input::run() {
  // Start message.
  logging::info(logging::medium)
    << "input: thread of input endpoint '" << _name << "' is starting";

  // Thread should run as long as no exit request was made.
  while (!should_exit()) {
    try {
      // Open endpoint.
      misc::shared_ptr<io::stream> s(_endp->open());
      if (!s.isNull()) {
        // Read/write loop.
        multiplexing::publisher p;
        while (!should_exit()) {
          misc::shared_ptr<io::data> d;
          s->read(d);
          p.write(d);
        }
      }
    }
    catch (std::exception const& e) {
      // Log error.
      logging::info(logging::medium)
        << "input: input endpoint '" << _name
        << "' encountered an error: " << e.what();

      // Wait a while before attempting reconnection.
      logging::info(logging::medium)
        << "input: input endpoint '" << _name
        << "' will wait " << _retry_interval
        << "s before attempting reconnection";
      // XXX
    }
  }

  // Stop message.
  logging::info(logging::medium)
    << "input: thread of input endpoint '" << _name << "' is stopping";

  return ;
}

/**
 *  Set retry interval.
 *
 *  @param[in] retry_interval  New retry interval.
 */
void input::set_retry_interval(time_t retry_interval) {
  _retry_interval = retry_interval;
  return ;
}
