/*
** Copyright 2009-2014 Merethis
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
#include <iomanip>
#include <sstream>
#include "com/centreon/broker/bam/events.hh"
#include "com/centreon/broker/correlation/events.hh"
#include "com/centreon/broker/ndo/internal.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/storage/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::ndo;

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
              data_member<T> const& member,
              std::stringstream& buffer) {
  buffer << (t.*(member.b) ? "1" : "0");
  return ;
}

/**
 *  Get a double from an object.
 */
template <typename T>
static void get_double(
              T const& t,
              data_member<T> const& member,
              std::stringstream& buffer) {
  buffer << std::fixed << t.*(member.d);
  return ;
}

/**
 *  Get an integer from an object.
 */
template <typename T>
static void get_integer(
              T const& t,
              data_member<T> const& member,
              std::stringstream& buffer) {
  buffer << t.*(member.i);
  return ;
}

/**
 *  Get a short from an object.
 */
template <typename T>
static void get_short(
              T const& t,
              data_member<T> const& member,
              std::stringstream& buffer) {
  buffer << t.*(member.s);
  return ;
}

/**
 *  Get a string from an object.
 */
template <typename T>
static void get_string(
              T const& t,
              data_member<T> const& member,
              std::stringstream& buffer) {
  QByteArray value((t.*(member.S)).toUtf8());
  value.replace('\\', "\\\\");
  value.replace('\n', "\\n");
  buffer.write(value.constData(), value.size());
  return ;
}

/**
 *  Get a time_t from an object.
 */
template <typename T>
static void get_timet(
              T const& t,
              data_member<T> const& member,
              std::stringstream& buffer) {
  buffer << (t.*(member.t)).get_time_t();
  return ;
}

/**
 *  Get an unsigned integer from an object.
 */
template <typename T>
static void get_uint(
              T const& t,
              data_member<T> const& member,
              std::stringstream& buffer) {
  buffer << t.*(member.u);
  return ;
}

/**
 *  Set a boolean within an object.
 */
