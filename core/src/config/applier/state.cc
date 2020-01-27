/*
** Copyright 2011-2013,2015-2016 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <cassert>
#include <cstdio>
#include "com/centreon/broker/config/applier/state.hh"
#include <cstdlib>
#include <cstring>
#include <memory>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/instance_broadcast.hh"
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/vars.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::config::applier;

// Class instance.
static state* gl_state = nullptr;

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
void state::apply(com::centreon::broker::config::state const& s, bool run_mux) {
  // Sanity checks.
  static char const* const allowed_chars(
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 -_");
  if (!s.poller_id() || s.poller_name().empty())
    throw(exceptions::msg()
          << "state applier: poller information are "
          << "not set: please fill poller_id and poller_name");
  if (!s.broker_id() || s.broker_name().empty())
    throw(exceptions::msg()
          << "state applier: instance information "
          << "are not set: please fill broker_id and broker_name");
  for (std::string::const_iterator it(s.broker_name().begin()),
       end(s.broker_name().end());
       it != end; ++it)
    if (!strchr(allowed_chars, *it))
      throw(exceptions::msg()
            << "state applier: broker_name is not "
            << " valid: allowed characters are " << allowed_chars);
  for (std::list<config::endpoint>::const_iterator it(s.endpoints().begin()),
       end(s.endpoints().end());
       it != end; ++it) {
    if (it->name.empty())
      throw(exceptions::msg() << "state applier: endpoint name is not set: "
                              << "please fill name of all endpoints");
    for (std::string::const_iterator it_name(it->name.begin()),
         end_name(it->name.end());
         it_name != end_name; ++it_name)
      if (!strchr(allowed_chars, *it_name))
        throw(exceptions::msg()
              << "state applier: endpoint name '" << *it_name
              << "' is not valid: allowed characters are " << allowed_chars);
  }

  // Set Broker instance ID.
  io::data::broker_id = s.broker_id();

  // Set poller instance.
  _poller_id = s.poller_id();
  _poller_name = s.poller_name();

  // Set cache directory.
  _cache_dir = s.cache_directory();
  if (_cache_dir.empty())
    _cache_dir.append(PREFIX_VAR);
  _cache_dir.append("/");
  _cache_dir.append(s.broker_name());

  // Apply logging configuration.
  logger::instance().apply(s.loggers());

  // Flush logs or not.
  com::centreon::broker::logging::file::with_flush(s.flush_logs());

  // Enable or not thread ID logging.
  com::centreon::broker::logging::file::with_thread_id(s.log_thread_id());

  // Enable or not timestamp logging.
  com::centreon::broker::logging::file::with_timestamp(s.log_timestamp());

  // Enable or not human readable timstamp logging.
  com::centreon::broker::logging::file::with_human_redable_timestamp(
      s.log_human_readable_timestamp());

  // Apply modules configuration.
  modules::instance().apply(s.module_list(), s.module_directory(), &s);
  static bool first_application(true);
  if (first_application)
    first_application = false;
  else {
    uint32_t module_count(0);
    for (modules::iterator it(modules::instance().begin()),
         end(modules::instance().end());
         it != end; ++it)
      ++module_count;
    if (module_count)
      logging::config(logging::high)
          << "applier: " << module_count << " modules loaded";
    else
      logging::config(logging::high)
          << "applier: no module loaded, "
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
    ept.params.insert({"extcmd", s.command_file()});
    ept.params.insert({"command_protocol", s.command_protocol()});
    ept.read_filters.insert("all");
    st.endpoints().push_back(ept);
  }

  // Apply input and output configuration.
  endpoint::instance().apply(st.endpoints());

  // Create instance broadcast event.
  std::shared_ptr<instance_broadcast> ib(new instance_broadcast);
  ib->broker_id = io::data::broker_id;
  ib->poller_id = _poller_id;
  ib->poller_name = _poller_name;
  ib->enabled = true;
  com::centreon::broker::multiplexing::engine::instance().publish(ib);

  // Enable multiplexing loop.
  if (run_mux)
    com::centreon::broker::multiplexing::engine::instance().start();
}

/**
 *  Get applied cache directory.
 *
 *  @return Cache directory.
 */
std::string const& state::cache_dir() const throw() {
  return _cache_dir;
}

/**
 *  Get the instance of this object.
 *
 *  @return Class instance.
 */
state& state::instance() {
  return *gl_state;
}

/**
 *  Load singleton.
 */
void state::load() {
  if (!gl_state)
    gl_state = new state;
}

/**
 *  Get the poller ID.
 *
 *  @return Poller ID of this Broker instance.
 */
uint32_t state::poller_id() const throw() {
  return _poller_id;
}

/**
 *  Get the poller name.
 *
 *  @return Poller name of this Broker instance.
 */
std::string const& state::poller_name() const throw() {
  return _poller_name;
}

/**
 *  Unload singleton.
 */
void state::unload() {
  delete gl_state;
  gl_state = nullptr;
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
