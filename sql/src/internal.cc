/*
** Copyright 2009-2012 Merethis
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
static void get_boolean(
              T const& t,
              QString const& field,
              data_member<T> const& member,
              QSqlQuery& q) {
  q.bindValue(field, QVariant(t.*(member.b)));
  return ;
}

/**
 *  Get a double from an object.
 */
template <typename T>
static void get_double(
              T const& t,
              QString const& field,
              data_member<T> const& member,
              QSqlQuery& q) {
  q.bindValue(field, QVariant(t.*(member.d)));
  return ;
}

/**
 *  Get an integer from an object.
 */
template <typename T>
static void get_integer(
              T const& t,
              QString const& field,
              data_member<T> const& member,
              QSqlQuery& q) {
  q.bindValue(field, QVariant(t.*(member.i)));
  return ;
}

/**
 *  Get an integer that might be null from an object.
 */
template <typename T>
static void get_integer_might_be_null(
              T const& t,
              QString const& field,
              data_member<T> const& member,
              QSqlQuery& q) {
  int val(t.*(member.i));
  // Not-NULL.
  if (val)
    q.bindValue(field, QVariant(val));
  // NULL.
  else
    q.bindValue(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Get a short from an object.
 */
template <typename T>
static void get_short(
              T const& t,
              QString const& field,
              data_member<T> const& member,
              QSqlQuery& q) {
  q.bindValue(field, QVariant(t.*(member.s)));
  return ;
}

/**
 *  Get a string from an object.
 */
template <typename T>
static void get_string(
              T const& t,
              QString const& field,
              data_member<T> const& member,
              QSqlQuery& q) {
  q.bindValue(field, QVariant((t.*(member.S)).toStdString().c_str()));
  return ;
}

/**
 *  Get a string that might be null from an object.
 */
template <typename T>
static void get_string_might_be_null(
              T const& t,
              QString const& field,
              data_member<T> const& member,
              QSqlQuery& q) {
  // Not-NULL.
  if (!(t.*(member.S)).isEmpty())
    q.bindValue(field, QVariant((t.*(member.S)).toStdString().c_str()));
  // NULL.
  else
    q.bindValue(field, QVariant(QVariant::String));
  return ;
}

#ifndef NO_TIME_T_MAPPING
/**
 *  Get a time_t from an object.
 */
template <typename T>
static void get_timet(
              T const& t,
              QString const& field,
              data_member<T> const& member,
              QSqlQuery& q) {
  get_integer<T>(t, field, member, q);
  return ;
}

/**
 *  Get a time_t that might be null from an object.
 */
template <typename T>
static void get_timet_might_be_null(
              T const& t,
              QString const& field,
              data_member<T> const& member,
              QSqlQuery& q) {
  get_integer_might_be_null<T>(t, field, member, q);
  return ;
}
#endif // !NO_TIME_T_MAPPING

/**
 *  Get an unsigned int from an object.
 */
template <typename T>
static void get_uint(
              T const& t,
              QString const& field,
              data_member<T> const& member,
              QSqlQuery& q) {
  q.bindValue(field, QVariant(t.*member.u));
  return ;
}

/**
 *  Get an unsigned int that might be null from an object.
 */
template <typename T>
static void get_uint_might_be_null(
              T const& t,
              QString const& field,
              data_member<T> const& member,
              QSqlQuery& q) {
  unsigned int val(t.*(member.u));
  // Not-NULL.
  if (val)
    q.bindValue(field, QVariant(val));
  // NULL
  else
    q.bindValue(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Static initialization template used by initialize().
 */
template <typename T>
static void static_init() {
  for (unsigned int i = 0; mapped_type<T>::members[i].type; ++i)
    if (mapped_type<T>::members[i].name) {
      db_mapped_type<T>::list.push_back(db_mapped_entry<T>());
      db_mapped_entry<T>& entry(db_mapped_type<T>::list.back());
      entry.name = mapped_type<T>::members[i].name;
      entry.name.squeeze();
      entry.field = ":";
      entry.field.append(entry.name);
      entry.field.squeeze();
      entry.gs.member = &mapped_type<T>::members[i].member;
      // XXX : setters are not set.
      switch (mapped_type<T>::members[i].type) {
      case mapped_data<T>::BOOL:
        entry.gs.getter = &get_boolean<T>;
        break ;
      case mapped_data<T>::DOUBLE:
        entry.gs.getter = &get_double<T>;
        break ;
      case mapped_data<T>::INT:
        if (mapped_type<T>::members[i].null_on_zero)
          entry.gs.getter = &get_integer_might_be_null<T>;
        else
          entry.gs.getter = &get_integer<T>;
        break ;
      case mapped_data<T>::SHORT:
        entry.gs.getter = &get_short<T>;
        break ;
      case mapped_data<T>::STRING:
        if (mapped_type<T>::members[i].null_on_zero)
          entry.gs.getter = &get_string_might_be_null<T>;
        else
          entry.gs.getter = &get_string<T>;
        break ;
#ifndef NO_TIME_T_MAPPING
      case mapped_data<T>::TIME_T:
        if (mapped_type<T>::members[i].null_on_zero)
          entry.gs.getter = &get_timet_might_be_null<T>;
        else
          entry.gs.getter = &get_timet<T>;
        break ;
#endif // !NO_TIME_T_MAPPING
      case mapped_data<T>::UINT:
        if (mapped_type<T>::members[i].null_on_zero)
          entry.gs.getter = &get_uint_might_be_null<T>;
        else
          entry.gs.getter = &get_uint<T>;
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
  for (typename std::vector<db_mapped_entry<T> >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it)
    (it->gs.getter)(t, it->field, *it->gs.member, q);
  return ;
}

/**************************************
*                                     *
*           Global Objects            *
*                                     *
**************************************/

namespace       com {
  namespace     centreon {
    namespace   broker {
      namespace sql {
        template <> std::vector<db_mapped_entry<neb::acknowledgement> >
          db_mapped_type<neb::acknowledgement>::list =
            std::vector<db_mapped_entry<neb::acknowledgement> >();
        template <> std::vector<db_mapped_entry<neb::comment> >
          db_mapped_type<neb::comment>::list =
            std::vector<db_mapped_entry<neb::comment> >();
        template <> std::vector<db_mapped_entry<neb::custom_variable> >
          db_mapped_type<neb::custom_variable>::list =
            std::vector<db_mapped_entry<neb::custom_variable> >();
        template <> std::vector<db_mapped_entry<neb::custom_variable_status> >
          db_mapped_type<neb::custom_variable_status>::list =
            std::vector<db_mapped_entry<neb::custom_variable_status> >();
        template <> std::vector<db_mapped_entry<neb::downtime> >
          db_mapped_type<neb::downtime>::list =
            std::vector<db_mapped_entry<neb::downtime> >();
        template <> std::vector<db_mapped_entry<neb::event_handler> >
          db_mapped_type<neb::event_handler>::list =
            std::vector<db_mapped_entry<neb::event_handler> >();
        template <> std::vector<db_mapped_entry<neb::flapping_status> >
          db_mapped_type<neb::flapping_status>::list =
            std::vector<db_mapped_entry<neb::flapping_status> >();
        template <> std::vector<db_mapped_entry<neb::host> >
          db_mapped_type<neb::host>::list =
            std::vector<db_mapped_entry<neb::host> >();
        template <> std::vector<db_mapped_entry<neb::host_check> >
          db_mapped_type<neb::host_check>::list =
            std::vector<db_mapped_entry<neb::host_check> >();
        template <> std::vector<db_mapped_entry<neb::host_dependency> >
          db_mapped_type<neb::host_dependency>::list =
            std::vector<db_mapped_entry<neb::host_dependency> >();
        template <> std::vector<db_mapped_entry<neb::host_group> >
          db_mapped_type<neb::host_group>::list =
            std::vector<db_mapped_entry<neb::host_group> >();
        template <> std::vector<db_mapped_entry<neb::host_group_member> >
          db_mapped_type<neb::host_group_member>::list =
            std::vector<db_mapped_entry<neb::host_group_member> >();
        template <> std::vector<db_mapped_entry<neb::host_parent> >
          db_mapped_type<neb::host_parent>::list =
            std::vector<db_mapped_entry<neb::host_parent> >();
        template <> std::vector<db_mapped_entry<neb::host_status> >
          db_mapped_type<neb::host_status>::list =
            std::vector<db_mapped_entry<neb::host_status> >();
        template <> std::vector<db_mapped_entry<neb::instance> >
          db_mapped_type<neb::instance>::list =
            std::vector<db_mapped_entry<neb::instance> >();
        template <> std::vector<db_mapped_entry<neb::instance_status> >
          db_mapped_type<neb::instance_status>::list =
            std::vector<db_mapped_entry<neb::instance_status> >();
        template <> std::vector<db_mapped_entry<neb::log_entry> >
          db_mapped_type<neb::log_entry>::list =
            std::vector<db_mapped_entry<neb::log_entry> >();
        template <> std::vector<db_mapped_entry<neb::module> >
          db_mapped_type<neb::module>::list =
            std::vector<db_mapped_entry<neb::module> >();
        template <> std::vector<db_mapped_entry<neb::notification> >
          db_mapped_type<neb::notification>::list =
            std::vector<db_mapped_entry<neb::notification> >();
        template <> std::vector<db_mapped_entry<neb::service> >
          db_mapped_type<neb::service>::list =
            std::vector<db_mapped_entry<neb::service> >();
        template <> std::vector<db_mapped_entry<neb::service_check> >
          db_mapped_type<neb::service_check>::list =
            std::vector<db_mapped_entry<neb::service_check> >();
        template <> std::vector<db_mapped_entry<neb::service_dependency> >
          db_mapped_type<neb::service_dependency>::list =
            std::vector<db_mapped_entry<neb::service_dependency> >();
        template <> std::vector<db_mapped_entry<neb::service_group> >
          db_mapped_type<neb::service_group>::list =
            std::vector<db_mapped_entry<neb::service_group> >();
        template <> std::vector<db_mapped_entry<neb::service_group_member> >
          db_mapped_type<neb::service_group_member>::list =
            std::vector<db_mapped_entry<neb::service_group_member> >();
        template <> std::vector<db_mapped_entry<neb::service_status> >
          db_mapped_type<neb::service_status>::list =
            std::vector<db_mapped_entry<neb::service_status> >();
        template <> std::vector<db_mapped_entry<correlation::host_state> >
          db_mapped_type<correlation::host_state>::list =
            std::vector<db_mapped_entry<correlation::host_state> >();
        template <> std::vector<db_mapped_entry<correlation::issue> >
          db_mapped_type<correlation::issue>::list =
            std::vector<db_mapped_entry<correlation::issue> >();
        template <> std::vector<db_mapped_entry<correlation::service_state> >
          db_mapped_type<correlation::service_state>::list =
            std::vector<db_mapped_entry<correlation::service_state> >();
     }
    }
  }
}

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
  static_init<neb::host_status>();
  static_init<neb::instance>();
  static_init<neb::instance_status>();
  static_init<neb::log_entry>();
  static_init<neb::module>();
  static_init<neb::notification>();
  static_init<neb::service>();
  static_init<neb::service_check>();
  static_init<neb::service_dependency>();
  static_init<neb::service_group>();
  static_init<neb::service_group_member>();
  static_init<neb::service_status>();
  static_init<correlation::host_state>();
  static_init<correlation::issue>();
  static_init<correlation::service_state>();
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
 *  ORM operator for service_status.
 */
QSqlQuery& operator<<(QSqlQuery& q, neb::service_status const& ss) {
  to_base(q, ss);
  return (q);
}

/**
 *  ORM operator for host_state.
 */
QSqlQuery& operator<<(QSqlQuery& q, correlation::host_state const& hs) {
  to_base(q, hs);
  return (q);
}

/**
 *  ORM operator for issue.
 */
QSqlQuery& operator<<(QSqlQuery& q, correlation::issue const& i) {
  to_base(q, i);
  return (q);
}

/**
 *  ORM operator for service_state.
 */
QSqlQuery& operator<<(QSqlQuery& q, correlation::service_state const& ss) {
  to_base(q, ss);
  return (q);
}
