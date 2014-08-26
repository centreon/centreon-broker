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

#ifndef CCB_NOTIFICATION_BUILDERS_COMPOSED_ESCALATION_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_COMPOSED_ESCALATION_BUILDER_HH

#  include <vector>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/escalation.hh"
#  include "com/centreon/broker/notification/builders/escalation_builder.hh"
#  include "com/centreon/broker/notification/builders/composed_builder.hh"

CCB_BEGIN()

namespace       notification {

  class           composed_escalation_builder : public composed_builder<escalation_builder> {
  public:
                  composed_escalation_builder();
    virtual       ~composed_escalation_builder() {}

    virtual void  add_escalation(unsigned int id,
                                 shared_ptr<escalation> esc);
    virtual void connect_escalation_node_id(unsigned int esc_id,
                                            node_id id);
    virtual void connect_escalation_contactgroup(unsigned int id,
                                                 unsigned int contactgroup_id);
    virtual void connect_escalation_hostgroup(unsigned int id,
                                              unsigned int hostgroup_id);
    virtual void connect_escalation_servicegroup(unsigned int id,
                                                 unsigned int servicegroup_id);
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_COMPOSED_ESCALATION_BUILDER_HH
