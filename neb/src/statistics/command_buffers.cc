/*
** Copyright 2013 Centreon
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

#include <pthread.h>
#include <sstream>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/statistics/command_buffers.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;

/**
 *  Default constructor.
 */
command_buffers::command_buffers()
  : plugin("command_buffers") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
command_buffers::command_buffers(command_buffers const& right)
 : plugin(right) {}

/**
 *  Destructor.
 */
command_buffers::~command_buffers() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
command_buffers& command_buffers::operator=(command_buffers const& right) {
  plugin::operator=(right);
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void command_buffers::run(std::string& output __attribute__((unused)),
                          std::string& perfdata __attribute__((unused))) {
  // XXX
  // unsigned int high(0);
  // unsigned int total(::external_command_buffer_slots);
  // unsigned int used(0);

  // // get number of items in the command buffer
  // if (::check_external_commands) {
  //   pthread_mutex_lock(&external_command_buffer.buffer_lock);
  //   used = external_command_buffer.items;
  //   high = external_command_buffer.high;
  //   pthread_mutex_unlock(&external_command_buffer.buffer_lock);
  // }

  // // Output.
  // std::ostringstream oss;
  // oss << "Engine " << config::applier::state::instance().poller_name()
  //     << " has " << used << "/" << total << " command";
  // output = oss.str();

  // // Perfdata.
  // oss.str("");
  // oss << "used=" << used << " high=" << high << " total=" << total;
  // perfdata = oss.str();

  return ;
}
