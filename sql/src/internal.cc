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

#include <QVariant>
#include "com/centreon/broker/database_query.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
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
 *
 *  @param[out] q  Query object.
 *  @param[in]  d  Object.
 */
database_query& operator<<(database_query& q, io::data const& d) {
  // Get event info.
  io::event_info const*
    info(io::events::instance().get_event_info(d.type()));
  if (info) {
    for (mapping::entry const* current_entry(info->get_mapping());
         !current_entry->is_null();
         ++current_entry) {
      char const* entry_name(current_entry->get_name());
      if (entry_name && entry_name[0]) {
        QString field(":");
        field.append(entry_name);
        switch (current_entry->get_type()) {
        case mapping::source::BOOL:
          bind_boolean(field, current_entry->get_bool(d), q);
          break ;
        case mapping::source::DOUBLE:
          bind_double(field, current_entry->get_double(d), q);
          break ;
        case mapping::source::INT:
          switch (current_entry->get_attribute()) {
          case mapping::entry::invalid_on_zero:
            bind_integer_null_on_zero(
              field,
              current_entry->get_int(d),
              q);
            break ;
          case mapping::entry::invalid_on_minus_one:
            bind_integer_null_on_minus_one(
              field,
              current_entry->get_int(d),
              q);
            break ;
          default:
            bind_integer(field, current_entry->get_int(d), q);
          }
          break ;
        case mapping::source::SHORT:
          bind_short(field, current_entry->get_short(d), q);
          break ;
        case mapping::source::STRING:
          if (current_entry->get_attribute()
              == mapping::entry::invalid_on_zero)
            bind_string_null_on_empty(
              field,
              current_entry->get_string(d),
              q);
          else
            bind_string(field, current_entry->get_string(d), q);
          break ;
        case mapping::source::TIME:
          switch (current_entry->get_attribute()) {
          case mapping::entry::invalid_on_zero:
            bind_timet_null_on_zero(
              field,
              current_entry->get_time(d),
              q);
            break ;
          case mapping::entry::invalid_on_minus_one:
            bind_timet_null_on_minus_one(
              field,
              current_entry->get_time(d),
              q);
            break ;
          default:
            bind_timet(field, current_entry->get_time(d), q);
          }
          break ;
        case mapping::source::UINT:
          switch (current_entry->get_attribute()) {
          case mapping::entry::invalid_on_zero:
            bind_uint_null_on_zero(
              field,
              current_entry->get_uint(d),
              q);
            break ;
          case mapping::entry::invalid_on_minus_one:
            bind_uint_null_on_minus_one(
              field,
              current_entry->get_uint(d),
              q);
            break ;
          default :
            bind_uint(field, current_entry->get_uint(d), q);
          }
          break ;
        default: // Error in one of the mappings.
          throw (exceptions::msg() << "SQL: invalid mapping for object "
                 << "of type '" << info->get_name() << "': "
                 << current_entry->get_type()
                 << " is not a known type ID");
        }
      }
    }
  }
  else
    throw (exceptions::msg() << "SQL: cannot bind object of type "
           << d.type() << " to database query: mapping does not exist");
  return (q);
}
