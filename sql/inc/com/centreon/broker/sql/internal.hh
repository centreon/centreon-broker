/*
** Copyright 2009-2015 Merethis
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

#ifndef CCB_SQL_INTERNAL_HH
#  define CCB_SQL_INTERNAL_HH

#  include <string>
#  include <utility>
#  include <vector>
#  include "com/centreon/broker/correlation/events.hh"
#  include "com/centreon/broker/database_query.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/events.hh"
#  include "com/centreon/broker/notification/acknowledgement.hh"
#  include "com/centreon/broker/notification/downtime.hh"

// ORM operators.
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::custom_variable const& cv);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::custom_variable_status const& cvs);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::event_handler const& eh);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::flapping_status const& fs);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::host const& h);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::host_check const& hc);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::host_dependency const& hd);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::host_group const& hg);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::host_group_member const& hgm);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::host_parent const& hp);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::host_status const& hs);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::instance const& p);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::instance_status const& ps);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::log_entry const& le);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::module const& m);
// XXX com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::notification const& n);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::service const& s);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::service_check const& sc);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::service_dependency const& sd);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::service_group const& sg);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::service_group_member const& sgm);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::service_status const& ss);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::notification::acknowledgement const& a);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::notification::downtime const& d);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::correlation::host_state const& hs);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::correlation::issue const& i);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::correlation::service_state const& ss);

#endif // !CCB_SQL_INTERNAL_HH
