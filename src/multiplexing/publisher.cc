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

#include <algorithm>
#include <utility>
#include "concurrency/lock.hh"
#include "concurrency/mutex.hh"
#include "config/globals.hh"
#include "correlation/correlator.hh"
#include "events/events.hh"
#include "logging/logging.hh"
#include "multiplexing/internal.hh"
#include "multiplexing/publisher.hh"
#include "multiplexing/subscriber.hh"

using namespace multiplexing;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Correlation engine.
static correlation::correlator gl_correlator;
static concurrency::mutex      gl_correlatorm;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
publisher::publisher() {}

/**
 *  @brief Copy constructor.
 *
 *  As publisher does not hold any data value, this constructor is
 *  similar to the default constructor.
 *
 *  @param[in] p Unused.
 */
publisher::publisher(publisher const& p)
  : interface::destination(p) {}

/**
 *  Destructor.
 */
publisher::~publisher() {}

/**
 *  @brief Assignment operator.
 *
 *  As publisher does not hold any data value, this assignment operator
 *  does nothing.
 *
 *  @param[in] p Unused.
 *
 *  @return This object.
 */
publisher& publisher::operator=(publisher const& p) {
  interface::destination::operator=(p);
  return (*this);
}

/**
 *  @brief Prevent any event to be sent without error.
 *
 *  In theory close() should prevent any event to be sent through the
 *  event(events::event*) method without error. However for performance
 *  purposes, no check is actually performed and therefore this method
 *  does nothing.
 */
void publisher::close() {
  return ;
}

/**
 *  Launch the correlation engine.
 */
void publisher::correlate() {
  concurrency::lock l(gl_correlatorm);
  gl_correlator.load(config::globals::correlation_file.c_str());
  return ;
}

/**
 *  @brief Send an event to all subscribers.
 *
 *  As soon as the method returns, the event object is owned by the
 *  publisher, meaning that it'll be automatically destroyed when
 *  necessary.
 *
 *  @param[in] e Event to publish.
 */
void publisher::event(events::event* e) {

  // Pass object to correlation.
  if (config::globals::correlation) {
    concurrency::lock l(gl_correlatorm);
    gl_correlator.event(*e);
  }

  // Send object to every subscriber.
  {
    concurrency::lock l(gl_subscribersm);
    std::list<subscriber*>::iterator end = gl_subscribers.end();
    for (std::list<subscriber*>::iterator it = gl_subscribers.begin();
         it != end;
         ++it) {
      e->add_reader();
      (*it)->event(e);
    }
  }

  // Get correlated events.
  std::auto_ptr<events::event> correlated;
  {
    concurrency::lock l(gl_correlatorm);
    correlated.reset(gl_correlator.event());
  }

  // Dispatch event recursively.
  if (correlated.get()) {
    correlated->add_reader();
    this->event(correlated.get());
    correlated.release();
  }

  // Self deregistration.
  e->remove_reader();

  return ;
}
