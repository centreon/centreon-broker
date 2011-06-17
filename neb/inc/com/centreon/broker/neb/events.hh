/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_NEB_EVENTS_HH_
# define CCB_NEB_EVENTS_HH_

# include "com/centreon/broker/neb/acknowledgement.hh"
# include "com/centreon/broker/neb/comment.hh"
# include "com/centreon/broker/neb/custom_variable.hh"
# include "com/centreon/broker/neb/custom_variable_status.hh"
# include "com/centreon/broker/neb/downtime.hh"
# include "com/centreon/broker/neb/event_handler.hh"
# include "com/centreon/broker/neb/flapping_status.hh"
# include "com/centreon/broker/neb/host.hh"
# include "com/centreon/broker/neb/host_check.hh"
# include "com/centreon/broker/neb/host_dependency.hh"
# include "com/centreon/broker/neb/host_group.hh"
# include "com/centreon/broker/neb/host_group_member.hh"
# include "com/centreon/broker/neb/host_parent.hh"
// XXX # include "com/centreon/broker/neb/host_state.hh"
# include "com/centreon/broker/neb/host_status.hh"
# include "com/centreon/broker/neb/instance.hh"
# include "com/centreon/broker/neb/instance_status.hh"
// XXX # include "com/centreon/broker/neb/issue.hh"
// XXX # include "com/centreon/broker/neb/issue_parent.hh"
# include "com/centreon/broker/neb/log_entry.hh"
# include "com/centreon/broker/neb/module.hh"
# include "com/centreon/broker/neb/notification.hh"
// XXX # include "com/centreon/broker/neb/perfdata.hh"
# include "com/centreon/broker/neb/service.hh"
# include "com/centreon/broker/neb/service_check.hh"
# include "com/centreon/broker/neb/service_dependency.hh"
# include "com/centreon/broker/neb/service_group.hh"
# include "com/centreon/broker/neb/service_group_member.hh"
// XXX # include "com/centreon/broker/neb/service_state.hh"
# include "com/centreon/broker/neb/service_status.hh"
// XXX # include "com/centreon/broker/neb/status_data.hh"

#endif /* CCB_NEB_EVENTS_HH_ */
