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
**
** For more information: contact@centreon.com
*/

#ifndef CCB_SQL_INTERNAL_HH_
# define CCB_SQL_INTERNAL_HH_

# include <QSqlQuery>
# include <list>
# include <string>
# include <utility>
# include "events/events.hh"
# include "mapping.hh"

namespace                       com {
  namespace                     centreon {
    namespace                   broker {
      namespace                 sql {
        template                <typename T>
        struct                  getter_setter {
          data_member<T> const* member;
          void                  (* getter)(T const&,
                                  std::string const&,
                                  data_member<T> const&,
                                  QSqlQuery&);
        };

        // DB mappings.
        template               <typename T>
        struct                 db_mapped_type {
          static std::list<std::pair<std::string, getter_setter<T> > > list;
        };

        // Mapping initialization routine.
        void initialize();
      }
    }
  }
}

// ORM operators.
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::acknowledgement const& a);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::comment const& c);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::custom_variable const& cv);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::custom_variable_status const& cvs);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::downtime const& d);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::event_handler const& eh);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::flapping_status const& fs);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::host const& h);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::host_check const& hc);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::host_dependency const& hd);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::host_group const& hg);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::host_group_member const& hgm);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::host_parent const& hp);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::host_state const& hs);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::host_status const& hs);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::instance const& p);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::instance_status const& ps);
//QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::issue const& i);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::log_entry const& le);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::module const& m);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::notification const& n);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::service const& s);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::service_check const& sc);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::service_dependency const& sd);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::service_group const& sg);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::service_group_member const& sgm);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::service_state const& ss);
QSqlQuery& operator<<(QSqlQuery& q, com::centreon::broker::events::service_status const& ss);

#endif /* !CCB_SQL_INTERNAL_HH_ */
