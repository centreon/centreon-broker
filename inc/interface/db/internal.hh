/*
** Copyright 2009-2011 MERETHIS
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

#ifndef INTERFACE_DB_INTERNAL_HH_
# define INTERFACE_DB_INTERNAL_HH_

# include <QSqlQuery>
# include <list>
# include <string>
# include <utility>
# include "events/events.hh"
# include "mapping.hh"

namespace                   interface {
  namespace                 db {
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

// ORM operators.
QSqlQuery& operator<<(QSqlQuery& q, events::acknowledgement const& a);
QSqlQuery& operator<<(QSqlQuery& q, events::comment const& c);
QSqlQuery& operator<<(QSqlQuery& q, events::custom_variable const& cv);
QSqlQuery& operator<<(QSqlQuery& q, events::custom_variable_status const& cvs);
QSqlQuery& operator<<(QSqlQuery& q, events::downtime const& d);
QSqlQuery& operator<<(QSqlQuery& q, events::event_handler const& eh);
QSqlQuery& operator<<(QSqlQuery& q, events::flapping_status const& fs);
QSqlQuery& operator<<(QSqlQuery& q, events::host const& h);
QSqlQuery& operator<<(QSqlQuery& q, events::host_check const& hc);
QSqlQuery& operator<<(QSqlQuery& q, events::host_dependency const& hd);
QSqlQuery& operator<<(QSqlQuery& q, events::host_group const& hg);
QSqlQuery& operator<<(QSqlQuery& q, events::host_group_member const& hgm);
QSqlQuery& operator<<(QSqlQuery& q, events::host_parent const& hp);
QSqlQuery& operator<<(QSqlQuery& q, events::host_status const& hs);
QSqlQuery& operator<<(QSqlQuery& q, events::instance const& p);
QSqlQuery& operator<<(QSqlQuery& q, events::instance_status const& ps);
QSqlQuery& operator<<(QSqlQuery& q, events::issue const& i);
QSqlQuery& operator<<(QSqlQuery& q, events::log_entry const& le);
QSqlQuery& operator<<(QSqlQuery& q, events::module const& m);
QSqlQuery& operator<<(QSqlQuery& q, events::notification const& n);
QSqlQuery& operator<<(QSqlQuery& q, events::service const& s);
QSqlQuery& operator<<(QSqlQuery& q, events::service_check const& sc);
QSqlQuery& operator<<(QSqlQuery& q, events::service_dependency const& sd);
QSqlQuery& operator<<(QSqlQuery& q, events::service_group const& sg);
QSqlQuery& operator<<(QSqlQuery& q, events::service_group_member const& sgm);
QSqlQuery& operator<<(QSqlQuery& q, events::service_status const& ss);
QSqlQuery& operator<<(QSqlQuery& q, events::state const& s);

#endif /* !INTERFACE_DB_INTERNAL_HH_ */
