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

#include <QSqlQuery>
#include <QVariant>
#include <assert.h>
#include <stdlib.h>
#include "com/centreon/broker/sql/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Get a boolean from an object.
 */
template <typename T>
static void get_boolean(T const& t,
                        std::string const& name,
                        data_member<T> const& member,
                        QSqlQuery& q) {
  std::string field(":");
  field.append(name);
  q.bindValue(field.c_str(), QVariant(t.*(member.b)));
  return ;
}

/**
 *  Get a double from an object.
 */
template <typename T>
static void get_double(T const& t,
                       std::string const& name,
                       data_member<T> const& member,
                       QSqlQuery& q) {
  std::string field (":");
  field.append(name);
  q.bindValue(field.c_str(), QVariant(t.*(member.d)));
  return ;
}

/**
 *  Get an ID from an object.
 */
template <typename T>
static void get_id(T const& t,
                   std::string const& name,
                   data_member<T> const& member,
                   QSqlQuery& q) {
  std::string field(":");
  field.append(name);
  int val(t.*(member.i));
  // Not-NULL
  if (val)
    q.bindValue(field.c_str(), QVariant(val));
  // NULL
  else
    q.bindValue(field.c_str(), QVariant(QVariant::Int));
  return ;
}

/**
 *  Get an integer from an object.
 */
template <typename T>
static void get_integer(T const& t,
                        std::string const& name,
                        data_member<T> const& member,
                        QSqlQuery& q) {
  std::string field(":");
  field.append(name);
  q.bindValue(field.c_str(), QVariant(t.*(member.i)));
  return ;
}

/**
 *  Get a short from an object.
 */
template <typename T>
static void get_short(T const& t,
                      std::string const& name,
                      data_member<T> const& member,
                      QSqlQuery& q) {
  std::string field(":");
  field.append(name);
  q.bindValue(field.c_str(), QVariant(t.*(member.s)));
  return ;
}

/**
 *  Get a string from an object.
 */
template <typename T>
static void get_string(T const& t,
                       std::string const& name,
                       data_member<T> const& member,
                       QSqlQuery& q) {
  std::string field(":");
  field.append(name);
  q.bindValue(field.c_str(), QVariant((t.*(member.S)).toStdString().c_str()));
  return ;
}

/**
 *  Get a time_t from an object.
 */
template <typename T>
static void get_timet(T const& t,
                      std::string const& name,
                      data_member<T> const& member,
                      QSqlQuery& q) {
  get_integer(t, name, member, q);
  return ;
}

/**
 *  Static initialization template used by initialize().
 */
template <typename T>
static void static_init() {
  for (unsigned int i = 0; mapped_type<T>::members[i].type; ++i)
    if (mapped_type<T>::members[i].name) {
      db_mapped_type<T>::list.push_back(
        std::make_pair(mapped_type<T>::members[i].name,
          getter_setter<T>()));
      getter_setter<T>& gs(db_mapped_type<T>::list.back().second);
      gs.member = &mapped_type<T>::members[i].member;
      // XXX : setters are not set.
      switch (mapped_type<T>::members[i].type) {
       case mapped_data<T>::BOOL:
        gs.getter = &get_boolean<T>;
        break ;
       case mapped_data<T>::DOUBLE:
        gs.getter = &get_double<T>;
        break ;
       case mapped_data<T>::ID:
        gs.getter = &get_id<T>;
        break ;
       case mapped_data<T>::INT:
        gs.getter = &get_integer<T>;
        break ;
       case mapped_data<T>::SHORT:
        gs.getter = &get_short<T>;
        break ;
       case mapped_data<T>::STRING:
        gs.getter = &get_string<T>;
        break ;
       case mapped_data<T>::TIME_T:
        gs.getter = &get_timet<T>;
        break ;
       default: // Error in one of the mappings.
        assert(false);
        abort();
      }
    }
  return ;
}

/**
 *  Extract data from object to DB row.
 */
template <typename T>
static void to_base(QSqlQuery& q, T const& t) {
  for (typename std::list<std::pair<std::string, getter_setter<T> > >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it)
    (it->second.getter)(t, it->first, *it->second.member, q);
  return ;
}

/**************************************
*                                     *
*           Global Objects            *
*                                     *
**************************************/

