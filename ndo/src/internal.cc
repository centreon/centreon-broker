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

#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include "com/centreon/broker/correlation/engine_state.hh"
#include "com/centreon/broker/correlation/host_state.hh"
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/service_state.hh"
#include "com/centreon/broker/ndo/internal.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/rebuild.hh"
#include "com/centreon/broker/storage/remove_graph.hh"
#include "com/centreon/broker/storage/status.hh"

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
static void get_boolean(T const& t,
                        data_member<T> const& member,
                        std::stringstream& buffer) {
  buffer << (t.*(member.b) ? "1" : "0");
  return ;
}

/**
 *  Get a double from an object.
 */
template <typename T>
static void get_double(T const& t,
                       data_member<T> const& member,
                       std::stringstream& buffer) {
  buffer << std::fixed << t.*(member.d);
  return ;
}

/**
 *  Get an integer from an object.
 */
template <typename T>
static void get_integer(T const& t,
                        data_member<T> const& member,
                        std::stringstream& buffer) {
  buffer << t.*(member.i);
  return ;
}

/**
 *  Get a short from an object.
 */
template <typename T>
static void get_short(T const& t,
                      data_member<T> const& member,
                      std::stringstream& buffer) {
  buffer << t.*(member.s);
  return ;
}

/**
 *  Get a string from an object.
 */
template <typename T>
static void get_string(T const& t,
                       data_member<T> const& member,
                       std::stringstream& buffer) {
  buffer << (t.*(member.S)).toStdString();
  return ;
}

/**
 *  Get a time_t from an object.
 */
template <typename T>
static void get_timet(T const& t,
                      data_member<T> const& member,
                      std::stringstream& buffer) {
  buffer << t.*(member.t);
  return ;
}

/**
 *  Get an unsigned integer from an object.
 */
template <typename T>
static void get_uint(T const& t,
                     data_member<T> const& member,
                     std::stringstream& buffer) {
  buffer << t.*(member.u);
  return ;
}

/**
 *  Set a boolean within an object.
 */
