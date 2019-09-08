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

#ifndef CCB_NOTIFICATION_BUILDERS_COMMAND_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_COMMAND_BUILDER_HH

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/objects/command.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class command_builder command_builder.hh
 * "com/centreon/broker/notification/builders/command_builder.hh"
 *  @brief Command builder interface.
 *
 *  This interface define what methods command builders need to implement.
 */
class command_builder {
 public:
  /**
   *  Virtual destructor.
   */
  virtual ~command_builder() {}

  /**
   *  Add a command to the builder.
   *
   *  @param[in] id The id of the command.
   *  @param[in] com The command.
   */
  virtual void add_command(unsigned int id, objects::command::ptr com) {
    (void)id;
    (void)com;
  }
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_COMMAND_BUILDER_HH
