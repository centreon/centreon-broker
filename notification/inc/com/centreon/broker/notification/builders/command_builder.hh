/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_NOTIFICATION_BUILDERS_COMMAND_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_COMMAND_BUILDER_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/command.hh"

CCB_BEGIN()

namespace         notification {
  /**
   *  @class command_builder command_builder.hh "com/centreon/broker/notification/builders/command_builder.hh"
   *  @brief Command builder interface.
   *
   *  This interface define what methods command builders need to implement.
   */
  class           command_builder {
  public:
    /**
     *  Virtual destructor.
     */
    virtual       ~command_builder() {}

    /**
     *  Add a command to the builder.
     *
     *  @param[in] id The id of the command.
     *  @param[in] com The command.
     */
    virtual void  add_command(
                    unsigned int id,
                    objects::command::ptr com) {}
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_COMMAND_BUILDER_HH
