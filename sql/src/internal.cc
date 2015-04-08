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

#include <cassert>
#include <cstdlib>
#include <QVariant>
#include "com/centreon/broker/database_query.hh"
#include "com/centreon/broker/sql/internal.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Get a boolean from an object.
 */
static void bind_boolean(
              QString const& field,
              bool val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get a double from an object.
 */
static void bind_double(
              QString const& field,
              double val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get an integer from an object.
 */
static void bind_integer(
              QString const& field,
              int val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get an integer that is null on zero.
 */
static void bind_integer_null_on_zero(
              QString const& field,
              int val,
              database_query& q) {
  // Not-NULL.
  if (val)
    q.bind_value(field, QVariant(val));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Get an integer that is null on -1.
 */
static void bind_integer_null_on_minus_one(
              QString const& field,
              int val,
              database_query& q) {
  // Not-NULL.
  if (val != -1)
    q.bind_value(field, QVariant(val));
  else
    q.bind_value(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Get a short from an object.
 */
static void bind_short(
              QString const& field,
              short val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get a string from an object.
 */
static void bind_string(
              QString const& field,
              QString const& val,
              database_query& q) {
  q.bind_value(field, QVariant(val.toStdString().c_str()));
  return ;
}

/**
 *  Get a string that might be null from an object.
 */
static void bind_string_null_on_empty(
              QString const& field,
              QString const& val,
              database_query& q) {
  // Not-NULL.
  if (!val.isEmpty())
    q.bind_value(field, QVariant(val.toStdString().c_str()));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::String));
  return ;
}

/**
 *  Get a time_t from an object.
 */
static void bind_timet(
              QString const& field,
              time_t val,
              database_query& q) {
  q.bind_value(
      field,
      QVariant(static_cast<qlonglong>(val)));
  return ;
}

/**
 *  Get a time_t that is null on 0.
 */
static void bind_timet_null_on_zero(
              QString const& field,
              time_t val,
              database_query& q) {
  qlonglong vall(val);
  // Not-NULL.
  if (vall)
    q.bind_value(field, QVariant(vall));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::LongLong));
  return ;
}

/**
 *  Get a time_t that is null on -1.
 */
static void bind_timet_null_on_minus_one(
              QString const& field,
              time_t val,
              database_query& q) {
  qlonglong vall(val);
  // Not-NULL.
  if (vall != -1)
    q.bind_value(field, QVariant(vall));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::LongLong));
  return ;
}

/**
 *  Get an unsigned int from an object.
 */
static void bind_uint(
              QString const& field,
              unsigned int val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get an unsigned int that is null on zero.
 */
static void bind_uint_null_on_zero(
              QString const& field,
              unsigned int val,
              database_query& q) {
  // Not-NULL.
  if (val)
    q.bind_value(field, QVariant(val));
  // NULL
  else
    q.bind_value(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Get an unsigned int that is null on -1.
 */
static void bind_uint_null_on_minus_one(
              QString const& field,
              unsigned int val,
              database_query& q) {
  // Not-NULL.
  if (val != (unsigned int)-1)
    q.bind_value(field, QVariant(val));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Extract data from object to DB row.
 */
template <typename T>
static void to_base(database_query& q, T const& t) {
  mapping::entry const* entries = T::entries;
  for (; !entries->is_null(); ++entries) {
    if (entries->get_name().empty())
      continue ;
    QString field(":");
    field.append(entries->get_name().c_str());
    switch (entries->get_type()) {
    case mapping::source::BOOL:
      bind_boolean(field, entries->get_bool(t), q);
      break ;
    case mapping::source::DOUBLE:
      bind_double(field, entries->get_double(t), q);
      break ;
    case mapping::source::INT:
      if (entries->get_attribute() == mapping::entry::NULL_ON_ZERO)
        bind_integer_null_on_zero(field, entries->get_int(t), q);
      else if (entries->get_attribute() == mapping::entry::NULL_ON_MINUS_ONE)
        bind_integer_null_on_minus_one(field, entries->get_int(t), q);
      else
        bind_integer(field, entries->get_int(t), q);
      break ;
    case mapping::source::SHORT:
      bind_short(field, entries->get_short(t), q);
      break ;
    case mapping::source::STRING:
      if (entries->get_attribute() == mapping::entry::NULL_ON_ZERO)
        bind_string_null_on_empty(field, entries->get_string(t), q);
      else
        bind_string(field, entries->get_string(t), q);
      break ;
    case mapping::source::TIME:
      if (entries->get_attribute() == mapping::entry::NULL_ON_ZERO)
        bind_timet_null_on_zero(field, entries->get_time(t), q);
      else if (entries->get_attribute() == mapping::entry::NULL_ON_MINUS_ONE)
        bind_timet_null_on_minus_one(field, entries->get_time(t), q);
      else
        bind_timet(field, entries->get_time(t), q);
      break ;
    case mapping::source::UINT:
      if (entries->get_attribute() == mapping::entry::NULL_ON_ZERO)
        bind_uint_null_on_zero(field, entries->get_uint(t), q);
      else if (entries->get_attribute() == mapping::entry::NULL_ON_MINUS_ONE)
        bind_uint_null_on_minus_one(field, entries->get_uint(t), q);
      else
        bind_uint(field, entries->get_uint(t), q);
      break ;
    default: // Error in one of the mappings.
      assert(false);
      abort();
    }
  }
}

/**
 *  ORM operator for custom_variable.
 */
database_query& operator<<(database_query& q, neb::custom_variable const& cv) {
  to_base(q, cv);
  return (q);
}

/**
 *  ORM operator for custom_variable_status.
 */
database_query& operator<<(database_query& q, neb::custom_variable_status const& cvs) {
  to_base(q, cvs);
  return (q);
}

/**
 *  ORM operator for event_handler.
 */
database_query& operator<<(database_query& q, neb::event_handler const& eh) {
  to_base(q, eh);
  return (q);
}

/**
 *  ORM operator for flapping_status.
 */
database_query& operator<<(database_query& q, neb::flapping_status const& fs) {
  to_base(q, fs);
  return (q);
}

/**
 *  ORM operator for host.
 */
database_query& operator<<(database_query& q, neb::host const& h) {
  to_base(q, h);
  return (q);
}

/**
 *  ORM operator for host_check.
 */
database_query& operator<<(database_query& q, neb::host_check const& hc) {
  to_base(q, hc);
  return (q);
}

/**
 *  ORM operator for host_dependency.
 */
database_query& operator<<(database_query& q, neb::host_dependency const& hd) {
  to_base(q, hd);
  return (q);
}

/**
 *  ORM operator for host_group.
 */
database_query& operator<<(database_query& q, neb::host_group const& hg) {
  to_base(q, hg);
  return (q);
}

/**
 *  ORM operator for host_group_member.
 */
database_query& operator<<(database_query& q, neb::host_group_member const& hgm) {
  to_base(q, hgm);
  return (q);
}

/**
 *  ORM operator for host_parent.
 */
database_query& operator<<(database_query& q, neb::host_parent const& hp) {
  to_base(q, hp);
  return (q);
}

/**
 *  ORM operator for host_status.
 */
database_query& operator<<(database_query& q, neb::host_status const& hs) {
  to_base(q, hs);
  return (q);
}

/**
 *  ORM operator for instance.
 */
database_query& operator<<(database_query& q, neb::instance const& p) {
  to_base(q, p);
  return (q);
}

/**
 *  ORM operator for instance_status.
 */
database_query& operator<<(database_query& q, neb::instance_status const& ps) {
  to_base(q, ps);
  return (q);
}

/**
 *  ORM operator for log_entry.
 */
database_query& operator<<(database_query& q, neb::log_entry const& le) {
  to_base(q, le);
  return (q);
}

/**
 *  ORM operator for module.
 */
database_query& operator<<(database_query& q, neb::module const& m) {
  to_base(q, m);
  return (q);
}

/**
 *  ORM operator for notification.
 */
// XXX
// database_query& operator<<(database_query& q, neb::notification const& n) {
//   to_base(q, n);
//   return (q);
// }

/**
 *  ORM operator for service.
 */
database_query& operator<<(database_query& q, neb::service const& s) {
  to_base(q, s);
  return (q);
}

/**
 *  ORM operator for service_check.
 */
database_query& operator<<(database_query& q, neb::service_check const& sc) {
  to_base(q, sc);
  return (q);
}

/**
 *  ORM operator for service_dependency.
 */
database_query& operator<<(database_query& q, neb::service_dependency const& sd) {
  to_base(q, sd);
  return (q);
}

/**
 *  ORM operator for service_group.
 */
database_query& operator<<(database_query& q, neb::service_group const& sg) {
  to_base(q, sg);
  return (q);
}

/**
 *  ORM operator for service_group_member.
 */
database_query& operator<<(database_query& q, neb::service_group_member const& sgm) {
  to_base(q, sgm);
  return (q);
}

/**
 *  ORM operator for service_status.
 */
database_query& operator<<(database_query& q, neb::service_status const& ss) {
  to_base(q, ss);
  return (q);
}

/**
 *  ORM operator for acknowledgement.
 */
database_query& operator<<(database_query& q, notification::acknowledgement const& a) {
  to_base(q, a);
  return (q);
}

/**
 *  ORM operator for downtime.
 */
database_query& operator<<(database_query& q, notification::downtime const& d) {
  to_base(q, d);
  return (q);
}

/**
 *  ORM operator for host_state.
 */
database_query& operator<<(database_query& q, correlation::host_state const& hs) {
  to_base(q, hs);
  return (q);
}

/**
 *  ORM operator for issue.
 */
database_query& operator<<(database_query& q, correlation::issue const& i) {
  to_base(q, i);
  return (q);
}

/**
 *  ORM operator for service_state.
 */
database_query& operator<<(database_query& q, correlation::service_state const& ss) {
  to_base(q, ss);
  return (q);
}
