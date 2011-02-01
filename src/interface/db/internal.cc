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

#include <QSqlQuery>
#include <QVariant>
#include <assert.h>
#include <stdlib.h>
#include "interface/db/internal.hh"

using namespace interface::db;

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
  q.bindValue(field.c_str(), QVariant((t.*(member.S)).c_str()));
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

template <> std::list<std::pair<std::string, getter_setter<events::acknowledgement> > >
  interface::db::db_mapped_type<events::acknowledgement>::list =
    std::list<std::pair<std::string, getter_setter<events::acknowledgement> > >();
template <> std::list<std::pair<std::string, getter_setter<events::comment> > >
  interface::db::db_mapped_type<events::comment>::list =
    std::list<std::pair<std::string, getter_setter<events::comment> > >();
template <> std::list<std::pair<std::string, getter_setter<events::custom_variable> > >
  interface::db::db_mapped_type<events::custom_variable>::list =
    std::list<std::pair<std::string, getter_setter<events::custom_variable> > >();
template <> std::list<std::pair<std::string, getter_setter<events::custom_variable_status> > >
  interface::db::db_mapped_type<events::custom_variable_status>::list =
    std::list<std::pair<std::string, getter_setter<events::custom_variable_status> > >();
template <> std::list<std::pair<std::string, getter_setter<events::downtime> > >
  interface::db::db_mapped_type<events::downtime>::list =
    std::list<std::pair<std::string, getter_setter<events::downtime> > >();
template <> std::list<std::pair<std::string, getter_setter<events::event_handler> > >
  interface::db::db_mapped_type<events::event_handler>::list =
    std::list<std::pair<std::string, getter_setter<events::event_handler> > >();
template <> std::list<std::pair<std::string, getter_setter<events::flapping_status> > >
  interface::db::db_mapped_type<events::flapping_status>::list =
    std::list<std::pair<std::string, getter_setter<events::flapping_status> > >();
template <> std::list<std::pair<std::string, getter_setter<events::host> > >
  interface::db::db_mapped_type<events::host>::list =
    std::list<std::pair<std::string, getter_setter<events::host> > >();
template <> std::list<std::pair<std::string, getter_setter<events::host_check> > >
  interface::db::db_mapped_type<events::host_check>::list =
    std::list<std::pair<std::string, getter_setter<events::host_check> > >();
template <> std::list<std::pair<std::string, getter_setter<events::host_dependency> > >
  interface::db::db_mapped_type<events::host_dependency>::list =
    std::list<std::pair<std::string, getter_setter<events::host_dependency> > >();
template <> std::list<std::pair<std::string, getter_setter<events::host_group> > >
  interface::db::db_mapped_type<events::host_group>::list =
    std::list<std::pair<std::string, getter_setter<events::host_group> > >();
template <> std::list<std::pair<std::string, getter_setter<events::host_group_member> > >
  interface::db::db_mapped_type<events::host_group_member>::list =
    std::list<std::pair<std::string, getter_setter<events::host_group_member> > >();
template <> std::list<std::pair<std::string, getter_setter<events::host_parent> > >
  interface::db::db_mapped_type<events::host_parent>::list =
    std::list<std::pair<std::string, getter_setter<events::host_parent> > >();
template <> std::list<std::pair<std::string, getter_setter<events::host_state> > >
  interface::db::db_mapped_type<events::host_state>::list =
    std::list<std::pair<std::string, getter_setter<events::host_state> > >();
template <> std::list<std::pair<std::string, getter_setter<events::host_status> > >
  interface::db::db_mapped_type<events::host_status>::list =
    std::list<std::pair<std::string, getter_setter<events::host_status> > >();
template <> std::list<std::pair<std::string, getter_setter<events::instance> > >
  interface::db::db_mapped_type<events::instance>::list =
    std::list<std::pair<std::string, getter_setter<events::instance> > >();
template <> std::list<std::pair<std::string, getter_setter<events::instance_status> > >
  interface::db::db_mapped_type<events::instance_status>::list =
    std::list<std::pair<std::string, getter_setter<events::instance_status> > >();
template <> std::list<std::pair<std::string, getter_setter<events::issue> > >
  interface::db::db_mapped_type<events::issue>::list =
    std::list<std::pair<std::string, getter_setter<events::issue> > >();
template <> std::list<std::pair<std::string, getter_setter<events::log_entry> > >
  interface::db::db_mapped_type<events::log_entry>::list =
    std::list<std::pair<std::string, getter_setter<events::log_entry> > >();
template <> std::list<std::pair<std::string, getter_setter<events::module> > >
  interface::db::db_mapped_type<events::module>::list =
    std::list<std::pair<std::string, getter_setter<events::module> > >();
template <> std::list<std::pair<std::string, getter_setter<events::notification> > >
  interface::db::db_mapped_type<events::notification>::list =
    std::list<std::pair<std::string, getter_setter<events::notification> > >();
template <> std::list<std::pair<std::string, getter_setter<events::service> > >
  interface::db::db_mapped_type<events::service>::list =
    std::list<std::pair<std::string, getter_setter<events::service> > >();
template <> std::list<std::pair<std::string, getter_setter<events::service_check> > >
  interface::db::db_mapped_type<events::service_check>::list =
    std::list<std::pair<std::string, getter_setter<events::service_check> > >();
