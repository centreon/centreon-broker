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

#ifndef CCB_NOTIFICATION_NOTIFICATION_RULE_HH
#  define CCB_NOTIFICATION_NOTIFICATION_RULE_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/utilities/ptr_typedef.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/objects/defines.hh"

CCB_BEGIN()

namespace         notification {
  namespace       objects {
    /**
     *  @class notification_rule notification_rule.hh "com/centreon/broker/notification/objects/notification_rule.hh"
     *  @brief The notification rule object.
     *
     */
    class           notification_rule {
    public:
                    DECLARE_SHARED_PTR(notification_rule);

                    notification_rule();
                    notification_rule(notification_rule const& obj);
      notification_rule&
                    operator=(notification_rule const& obj);

      unsigned int  get_id();
      void          set_id(unsigned int id);

      unsigned int  get_method_id() const throw();
      void          set_method_id(unsigned int) throw();

      unsigned int  get_timeperiod_id() const throw();
      void          set_timeperiod_id(unsigned int val) throw();

      unsigned int  get_contact_id() const throw();
      void          set_contact_id(unsigned int val) throw();

      node_id       get_node_id() const throw();
      void          set_node_id(node_id val) throw();

      bool          should_be_notified_for(node_state nst) const throw();

    private:
      unsigned int  _id;
      unsigned int  _method_id;
      unsigned int  _timeperiod_id;
      unsigned int  _contact_id;
      node_id       _node_id;
    };
  }
}

CCB_END()

#endif // !CCB_NOTIFICATION_NOTIFICATION_RULE_HH
