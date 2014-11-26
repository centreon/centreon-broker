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

#ifndef CCB_NOTIFICATION_NODEGROUP_HH
#  define CCB_NOTIFICATION_NODEGROUP_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/defines.hh"
#  include "com/centreon/broker/notification/utilities/ptr_typedef.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace        notification {
  namespace      objects {
    /**
     *  @class nodegroup nodegroup.hh "com/centreon/broker/notification/objects/nodegroup.hh"
     *  @brief A nodegroup object.
     *
     *  The object containing a nodegroup of the notification module.
     */
    class     nodegroup {
    public:
              DECLARE_SHARED_PTR(nodegroup);

              nodegroup();
              nodegroup(nodegroup const& obj);
              nodegroup& operator=(nodegroup const& obj);

      std::string const&
              get_name() const;
      void    set_name(std::string const& name);

      std::string const&
              get_alias() const;
      void    set_alias(std::string const& alias);

    private:
      std::string
              _name;
      std::string
              _alias;
    };
  }
}

CCB_END()

#endif // !CCB_NOTIFICATION_NODEGROUP_HH
