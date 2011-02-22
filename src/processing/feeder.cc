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
#include "concurrency/lock.hh"
#include "events/event.hh"
#include "interface/destination.hh"
#include "interface/source.hh"
#include "logging/logging.hh"
#include "processing/feeder.hh"

using namespace processing;


/******************************************************************************
*                                                                             *
*                                                                             *
*                                  feeder                                     *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  feeder is not copyable. Any attempt to use the copy constructor will
 *  result in a call to abort().
 *
 *  @param[in] f Unused.
 */
feeder::feeder(feeder const& f) : concurrency::thread() {
  (void)f;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  feeder is not copyable. Any attempt to use the assignment operator
 *  will result in a call to abort().
 *
 *  @param[in] f Unused.
 *
 *  @return This object.
 */
feeder& feeder::operator=(feeder const& f) {
  (void)f;
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
feeder::feeder() {}

/**
 *  Destructor.
 */
feeder::~feeder() {
  try {
    this->exit();
    join();
  }
  catch (...) {}
}

/**
 *  Send events from the source to the destination.
 */
void feeder::feed(interface::source* source,
                  interface::destination* dest) {
  events::event* event = NULL;
  try {
    // Fetch first event.
    logging::debug << logging::LOW << "feeder fetch event";
    event = source->event();
    while (!_should_exit || event) {
      // Print output messages.
      if (_should_exit) {
        unsigned int n(source->size());
        if (!(n % 1000)) {
          logging::info << logging::HIGH << n
                        << " events to process";
        }
      }
      // Send event.
      logging::debug << logging::LOW << "feeder send event";
      dest->event(event);
      event = NULL;
      // Fetch next event.
      logging::debug << logging::LOW << "feeder fetch event";
      event = source->event();
    }
    logging::debug << logging::MEDIUM
                   << "exiting feeding loop without error";
  }
  catch (...) {
    logging::debug << logging::LOW
                   << "rethrowing exception from feeder";
    if (event)
      event->remove_reader();
    throw ;
  }

  // In case we read an event but have to exit.
  if (event)
    event->remove_reader();

  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                               feeder_once                                   *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  feeder_once is not copyable ; therefore any attempt to use the copy
 *  constructor will result in a call to abort().
 *
 *  @param[in] fo Unused.
 */
feeder_once::feeder_once(feeder_once const& fo)
  : interface::source(),
    interface::destination(),
    processing::feeder(),
    interface::source_destination() {
  (void)fo;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  feeder_once is not copyable ; therefore any attempt to use the
 *  assignment operator will result in a call to abort().
 *
 *  @param[in] fo Unused.
 *
 *  @return This object.
 */
feeder_once& feeder_once::operator=(feeder_once const& fo) {
  (void)fo;
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
feeder_once::feeder_once() {}

/**
 *  Destructor.
 */
feeder_once::~feeder_once() {
  try {
    this->exit();
    join();
  }
  catch (...) {}
}

/**
 *  Feeder thread entry point.
 */
void feeder_once::operator()() {
  feed(this, this);
  return ;
}

/**
 *  Close.
 */
void feeder_once::close() {
  // Close source.
  if (_source.get())
    _source->close();

  return ;
}

/**
 *  Get next available event.
 */
events::event* feeder_once::event() {
  concurrency::lock l(_sourcem);
  return (_source->event());
}

/**
 *  Store event.
 *
 *  @param[in] e Event to store.
 */
void feeder_once::event(events::event* e) {
  concurrency::lock l(_destm);
  _dest->event(e);
  return ;
}

/**
 *  Tell the thread that processing should stop.
 */
void feeder_once::exit() {
  // Set exit flag.
  thread::exit();

  // Close.
  this->close();

  return ;
}

/**
 *  Run feeder thread.
 */
void feeder_once::run(interface::source* source,
                      interface::destination* dest,
                      concurrency::thread_listener* tl) {
  try {
    {
      concurrency::lock l(_destm);
      _dest.reset(dest);
    }
    {
      concurrency::lock l(_sourcem);
      _source.reset(source);
    }
    concurrency::thread::run(tl);
  }
  catch (...) {
    _dest.reset();
    _source.reset();
    throw ;
  }
  return ;
}
