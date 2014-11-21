/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_NOTIFICATION_COMMAND_HH
#  define CCB_NOTIFICATION_COMMAND_HH

#  include <string>
#  include "com/centreon/broker/neb/host_status.hh"
#  include "com/centreon/broker/neb/service_status.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/utilities/ptr_typedef.hh"
#  include "com/centreon/broker/notification/node_cache.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/objects/notification_rule.hh"
#  include "com/centreon/broker/notification/objects/timeperiod.hh"
#  include "com/centreon/broker/notification/objects/notification_method.hh"
#  include "com/centreon/broker/notification/objects/contact.hh"
#  include "com/centreon/broker/notification/objects/node.hh"

CCB_BEGIN()

namespace               notification {
  namespace             objects {
    /**
     *  @class command command.hh "com/centreon/broker/notification/objects/command.hh"
     *  @brief Command object.
     *
     *  The object containing a command.
     */
    class                 command {
    public:
                          DECLARE_SHARED_PTR(command);

                          command(std::string const& base_command);
                          command(command const& obj);
                          command& operator=(command const& obj);

      std::string const&  get_name() const throw();
      void                set_name(std::string const& name);

      std::string         resolve(
                            notification_rule::ptr const& rule,
                            timeperiod::ptr const& tp,
                            notification_method::ptr const& method,
                            contact::ptr const& contact,
                            node::ptr const& n,
                            node_cache const& cache);

    private:
      command();

      std::string         _name;
      std::string         _base_command;

      //static              _macro_init();
    };
  }
}

CCB_END()

#endif // !CCB_NOTIFICATION_COMMAND_HH