template <typename T>
static void set_boolean(
              T& t,
              data_member<T> const& member,
              char const* str) {
  t.*(member.b) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set a double within an object.
 */
template <typename T>
static void set_double(
              T& t,
              data_member<T> const& member,
              char const* str) {
  t.*(member.d) = strtod(str, NULL);
  return ;
}

/**
 *  Set an integer within an object.
 */
template <typename T>
static void set_integer(
              T& t,
              data_member<T> const& member,
              char const* str) {
  t.*(member.i) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set a short within an object.
 */
template <typename T>
static void set_short(
              T& t,
              data_member<T> const& member,
              char const* str) {
  t.*(member.s) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set a string within an object.
 */
template <typename T>
static void set_string(
              T& t,
              data_member<T> const& member,
              char const* str) {
  t.*(member.S) = str;
  QString& s(t.*(member.S));
  for (int i(0), size(s.size() - 1); i < size; ++i)
    if (s[i] == '\\') {
      if ('\\' == s[i + 1])
        s.replace(i, 2, "\\");
      else if ('n' == s[i + 1])
        s.replace(i, 2, "\n");
    }
  return ;
}

/**
 *  Set a time_t within an object.
 */
template <typename T>
static void set_timet(
              T& t,
              data_member<T> const& member,
              char const* str) {
  t.*(member.t) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set an unsigned integer within an object.
 */
template <typename T>
static void set_uint(
              T& t,
              data_member<T> const& member,
              char const* str) {
  t.*(member.u) = strtoul(str, NULL, 0);
  return ;
}

/**
 *  Static initialization template used by Initialize().
 */
template <typename T>
static void static_init() {
  for (unsigned int i = 0; mapped_type<T>::members[i].type; ++i)
    if (mapped_type<T>::members[i].id) {
      getter_setter<T>& gs(ndo_mapped_type<T>::map[mapped_type<T>::members[i].id]);
      gs.member = &mapped_type<T>::members[i].member;
      switch (mapped_type<T>::members[i].type) {
       case mapped_data<T>::BOOL:
        gs.getter = &get_boolean<T>;
        gs.setter = &set_boolean<T>;
        break ;
       case mapped_data<T>::DOUBLE:
        gs.getter = &get_double<T>;
        gs.setter = &set_double<T>;
        break ;
       case mapped_data<T>::INT:
        gs.getter = &get_integer<T>;
        gs.setter = &set_integer<T>;
        break ;
       case mapped_data<T>::SHORT:
        gs.getter = &get_short<T>;
        gs.setter = &set_short<T>;
        break ;
       case mapped_data<T>::STRING:
        gs.getter = &get_string<T>;
        gs.setter = &set_string<T>;
        break ;
       case mapped_data<T>::TIMESTAMP:
        gs.getter = &get_timet<T>;
        gs.setter = &set_timet<T>;
        break ;
       case mapped_data<T>::UINT:
        gs.getter = &get_uint<T>;
        gs.setter = &set_uint<T>;
        break ;
       default: // Error in one of the mappings.
        assert(false);
        abort();
      }
    }
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
      namespace ndo {
        template <> umap<int, getter_setter<correlation::engine_state> >
          ndo_mapped_type<correlation::engine_state>::map =
            umap<int, getter_setter<correlation::engine_state> >();
        template <> umap<int, getter_setter<correlation::host_state> >
          ndo_mapped_type<correlation::host_state>::map =
            umap<int, getter_setter<correlation::host_state> >();
        template <> umap<int, getter_setter<correlation::issue> >
          ndo_mapped_type<correlation::issue>::map =
            umap<int, getter_setter<correlation::issue> >();
        template <> umap<int, getter_setter<correlation::issue_parent> >
          ndo_mapped_type<correlation::issue_parent>::map =
          umap<int, getter_setter<correlation::issue_parent> >();
        template <> umap<int, getter_setter<correlation::service_state> >
          ndo_mapped_type<correlation::service_state>::map =
            umap<int, getter_setter<correlation::service_state> >();
        template <> umap<int, getter_setter<neb::acknowledgement> >
          ndo_mapped_type<neb::acknowledgement>::map =
            umap<int, getter_setter<neb::acknowledgement> >();
        template <> umap<int, getter_setter<neb::comment> >
          ndo_mapped_type<neb::comment>::map =
            umap<int, getter_setter<neb::comment> >();
        template <> umap<int, getter_setter<neb::custom_variable> >
          ndo_mapped_type<neb::custom_variable>::map =
            umap<int, getter_setter<neb::custom_variable> >();
        template <> umap<int, getter_setter<neb::custom_variable_status> >
          ndo_mapped_type<neb::custom_variable_status>::map =
            umap<int, getter_setter<neb::custom_variable_status> >();
        template <> umap<int, getter_setter<neb::downtime> >
          ndo_mapped_type<neb::downtime>::map =
            umap<int, getter_setter<neb::downtime> >();
        template <> umap<int, getter_setter<neb::event_handler> >
          ndo_mapped_type<neb::event_handler>::map =
            umap<int, getter_setter<neb::event_handler> >();
        template <> umap<int, getter_setter<neb::flapping_status> >
          ndo_mapped_type<neb::flapping_status>::map =
            umap<int, getter_setter<neb::flapping_status> >();
        template <> umap<int, getter_setter<neb::host> >
          ndo_mapped_type<neb::host>::map =
            umap<int, getter_setter<neb::host> >();
        template <> umap<int, getter_setter<neb::host_check> >
          ndo_mapped_type<neb::host_check>::map =
            umap<int, getter_setter<neb::host_check> >();
        template <> umap<int, getter_setter<neb::host_dependency> >
          ndo_mapped_type<neb::host_dependency>::map =
            umap<int, getter_setter<neb::host_dependency> >();
        template <> umap<int, getter_setter<neb::host_group> >
          ndo_mapped_type<neb::host_group>::map =
            umap<int, getter_setter<neb::host_group> >();
        template <> umap<int, getter_setter<neb::host_group_member> >
          ndo_mapped_type<neb::host_group_member>::map =
            umap<int, getter_setter<neb::host_group_member> >();
        template <> umap<int, getter_setter<neb::host_parent> >
          ndo_mapped_type<neb::host_parent>::map =
            umap<int, getter_setter<neb::host_parent> >();
        template <> umap<int, getter_setter<neb::host_status> >
          ndo_mapped_type<neb::host_status>::map =
            umap<int, getter_setter<neb::host_status> >();
        template <> umap<int, getter_setter<neb::instance> >
          ndo_mapped_type<neb::instance>::map =
            umap<int, getter_setter<neb::instance> >();
        template <> umap<int, getter_setter<neb::instance_status> >
          ndo_mapped_type<neb::instance_status>::map =
            umap<int, getter_setter<neb::instance_status> >();
        template <> umap<int, getter_setter<neb::log_entry> >
          ndo_mapped_type<neb::log_entry>::map =
            umap<int, getter_setter<neb::log_entry> >();
        template <> umap<int, getter_setter<neb::module> >
          ndo_mapped_type<neb::module>::map =
            umap<int, getter_setter<neb::module> >();
        template <> umap<int, getter_setter<neb::notification> >
          ndo_mapped_type<neb::notification>::map =
            umap<int, getter_setter<neb::notification> >();
        template <> umap<int, getter_setter<neb::service> >
          ndo_mapped_type<neb::service>::map =
            umap<int, getter_setter<neb::service> >();
        template <> umap<int, getter_setter<neb::service_check> >
          ndo_mapped_type<neb::service_check>::map =
            umap<int, getter_setter<neb::service_check> >();
        template <> umap<int, getter_setter<neb::service_dependency> >
          ndo_mapped_type<neb::service_dependency>::map =
            umap<int, getter_setter<neb::service_dependency> >();
        template <> umap<int, getter_setter<neb::service_group> >
          ndo_mapped_type<neb::service_group>::map =
            umap<int, getter_setter<neb::service_group> >();
        template <> umap<int, getter_setter<neb::service_group_member> >
          ndo_mapped_type<neb::service_group_member>::map =
            umap<int, getter_setter<neb::service_group_member> >();
        template <> umap<int, getter_setter<neb::service_status> >
          ndo_mapped_type<neb::service_status>::map =
            umap<int, getter_setter<neb::service_status> >();
        template <> umap<int, getter_setter<storage::metric> >
          ndo_mapped_type<storage::metric>::map =
            umap<int, getter_setter<storage::metric> >();
        template <> umap<int, getter_setter<storage::rebuild> >
          ndo_mapped_type<storage::rebuild>::map =
            umap<int, getter_setter<storage::rebuild> >();
        template <> umap<int, getter_setter<storage::remove_graph> >
          ndo_mapped_type<storage::remove_graph>::map =
            umap<int, getter_setter<storage::remove_graph> >();
        template <> umap<int, getter_setter<storage::status> >
          ndo_mapped_type<storage::status>::map =
            umap<int, getter_setter<storage::status> >();
        template <> umap<int, getter_setter<bam::ba_status> >
          ndo_mapped_type<bam::ba_status>::map =
            umap<int, getter_setter<bam::ba_status> >();
        template <> umap<int, getter_setter<bam::bool_status> >
          ndo_mapped_type<bam::bool_status>::map =
            umap<int, getter_setter<bam::bool_status> >();
        template <> umap<int, getter_setter<bam::kpi_status> >
          ndo_mapped_type<bam::kpi_status>::map =
            umap<int, getter_setter<bam::kpi_status> >();
        template <> umap<int, getter_setter<bam::meta_service_status> >
          ndo_mapped_type<bam::meta_service_status>::map =
            umap<int, getter_setter<bam::meta_service_status> >();
        template <> umap<int, getter_setter<bam::ba_event> >
          ndo_mapped_type<bam::ba_event>::map =
            umap<int, getter_setter<bam::ba_event> >();
        template <> umap<int, getter_setter<bam::kpi_event> >
          ndo_mapped_type<bam::kpi_event>::map =
            umap<int, getter_setter<bam::kpi_event> >();
        template <> umap<int, getter_setter<bam::ba_duration_event> >
          ndo_mapped_type<bam::ba_duration_event>::map =
            umap<int, getter_setter<bam::ba_duration_event> >();
        template <> umap<int, getter_setter<bam::dimension_ba_event> >
          ndo_mapped_type<bam::dimension_ba_event>::map =
            umap<int, getter_setter<bam::dimension_ba_event> >();
        template <> umap<int, getter_setter<bam::dimension_kpi_event> >
          ndo_mapped_type<bam::dimension_kpi_event>::map =
            umap<int, getter_setter<bam::dimension_kpi_event> >();
        template <> umap<int, getter_setter<bam::dimension_ba_bv_relation_event> >
          ndo_mapped_type<bam::dimension_ba_bv_relation_event>::map =
            umap<int, getter_setter<bam::dimension_ba_bv_relation_event> >();
        template <> umap<int, getter_setter<bam::dimension_bv_event> >
          ndo_mapped_type<bam::dimension_bv_event>::map =
            umap<int, getter_setter<bam::dimension_bv_event> >();
        template <> umap<int, getter_setter<bam::dimension_truncate_table_signal> >
          ndo_mapped_type<bam::dimension_truncate_table_signal>::map =
            umap<int, getter_setter<bam::dimension_truncate_table_signal> >();
        template <> umap<int, getter_setter<bam::rebuild> >
          ndo_mapped_type<bam::rebuild>::map =
            umap<int, getter_setter<bam::rebuild> >();
        template <> umap<int, getter_setter<bam::dimension_timeperiod> >
          ndo_mapped_type<bam::dimension_timeperiod>::map =
            umap<int, getter_setter<bam::dimension_timeperiod> >();
        template <> umap<int, getter_setter<bam::dimension_ba_timeperiod_relation> >
          ndo_mapped_type<bam::dimension_ba_timeperiod_relation>::map =
            umap<int, getter_setter<bam::dimension_ba_timeperiod_relation> >();
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
 *  Initialize NDO mappings.
 */
void ndo::initialize() {
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
  static_init<storage::metric>();
  static_init<storage::rebuild>();
  static_init<storage::remove_graph>();
  static_init<storage::status>();
  static_init<correlation::engine_state>();
  static_init<correlation::host_state>();
  static_init<correlation::issue>();
  static_init<correlation::issue_parent>();
  static_init<correlation::service_state>();
  static_init<bam::ba_status>();
  static_init<bam::bool_status>();
  static_init<bam::kpi_status>();
  static_init<bam::meta_service_status>();
  static_init<bam::ba_event>();
  static_init<bam::kpi_event>();
  static_init<bam::ba_duration_event>();
  static_init<bam::dimension_ba_event>();
  static_init<bam::dimension_kpi_event>();
  static_init<bam::dimension_ba_bv_relation_event>();
  static_init<bam::dimension_bv_event>();
  static_init<bam::dimension_truncate_table_signal>();
  static_init<bam::rebuild>();
  static_init<bam::dimension_timeperiod>();
  static_init<bam::dimension_ba_timeperiod_relation>();
  return ;
}