template <> std::list<std::pair<std::string, getter_setter<events::service_dependency> > >
  interface::db::db_mapped_type<events::service_dependency>::list =
    std::list<std::pair<std::string, getter_setter<events::service_dependency> > >();
template <> std::list<std::pair<std::string, getter_setter<events::service_group> > >
  interface::db::db_mapped_type<events::service_group>::list =
    std::list<std::pair<std::string, getter_setter<events::service_group> > >();
template <> std::list<std::pair<std::string, getter_setter<events::service_group_member> > >
  interface::db::db_mapped_type<events::service_group_member>::list =
    std::list<std::pair<std::string, getter_setter<events::service_group_member> > >();
template <> std::list<std::pair<std::string, getter_setter<events::service_state> > >
  interface::db::db_mapped_type<events::service_state>::list =
    std::list<std::pair<std::string, getter_setter<events::service_state> > >();
template <> std::list<std::pair<std::string, getter_setter<events::service_status> > >
  interface::db::db_mapped_type<events::service_status>::list =
    std::list<std::pair<std::string, getter_setter<events::service_status> > >();

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
void interface::db::initialize() {
  static_init<events::acknowledgement>();
  static_init<events::comment>();
  static_init<events::custom_variable>();
  static_init<events::custom_variable_status>();
  static_init<events::downtime>();
  static_init<events::event_handler>();
  static_init<events::flapping_status>();
  static_init<events::host>();
  static_init<events::host_check>();
  static_init<events::host_dependency>();
  static_init<events::host_group>();
  static_init<events::host_group_member>();
  static_init<events::host_parent>();
  static_init<events::host_state>();
  static_init<events::host_status>();
  static_init<events::instance>();
  static_init<events::instance_status>();
  static_init<events::issue>();
  static_init<events::log_entry>();
  static_init<events::module>();
  static_init<events::notification>();
  static_init<events::service>();
  static_init<events::service_check>();
  static_init<events::service_dependency>();
  static_init<events::service_group>();
  static_init<events::service_group_member>();
  static_init<events::service_state>();
  static_init<events::service_status>();
  return ;
}

/**
 *  ORM operator for acknowledgement.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::acknowledgement const& a) {
  to_base(q, a);
  return (q);
}

/**
 *  ORM operator for comment.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::comment const& c) {
  to_base(q, c);
  return (q);
}

/**
 *  ORM operator for custom_variable.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::custom_variable const& cv) {
  to_base(q, cv);
  return (q);
}

/**
 *  ORM operator for custom_variable_status.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::custom_variable_status const& cvs) {
  to_base(q, cvs);
  return (q);
}

/**
 *  ORM operator for downtime.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::downtime const& d) {
  to_base(q, d);
  return (q);
}

/**
 *  ORM operator for event_handler.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::event_handler const& eh) {
  to_base(q, eh);
  return (q);
}

/**
 *  ORM operator for flapping_status.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::flapping_status const& fs) {
  to_base(q, fs);
  return (q);
}

/**
 *  ORM operator for host.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::host const& h) {
  to_base(q, h);
  return (q);
}

/**
 *  ORM operator for host_check.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::host_check const& hc) {
  to_base(q, hc);
  return (q);
}

/**
 *  ORM operator for host_dependency.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::host_dependency const& hd) {
  to_base(q, hd);
  return (q);
}

/**
 *  ORM operator for host_group.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::host_group const& hg) {
  to_base(q, hg);
  return (q);
}

/**
 *  ORM operator for host_group_member.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::host_group_member const& hgm) {
  to_base(q, hgm);
  return (q);
}

/**
 *  ORM operator for host_parent.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::host_parent const& hp) {
  to_base(q, hp);
  return (q);
}

/**
 *  ORM operator for host_state.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::host_state const& hs) {
  to_base(q, hs);
  return (q);
}

/**
 *  ORM operator for host_status.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::host_status const& hs) {
  to_base(q, hs);
  return (q);
}

/**
 *  ORM operator for instance.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::instance const& p) {
  to_base(q, p);
  return (q);
}

/**
 *  ORM operator for instance_status.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::instance_status const& ps) {
  to_base(q, ps);
  return (q);
}

/**
 *  ORM operator for issue.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::issue const& i) {
  to_base(q, i);
  return (q);
}

/**
 *  ORM operator for log_entry.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::log_entry const& le) {
  to_base(q, le);
  return (q);
}

/**
 *  ORM operator for module.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::module const& m) {
  to_base(q, m);
  return (q);
}

/**
 *  ORM operator for notification.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::notification const& n) {
  to_base(q, n);
  return (q);
}

/**
 *  ORM operator for service.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::service const& s) {
  to_base(q, s);
  return (q);
}

/**
 *  ORM operator for service_check.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::service_check const& sc) {
  to_base(q, sc);
  return (q);
}

/**
 *  ORM operator for service_dependency.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::service_dependency const& sd) {
  to_base(q, sd);
  return (q);
}

/**
 *  ORM operator for service_group.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::service_group const& sg) {
  to_base(q, sg);
  return (q);
}

/**
 *  ORM operator for service_group_member.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::service_group_member const& sgm) {
  to_base(q, sgm);
  return (q);
}

/**
 *  ORM operator for service_state.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::service_state const& ss) {
  to_base(q, ss);
  return (q);
}

/**
 *  ORM operator for service_status.
 */
QSqlQuery& operator<<(QSqlQuery& q, events::service_status const& ss) {
  to_base(q, ss);
  return (q);
}
