/*
** Copyright 2013 Merethis
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

#include <pthread.h>
#include <sstream>
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/statistics/command_buffers.hh"
#include "com/centreon/engine/globals.hh"

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
 : plugin(right) {

}

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
void command_buffers::run(
              std::string& output,
	      std::string& perfdata) {
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
  // oss << "Engine " << instance_name.toStdString()
  //     << " has " << used << "/" << total << " command";
  // output = oss.str();

  // // Perfdata.
  // oss.str("");
  // oss << "used=" << used << " high=" << high << " total=" << total;
  // perfdata = oss.str();

  return ;
}
