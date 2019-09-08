/*
** Copyright 2011-2014 Centreon
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

#include "com/centreon/broker/notification/builders/command_by_id_builder.hh"
#include "com/centreon/broker/notification/utilities/qhash_func.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Construct the object.
 *
 *  @param[in,out] table  The table to fill.
 */
command_by_id_builder::command_by_id_builder(
    QHash<unsigned int, command::ptr>& table)
    : _table(table) {}

/**
 *  Add a command to the builder.
 *
 *  @param[in] id The id of the command.
 *  @param[in] com The command.
 */
void command_by_id_builder::add_command(unsigned int id, command::ptr com) {
  _table[id] = com;
}