template <> std::list<std::pair<std::string, getter_setter<neb::acknowledgement> > >
  sql::db_mapped_type<neb::acknowledgement>::list =
    std::list<std::pair<std::string, getter_setter<neb::acknowledgement> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::comment> > >
  sql::db_mapped_type<neb::comment>::list =
    std::list<std::pair<std::string, getter_setter<neb::comment> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::custom_variable> > >
  sql::db_mapped_type<neb::custom_variable>::list =
    std::list<std::pair<std::string, getter_setter<neb::custom_variable> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::custom_variable_status> > >
  sql::db_mapped_type<neb::custom_variable_status>::list =
    std::list<std::pair<std::string, getter_setter<neb::custom_variable_status> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::downtime> > >
  sql::db_mapped_type<neb::downtime>::list =
    std::list<std::pair<std::string, getter_setter<neb::downtime> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::event_handler> > >
  sql::db_mapped_type<neb::event_handler>::list =
    std::list<std::pair<std::string, getter_setter<neb::event_handler> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::flapping_status> > >
  sql::db_mapped_type<neb::flapping_status>::list =
    std::list<std::pair<std::string, getter_setter<neb::flapping_status> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::host> > >
  sql::db_mapped_type<neb::host>::list =
    std::list<std::pair<std::string, getter_setter<neb::host> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::host_check> > >
  sql::db_mapped_type<neb::host_check>::list =
    std::list<std::pair<std::string, getter_setter<neb::host_check> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::host_dependency> > >
  sql::db_mapped_type<neb::host_dependency>::list =
    std::list<std::pair<std::string, getter_setter<neb::host_dependency> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::host_group> > >
  sql::db_mapped_type<neb::host_group>::list =
    std::list<std::pair<std::string, getter_setter<neb::host_group> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::host_group_member> > >
  sql::db_mapped_type<neb::host_group_member>::list =
    std::list<std::pair<std::string, getter_setter<neb::host_group_member> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::host_parent> > >
  sql::db_mapped_type<neb::host_parent>::list =
    std::list<std::pair<std::string, getter_setter<neb::host_parent> > >();
/*template <> std::list<std::pair<std::string, getter_setter<neb::host_state> > >
  sql::db_mapped_type<neb::host_state>::list =
  std::list<std::pair<std::string, getter_setter<neb::host_state> > >();*/
template <> std::list<std::pair<std::string, getter_setter<neb::host_status> > >
  sql::db_mapped_type<neb::host_status>::list =
    std::list<std::pair<std::string, getter_setter<neb::host_status> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::instance> > >
  sql::db_mapped_type<neb::instance>::list =
    std::list<std::pair<std::string, getter_setter<neb::instance> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::instance_status> > >
  sql::db_mapped_type<neb::instance_status>::list =
    std::list<std::pair<std::string, getter_setter<neb::instance_status> > >();
/*template <> std::list<std::pair<std::string, getter_setter<neb::issue> > >
  sql::db_mapped_type<neb::issue>::list =
  std::list<std::pair<std::string, getter_setter<neb::issue> > >();*/
template <> std::list<std::pair<std::string, getter_setter<neb::log_entry> > >
  sql::db_mapped_type<neb::log_entry>::list =
    std::list<std::pair<std::string, getter_setter<neb::log_entry> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::module> > >
  sql::db_mapped_type<neb::module>::list =
    std::list<std::pair<std::string, getter_setter<neb::module> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::notification> > >
  sql::db_mapped_type<neb::notification>::list =
    std::list<std::pair<std::string, getter_setter<neb::notification> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::service> > >
  sql::db_mapped_type<neb::service>::list =
    std::list<std::pair<std::string, getter_setter<neb::service> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::service_check> > >
  sql::db_mapped_type<neb::service_check>::list =
    std::list<std::pair<std::string, getter_setter<neb::service_check> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::service_dependency> > >
  sql::db_mapped_type<neb::service_dependency>::list =
    std::list<std::pair<std::string, getter_setter<neb::service_dependency> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::service_group> > >
  sql::db_mapped_type<neb::service_group>::list =
    std::list<std::pair<std::string, getter_setter<neb::service_group> > >();
template <> std::list<std::pair<std::string, getter_setter<neb::service_group_member> > >
  sql::db_mapped_type<neb::service_group_member>::list =
    std::list<std::pair<std::string, getter_setter<neb::service_group_member> > >();
/*template <> std::list<std::pair<std::string, getter_setter<neb::service_state> > >
  sql::db_mapped_type<neb::service_state>::list =
  std::list<std::pair<std::string, getter_setter<neb::service_state> > >();*/
template <> std::list<std::pair<std::string, getter_setter<neb::service_status> > >
  sql::db_mapped_type<neb::service_status>::list =
    std::list<std::pair<std::string, getter_setter<neb::service_status> > >();

/**************************************
*                                     *
*          Global Functions           *
*                                     *
**************************************/

/**
 *  @brief Initialization routine.
 *
 *  Initialize DB mappings.
 */
