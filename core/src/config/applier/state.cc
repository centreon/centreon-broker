/*
** Copyright 2011-2012 Merethis
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

#include <cassert>
#include <cstdlib>
#include <memory>
#include <QCoreApplication>
#include <QMutex>
#include <QMutexLocker>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker::config::applier;

// Class instance.
static std::auto_ptr<state> gl_state;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destructor.
 */
state::~state() {}

/**
 *  Apply a configuration state.
 *
 *  @param[in] s       State to apply.
 *  @param[in] run_mux Set to true if multiplexing must be run.
 */
void state::apply(com::centreon::broker::config::state const& s,
                  bool run_mux) {
  // Apply logging configuration.
  logger::instance().apply(s.loggers());

  // Flush logs or not.
  com::centreon::broker::logging::file::with_flush(
    s.flush_logs());

  // Enable or not thread ID logging.
  com::centreon::broker::logging::file::with_thread_id(
    s.log_thread_id());

  // Apply modules configuration.
  modules::instance().apply(s.module_directory(), &s);

  // Apply input and output configuration.
  endpoint::instance().apply(s.inputs(), s.outputs());

  // Enable multiplexing loop.
  if (run_mux)
    com::centreon::broker::multiplexing::engine::instance().start();

  return ;
}

/**
 *  Get the instance of this object.
 *
 *  @return Class instance.
 */
state& state::instance() {
  return (*gl_state);
}

/**
 *  Load singleton.
 */
void state::load() {
  if (!gl_state.get())
    gl_state.reset(new state);
  return ;
}

/**
 *  Unload singleton.
 */
void state::unload() {
  gl_state.reset();
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
state::state() {}

/**
 *  @brief Copy constructor.
 *
 *  Any call to this constructor will result in a call to abort().
 *
 *  @param[in] s Object to copy.
 */
state::state(state const& s) {
  (void)s;
  _internal_copy(s);
}

/**
 *  @brief Assignment operator.
 *
 *  Any call to this method will result in a call to abort().
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
state& state::operator=(state const& s) {
  (void)s;
  _internal_copy(s);
  return (*this);
}

/**
 *  Calls abort().
 *
 *  @param[in] s Unused.
 */
void state::_internal_copy(state const& s) {
  (void)s;
  assert(!"state configuration applier is not copyable");
  abort();
  return ;
}
