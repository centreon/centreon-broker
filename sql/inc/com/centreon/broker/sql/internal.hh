/*
** Copyright 2009-2013 Merethis
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

#  include <QSqlQuery>
#  include <string>
#  include <utility>
#  include <vector>
#  include "com/centreon/broker/correlation/events.hh"
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
                            QSqlQuery&);
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
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::acknowledgement const& a);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::comment const& c);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::custom_variable const& cv);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::custom_variable_status const& cvs);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::downtime const& d);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::event_handler const& eh);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::flapping_status const& fs);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::host const& h);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::host_check const& hc);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::host_dependency const& hd);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::host_group const& hg);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::host_group_member const& hgm);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::host_parent const& hp);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::host_status const& hs);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::instance const& p);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::instance_status const& ps);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::log_entry const& le);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::module const& m);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::notification const& n);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::service const& s);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::service_check const& sc);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::service_dependency const& sd);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::service_group const& sg);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::service_group_member const& sgm);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::neb::service_status const& ss);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::correlation::host_state const& hs);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::correlation::issue const& i);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::correlation::service_state const& ss);

#endif // !CCB_SQL_INTERNAL_HH