void sql::initialize() {
  static_init<neb::acknowledgement>();
  static_init<neb::comment>();
  static_init<neb::custom_variable>();
  static_init<neb::custom_variable_status>();
  static_init<neb::downtime>();
  static_init<neb::event_handler>();
  static_init<neb::flapping_status>();
  static_init<neb::host>();
  static_init<neb::host_check>();
  static_init<neb::host_dependency>();
  static_init<neb::host_group>();
  static_init<neb::host_group_member>();
  static_init<neb::host_parent>();
  //static_init<neb::host_state>();
  static_init<neb::host_status>();
  static_init<neb::instance>();
  static_init<neb::instance_status>();
  //static_init<neb::issue>();
  static_init<neb::log_entry>();
  static_init<neb::module>();
  static_init<neb::notification>();
  static_init<neb::service>();
  static_init<neb::service_check>();
  static_init<neb::service_dependency>();
  static_init<neb::service_group>();
  static_init<neb::service_group_member>();
  //static_init<neb::service_state>();
  static_init<neb::service_status>();
  return ;
}

/**
 *  ORM operator for acknowledgement.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::acknowledgement const& a) {
  to_base(q, a);
  return (q);
}

/**
 *  ORM operator for comment.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::comment const& c) {
  to_base(q, c);
  return (q);
}

/**
 *  ORM operator for custom_variable.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::custom_variable const& cv) {
  to_base(q, cv);
  return (q);
}

/**
 *  ORM operator for custom_variable_status.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::custom_variable_status const& cvs) {
  to_base(q, cvs);
  return (q);
}

/**
 *  ORM operator for downtime.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::downtime const& d) {
  to_base(q, d);
  return (q);
}

/**
 *  ORM operator for event_handler.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::event_handler const& eh) {
  to_base(q, eh);
  return (q);
}

/**
 *  ORM operator for flapping_status.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::flapping_status const& fs) {
  to_base(q, fs);
  return (q);
}

/**
 *  ORM operator for host.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::host const& h) {
  to_base(q, h);
  return (q);
}

/**
 *  ORM operator for host_check.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::host_check const& hc) {
  to_base(q, hc);
  return (q);
}

/**
 *  ORM operator for host_dependency.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::host_dependency const& hd) {
  to_base(q, hd);
  return (q);
}

/**
 *  ORM operator for host_group.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::host_group const& hg) {
  to_base(q, hg);
  return (q);
}

/**
 *  ORM operator for host_group_member.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::host_group_member const& hgm) {
  to_base(q, hgm);
  return (q);
}

/**
 *  ORM operator for host_parent.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::host_parent const& hp) {
  to_base(q, hp);
  return (q);
}

/**
 *  ORM operator for host_state.
 *
QSqlQuery& operator<<(QSqlQuery& q, neb::host_state const& hs) {
  to_base(q, hs);
  return (q);
}
*/
/**
 *  ORM operator for host_status.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::host_status const& hs) {
  to_base(q, hs);
  return (q);
}

/**
 *  ORM operator for instance.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::instance const& p) {
  to_base(q, p);
  return (q);
}

/**
 *  ORM operator for instance_status.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::instance_status const& ps) {
  to_base(q, ps);
  return (q);
}

/**
 *  ORM operator for issue.
 */
/*QSqlQuery& operator<<(QSqlQuery& q, neb::issue const& i) {
  to_base(q, i);
  return (q);
  }*/

/**
 *  ORM operator for log_entry.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::log_entry const& le) {
  to_base(q, le);
  return (q);
}

/**
 *  ORM operator for module.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::module const& m) {
  to_base(q, m);
  return (q);
}

/**
 *  ORM operator for notification.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::notification const& n) {
  to_base(q, n);
  return (q);
}

/**
 *  ORM operator for service.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::service const& s) {
  to_base(q, s);
  return (q);
}

/**
 *  ORM operator for service_check.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::service_check const& sc) {
  to_base(q, sc);
  return (q);
}

/**
 *  ORM operator for service_dependency.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::service_dependency const& sd) {
  to_base(q, sd);
  return (q);
}

/**
 *  ORM operator for service_group.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::service_group const& sg) {
  to_base(q, sg);
  return (q);
}

/**
 *  ORM operator for service_group_member.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::service_group_member const& sgm) {
  to_base(q, sgm);
  return (q);
}

/**
 *  ORM operator for service_state.
 *
QSqlQuery& operator<<(QSqlQuery& q, neb::service_state const& ss) {
  to_base(q, ss);
  return (q);
  }*/

/**
 *  ORM operator for service_status.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::service_status const& ss) {
  to_base(q, ss);
  return (q);
}