template <typename T>
static void set_boolean(T& t,
                        data_member<T> const& member,
                        char const* str) {
  t.*(member.b) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set a double within an object.
 */
template <typename T>
static void set_double(T& t,
                       data_member<T> const& member,
                       char const* str) {
  t.*(member.d) = strtod(str, NULL);
  return ;
}

/**
 *  Set an integer within an object.
 */
template <typename T>
static void set_integer(T& t,
                        data_member<T> const& member,
                        char const* str) {
  t.*(member.i) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set a short within an object.
 */
template <typename T>
static void set_short(T& t,
                      data_member<T> const& member,
                      char const* str) {
  t.*(member.s) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set a string within an object.
 */
template <typename T>
static void set_string(T& t,
                       data_member<T> const& member,
                       char const* str) {
  t.*(member.S) = str;
  return ;
}

/**
 *  Set a time_t within an object.
 */
template <typename T>
static void set_timet(T& t,
                      data_member<T> const& member,
                      char const* str) {
  t.*(member.t) = strtol(str, NULL, 0);
  return ;
}

/**
 *  Set an unsigned integer within an object.
 */
template <typename T>
static void set_uint(T& t,
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
       case mapped_data<T>::TIME_T:
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
        template <> std::map<int, getter_setter<correlation::engine_state> >
          ndo_mapped_type<correlation::engine_state>::map =
            std::map<int, getter_setter<correlation::engine_state> >();
        template <> std::map<int, getter_setter<correlation::host_state> >
          ndo_mapped_type<correlation::host_state>::map =
            std::map<int, getter_setter<correlation::host_state> >();
        template <> std::map<int, getter_setter<correlation::issue> >
          ndo_mapped_type<correlation::issue>::map =
            std::map<int, getter_setter<correlation::issue> >();
        template <> std::map<int, getter_setter<correlation::issue_parent> >
          ndo_mapped_type<correlation::issue_parent>::map =
          std::map<int, getter_setter<correlation::issue_parent> >();
        template <> std::map<int, getter_setter<correlation::service_state> >
          ndo_mapped_type<correlation::service_state>::map =
            std::map<int, getter_setter<correlation::service_state> >();
        template <> std::map<int, getter_setter<neb::acknowledgement> >
          ndo_mapped_type<neb::acknowledgement>::map =
            std::map<int, getter_setter<neb::acknowledgement> >();
        template <> std::map<int, getter_setter<neb::comment> >
          ndo_mapped_type<neb::comment>::map =
            std::map<int, getter_setter<neb::comment> >();
        template <> std::map<int, getter_setter<neb::custom_variable> >
          ndo_mapped_type<neb::custom_variable>::map =
            std::map<int, getter_setter<neb::custom_variable> >();
        template <> std::map<int, getter_setter<neb::custom_variable_status> >
          ndo_mapped_type<neb::custom_variable_status>::map =
            std::map<int, getter_setter<neb::custom_variable_status> >();
        template <> std::map<int, getter_setter<neb::downtime> >
          ndo_mapped_type<neb::downtime>::map =
            std::map<int, getter_setter<neb::downtime> >();
        template <> std::map<int, getter_setter<neb::event_handler> >
          ndo_mapped_type<neb::event_handler>::map =
            std::map<int, getter_setter<neb::event_handler> >();
        template <> std::map<int, getter_setter<neb::flapping_status> >
          ndo_mapped_type<neb::flapping_status>::map =
            std::map<int, getter_setter<neb::flapping_status> >();
        template <> std::map<int, getter_setter<neb::host> >
          ndo_mapped_type<neb::host>::map =
            std::map<int, getter_setter<neb::host> >();
        template <> std::map<int, getter_setter<neb::host_check> >
          ndo_mapped_type<neb::host_check>::map =
            std::map<int, getter_setter<neb::host_check> >();
        template <> std::map<int, getter_setter<neb::host_dependency> >
          ndo_mapped_type<neb::host_dependency>::map =
            std::map<int, getter_setter<neb::host_dependency> >();
        template <> std::map<int, getter_setter<neb::host_group> >
          ndo_mapped_type<neb::host_group>::map =
            std::map<int, getter_setter<neb::host_group> >();
        template <> std::map<int, getter_setter<neb::host_group_member> >
          ndo_mapped_type<neb::host_group_member>::map =
            std::map<int, getter_setter<neb::host_group_member> >();
        template <> std::map<int, getter_setter<neb::host_parent> >
          ndo_mapped_type<neb::host_parent>::map =
            std::map<int, getter_setter<neb::host_parent> >();
        template <> std::map<int, getter_setter<neb::host_status> >
          ndo_mapped_type<neb::host_status>::map =
            std::map<int, getter_setter<neb::host_status> >();
        template <> std::map<int, getter_setter<neb::instance> >
          ndo_mapped_type<neb::instance>::map =
            std::map<int, getter_setter<neb::instance> >();
        template <> std::map<int, getter_setter<neb::instance_status> >
          ndo_mapped_type<neb::instance_status>::map =
            std::map<int, getter_setter<neb::instance_status> >();
        template <> std::map<int, getter_setter<neb::log_entry> >
          ndo_mapped_type<neb::log_entry>::map =
            std::map<int, getter_setter<neb::log_entry> >();
        template <> std::map<int, getter_setter<neb::module> >
          ndo_mapped_type<neb::module>::map =
            std::map<int, getter_setter<neb::module> >();
        template <> std::map<int, getter_setter<neb::notification> >
          ndo_mapped_type<neb::notification>::map =
            std::map<int, getter_setter<neb::notification> >();
        template <> std::map<int, getter_setter<neb::service> >
          ndo_mapped_type<neb::service>::map =
            std::map<int, getter_setter<neb::service> >();
        template <> std::map<int, getter_setter<neb::service_check> >
          ndo_mapped_type<neb::service_check>::map =
            std::map<int, getter_setter<neb::service_check> >();
        template <> std::map<int, getter_setter<neb::service_dependency> >
          ndo_mapped_type<neb::service_dependency>::map =
            std::map<int, getter_setter<neb::service_dependency> >();
        template <> std::map<int, getter_setter<neb::service_group> >
          ndo_mapped_type<neb::service_group>::map =
            std::map<int, getter_setter<neb::service_group> >();
        template <> std::map<int, getter_setter<neb::service_group_member> >
          ndo_mapped_type<neb::service_group_member>::map =
            std::map<int, getter_setter<neb::service_group_member> >();
        template <> std::map<int, getter_setter<neb::service_status> >
          ndo_mapped_type<neb::service_status>::map =
            std::map<int, getter_setter<neb::service_status> >();
        template <> std::map<int, getter_setter<storage::metric> >
          ndo_mapped_type<storage::metric>::map =
            std::map<int, getter_setter<storage::metric> >();
        template <> std::map<int, getter_setter<storage::rebuild> >
          ndo_mapped_type<storage::rebuild>::map =
            std::map<int, getter_setter<storage::rebuild> >();
        template <> std::map<int, getter_setter<storage::remove_graph> >
          ndo_mapped_type<storage::remove_graph>::map =
            std::map<int, getter_setter<storage::remove_graph> >();
        template <> std::map<int, getter_setter<storage::status> >
          ndo_mapped_type<storage::status>::map =
            std::map<int, getter_setter<storage::status> >();
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
  return ;
}
