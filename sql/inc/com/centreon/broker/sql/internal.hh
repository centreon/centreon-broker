/*
** Copyright 2009-2014 Centreon
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

#ifndef CCB_SQL_INTERNAL_HH
#  define CCB_SQL_INTERNAL_HH

#  include <string>
#  include <utility>
#  include <vector>
#  include "com/centreon/broker/correlation/events.hh"
#  include "com/centreon/broker/database_query.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/events.hh"
#  include "mapping.hh"

CCB_BEGIN()

namespace                 sql {
  template                <typename T>
  struct                  getter_setter {
    data_member<T> const* member;
    void (*               getter)(
                            T const&,
                            QString const&,
                            data_member<T> const&,
                            database_query&);
  };

  // DB mappings.
  template               <typename T>
  struct                 db_mapped_entry {
    QString              field;
    getter_setter<T>     gs;
    QString              name;
  };
  template               <typename T>
  struct                 db_mapped_type {
    static std::vector<db_mapped_entry<T> > list;
  };

  // Mapping initialization routine.
  void                   initialize();
}

CCB_END()

// ORM operators.
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::acknowledgement const& a);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::comment const& c);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::custom_variable const& cv);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::custom_variable_status const& cvs);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::downtime const& d);
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
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::notification const& n);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::service const& s);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::service_check const& sc);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::service_dependency const& sd);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::service_group const& sg);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::service_group_member const& sgm);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::neb::service_status const& ss);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::correlation::host_state const& hs);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::correlation::issue const& i);
com::centreon::broker::database_query& operator<<(com::centreon::broker::database_query& q, com::centreon::broker::correlation::service_state const& ss);

#endif // !CCB_SQL_INTERNAL_HH
