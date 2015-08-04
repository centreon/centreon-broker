/*
** Copyright 2011-2012,2015 Merethis
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

#include <cstdlib>
#include <memory>
#include <QCoreApplication>
#include <QMutex>
#include <QMutexLocker>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/instance_broadcast.hh"

using namespace com::centreon::broker::config::applier;

// Class instance.
static state* gl_state = NULL;

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
void state::apply(
              com::centreon::broker::config::state const& s,
              bool run_mux) {
  // Set Broker instance ID.
  io::data::broker_id = s.broker_id();

  // Set poller instance.
  _poller_id = s.poller_id();
  _poller_name = s.poller_name();

  // Set cache directory.
  _cache_dir = s.cache_directory();
  if (!_cache_dir.empty())
    _cache_dir.append("/");

  // Apply logging configuration.
  logger::instance().apply(s.loggers());

  // Flush logs or not.
  com::centreon::broker::logging::file::with_flush(
    s.flush_logs());

  // Enable or not thread ID logging.
  com::centreon::broker::logging::file::with_thread_id(
    s.log_thread_id());

  // Enable or not timestamp logging.
  com::centreon::broker::logging::file::with_timestamp(
    s.log_timestamp());

  // Enable or not human readable timstamp logging.
  com::centreon::broker::logging::file::with_human_redable_timestamp(
    s.log_human_readable_timestamp());

  // Apply modules configuration.
  modules::instance().apply(
                        s.module_list(),
                        s.module_directory(),
                        &s);
  static bool first_application(true);
  if (first_application)
    first_application = false;
  else {
    unsigned int module_count(0);
    for (modules::iterator
           it(modules::instance().begin()),
           end(modules::instance().end());
         it != end;
         ++it)
      ++module_count;
    if (module_count)
      logging::config(logging::high) << "applier: " << module_count
        << " modules loaded";
    else
      logging::config(logging::high) << "applier: no module loaded, "
        "you might want to check the 'module_directory' directive";
  }

  // Event queue max size (used to limit memory consumption).
  com::centreon::broker::multiplexing::muxer::event_queue_max_size(
    s.event_queue_max_size());

  com::centreon::broker::config::state st = s;

  // Create command file input.
  if (!s.command_file().empty()) {
    config::endpoint ept;
    ept.name = "(external commands)";
    ept.type = "extcmd";
    ept.params.insert(
      "extcmd",
      QString::fromStdString(s.command_file()));
    ept.write_filters.insert("all");
    st.endpoints().push_back(ept);
  }

  // Apply input and output configuration.
  endpoint::instance().apply(
                         st.endpoints(),
                         st.cache_directory());

  // Create instance broadcast event.
  misc::shared_ptr<instance_broadcast> ib(new instance_broadcast);
  ib->broker_id = io::data::broker_id;
  ib->poller_id = _poller_id;
  ib->poller_name = _poller_name.c_str();
  ib->enabled = true;
  com::centreon::broker::multiplexing::engine::instance().publish(ib);

  // Enable multiplexing loop.
  if (run_mux)
    com::centreon::broker::multiplexing::engine::instance().start();

  return ;
}

/**
 *  Get applied cache directory.
 *
 *  @return Cache directory.
 */
std::string const& state::cache_dir() const throw () {
  return (_cache_dir);
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
  if (!gl_state)
    gl_state = new state;
  return ;
}

/**
 *  Get the poller ID.
 *
 *  @return Poller ID of this Broker instance.
 */
unsigned int state::poller_id() const throw () {
  return (_poller_id);
}

/**
 *  Get the poller name.
 *
 *  @return Poller name of this Broker instance.
 */
std::string const& state::poller_name() const throw () {
  return (_poller_name);
}

/**
 *  Unload singleton.
 */
void state::unload() {
  delete gl_state;
  gl_state = NULL;
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
state::state() : _poller_id(0) {}
