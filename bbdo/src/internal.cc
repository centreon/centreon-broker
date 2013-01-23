/*
** Copyright 2013 Merethis
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

#include <arpa/inet.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/correlation/engine_state.hh"
#include "com/centreon/broker/correlation/host_state.hh"
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/service_state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/rebuild.hh"
#include "com/centreon/broker/storage/remove_graph.hh"
#include "com/centreon/broker/storage/status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

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
              std::string& buffer) {
  char c(t.*(member.b) ? 1 : 0);
  buffer.append(&c, 1);
  return ;
}

/**
 *  Get a double from an object.
 */
template <typename T>
static void get_double(
              T const& t,
              data_member<T> const& member,
              std::string& buffer) {
  char str[32];
  snprintf(str, sizeof(str), "%f", t.*(member.d));
  size_t strsz(strlen(str));
  buffer.append(str, strsz);
  return ;
}

/**
 *  Get an integer from an object.
 */
template <typename T>
static void get_integer(
              T const& t,
              data_member<T> const& member,
              std::string& buffer) {
  uint32_t value(htonl(t.*(member.i)));
  buffer.append(
           static_cast<char*>(static_cast<void*>(&value)),
           sizeof(value));
  return ;
}

/**
 *  Get a short from an object.
 */
template <typename T>
static void get_short(
              T const& t,
              data_member<T> const& member,
              std::string& buffer) {
  uint16_t value(htons(t.*(member.s)));
  buffer.append(
           static_cast<char*>(static_cast<void*>(&value)),
           sizeof(value));
  return ;
}

/**
 *  Get a string from an object.
 */
template <typename T>
static void get_string(
              T const& t,
              data_member<T> const& member,
              std::string& buffer) {
  buffer.append(qPrintable(t.*(member.S)));
  char c('\0');
  buffer.append(&c, 1);
  return ;
}

/**
 *  Get a timestamp from an object.
 */
template <typename T>
static void get_timestamp(
              T const& t,
              data_member<T> const& member,
              std::string& buffer) {
  long long ts((t.*(member.t)).get_time_t());
  uint32_t high(htonl(ts / (1ll << 32)));
  uint32_t low(htonl(ts % (1ll << 32)));
  buffer.append(
           static_cast<char*>(static_cast<void*>(&high)),
           sizeof(high));
  buffer.append(
           static_cast<char*>(static_cast<void*>(&low)),
           sizeof(low));
  return ;
}

/**
 *  Get an unsigned integer from an object.
 */
template <typename T>
static void get_uint(
              T const& t,
              data_member<T> const& member,
              std::string& buffer) {
  uint32_t value(htonl(t.*(member.u)));
  buffer.append(
           static_cast<char*>(static_cast<void*>(&value)),
           sizeof(value));
  return ;
}

/**
 *  Set a boolean within an object.
 */
template <typename T>
static unsigned int set_boolean(
                      T& t,
                      data_member<T> const& member,
                      void const* data,
                      unsigned int size) {
  if (!size)
    throw (exceptions::msg() << "BBDO: cannot extract boolean value: "
           << "0 bytes left in packet");
  t.*(member.b) = *static_cast<char const*>(data);
  return (1);
}

/**
 *  Set a double within an object.
 */
template <typename T>
static unsigned int set_double(
                      T& t,
                      data_member<T> const& member,
                      void const* data,
                      unsigned int size) {
  char const* str(static_cast<char const*>(data));
  unsigned int len(strlen(str));
  if (len >= size)
    throw (exceptions::msg() << "BBDO: cannot extract double value: "
           << "not terminating '\0' in remaining " << size
           << " bytes of packet");
  t.*(member.d) = strtod(str, NULL);
  return (len + 1);
}

/**
 *  Set an integer within an object.
 */
template <typename T>
static unsigned int set_integer(
                      T& t,
                      data_member<T> const& member,
                      void const* data,
                      unsigned int size) {
  if (size < sizeof(uint32_t))
    throw (exceptions::msg() << "BBDO: cannot extract integer value: "
           << size << " bytes left in packet");
  t.*(member.i) = ntohl(*static_cast<uint32_t const*>(data));
  return (sizeof(uint32_t));
}

/**
 *  Set a short within an object.
 */
template <typename T>
static unsigned int set_short(
                      T& t,
                      data_member<T> const& member,
                      void const* data,
                      unsigned int size) {
  if (size < sizeof(uint16_t))
    throw (exceptions::msg() << "BBDO: cannot extract short value: "
           << size << " bytes left in packet");
  t.*(member.s) = ntohs(*static_cast<uint16_t const*>(data));
  return (sizeof(uint16_t));
}

/**
 *  Set a string within an object.
 */
template <typename T>
static unsigned int set_string(
                      T& t,
                      data_member<T> const& member,
                      void const* data,
                      unsigned int size) {
  char const* str(static_cast<char const*>(data));
  unsigned int len(strlen(str));
  if (len >= size)
    throw (exceptions::msg() << "BBDO: cannot extract string value: "
           << "no terminating '\0' in remaining " << size
           << " bytes of packet");
  t.*(member.S) = str;
  return (len + 1);
}

/**
 *  Set a timestamp within an object.
 */
template <typename T>
static unsigned int set_timestamp(
                      T& t,
                      data_member<T> const& member,
                      void const* data,
                      unsigned int size) {
  if (size < 2 * sizeof(uint32_t))
    throw (exceptions::msg()
           << "BBDO: cannot extract timestamp value: "
           << size << " bytes left in packet");
  uint32_t const* ptr(static_cast<uint32_t const*>(data));
  long long val(*ptr);
  ++ptr;
  val <<= 32;
  val |= *ptr;
  t.*(member.t) = val;
  return (2 * sizeof(uint32_t));
}

/**
 *  Set an unsigned integer within an object.
 */
template <typename T>
static unsigned int set_uint(
                      T& t,
                      data_member<T> const& member,
                      void const* data,
                      unsigned int size) {
  if (size < sizeof(uint32_t))
    throw (exceptions::msg()
           << "BBDO: cannot extract unsigned integer value: "
           << size << " bytes left in packet");
  t.*(member.u) = ntohl(*static_cast<uint32_t const*>(data));
  return (sizeof(uint32_t));
}

/**
 *  Static initialization template used by initialize().
 */
template <typename T>
static void static_init() {
  for (unsigned int i(0); mapped_type<T>::members[i].type; ++i)
    if (mapped_type<T>::members[i].id) {
      bbdo_mapped_type<T>::table.resize(bbdo_mapped_type<T>::table.size() + 1);
      getter_setter<T>& gs(bbdo_mapped_type<T>::table.back());
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
        gs.getter = &get_timestamp<T>;
        gs.setter = &set_timestamp<T>;
        break ;
      case mapped_data<T>::UINT:
        gs.getter = &get_uint<T>;
        gs.setter = &set_uint<T>;
        break ;
      default:
        assert(!"invalid object mapping");
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

CCB_BEGIN()

namespace bbdo {
  template <> std::vector<getter_setter<correlation::engine_state> >
    bbdo_mapped_type<correlation::engine_state>::table =
      std::vector<getter_setter<correlation::engine_state> >();
  template <> std::vector<getter_setter<correlation::host_state> >
    bbdo_mapped_type<correlation::host_state>::table =
      std::vector<getter_setter<correlation::host_state> >();
  template <> std::vector<getter_setter<correlation::issue> >
    bbdo_mapped_type<correlation::issue>::table =
      std::vector<getter_setter<correlation::issue> >();
  template <> std::vector<getter_setter<correlation::issue_parent> >
    bbdo_mapped_type<correlation::issue_parent>::table =
      std::vector<getter_setter<correlation::issue_parent> >();
  template <> std::vector<getter_setter<correlation::service_state> >
    bbdo_mapped_type<correlation::service_state>::table =
      std::vector<getter_setter<correlation::service_state> >();
  template <> std::vector<getter_setter<neb::acknowledgement> >
    bbdo_mapped_type<neb::acknowledgement>::table =
      std::vector<getter_setter<neb::acknowledgement> >();
  template <> std::vector<getter_setter<neb::comment> >
    bbdo_mapped_type<neb::comment>::table =
      std::vector<getter_setter<neb::comment> >();
  template <> std::vector<getter_setter<neb::custom_variable> >
    bbdo_mapped_type<neb::custom_variable>::table =
      std::vector<getter_setter<neb::custom_variable> >();
  template <> std::vector<getter_setter<neb::custom_variable_status> >
    bbdo_mapped_type<neb::custom_variable_status>::table =
      std::vector<getter_setter<neb::custom_variable_status> >();
  template <> std::vector<getter_setter<neb::downtime> >
    bbdo_mapped_type<neb::downtime>::table =
      std::vector<getter_setter<neb::downtime> >();
  template <> std::vector<getter_setter<neb::event_handler> >
    bbdo_mapped_type<neb::event_handler>::table =
      std::vector<getter_setter<neb::event_handler> >();
  template <> std::vector<getter_setter<neb::flapping_status> >
    bbdo_mapped_type<neb::flapping_status>::table =
      std::vector<getter_setter<neb::flapping_status> >();
  template <> std::vector<getter_setter<neb::host> >
    bbdo_mapped_type<neb::host>::table =
      std::vector<getter_setter<neb::host> >();
  template <> std::vector<getter_setter<neb::host_check> >
    bbdo_mapped_type<neb::host_check>::table =
      std::vector<getter_setter<neb::host_check> >();
  template <> std::vector<getter_setter<neb::host_dependency> >
    bbdo_mapped_type<neb::host_dependency>::table =
      std::vector<getter_setter<neb::host_dependency> >();
  template <> std::vector<getter_setter<neb::host_group> >
    bbdo_mapped_type<neb::host_group>::table =
      std::vector<getter_setter<neb::host_group> >();
  template <> std::vector<getter_setter<neb::host_group_member> >
    bbdo_mapped_type<neb::host_group_member>::table =
      std::vector<getter_setter<neb::host_group_member> >();
  template <> std::vector<getter_setter<neb::host_parent> >
    bbdo_mapped_type<neb::host_parent>::table =
      std::vector<getter_setter<neb::host_parent> >();
  template <> std::vector<getter_setter<neb::host_status> >
    bbdo_mapped_type<neb::host_status>::table =
      std::vector<getter_setter<neb::host_status> >();
  template <> std::vector<getter_setter<neb::instance> >
    bbdo_mapped_type<neb::instance>::table =
      std::vector<getter_setter<neb::instance> >();
  template <> std::vector<getter_setter<neb::instance_status> >
    bbdo_mapped_type<neb::instance_status>::table =
      std::vector<getter_setter<neb::instance_status> >();
  template <> std::vector<getter_setter<neb::log_entry> >
    bbdo_mapped_type<neb::log_entry>::table =
      std::vector<getter_setter<neb::log_entry> >();
  template <> std::vector<getter_setter<neb::module> >
    bbdo_mapped_type<neb::module>::table =
      std::vector<getter_setter<neb::module> >();
  template <> std::vector<getter_setter<neb::notification> >
    bbdo_mapped_type<neb::notification>::table =
      std::vector<getter_setter<neb::notification> >();
  template <> std::vector<getter_setter<neb::service> >
    bbdo_mapped_type<neb::service>::table =
      std::vector<getter_setter<neb::service> >();
  template <> std::vector<getter_setter<neb::service_check> >
    bbdo_mapped_type<neb::service_check>::table =
      std::vector<getter_setter<neb::service_check> >();
  template <> std::vector<getter_setter<neb::service_dependency> >
    bbdo_mapped_type<neb::service_dependency>::table =
      std::vector<getter_setter<neb::service_dependency> >();
  template <> std::vector<getter_setter<neb::service_group> >
    bbdo_mapped_type<neb::service_group>::table =
      std::vector<getter_setter<neb::service_group> >();
  template <> std::vector<getter_setter<neb::service_group_member> >
    bbdo_mapped_type<neb::service_group_member>::table =
      std::vector<getter_setter<neb::service_group_member> >();
  template <> std::vector<getter_setter<neb::service_status> >
    bbdo_mapped_type<neb::service_status>::table =
      std::vector<getter_setter<neb::service_status> >();
  template <> std::vector<getter_setter<storage::metric> >
    bbdo_mapped_type<storage::metric>::table =
      std::vector<getter_setter<storage::metric> >();
  template <> std::vector<getter_setter<storage::rebuild> >
    bbdo_mapped_type<storage::rebuild>::table =
      std::vector<getter_setter<storage::rebuild> >();
  template <> std::vector<getter_setter<storage::remove_graph> >
    bbdo_mapped_type<storage::remove_graph>::table =
      std::vector<getter_setter<storage::remove_graph> >();
  template <> std::vector<getter_setter<storage::status> >
    bbdo_mapped_type<storage::status>::table =
      std::vector<getter_setter<storage::status> >();
}

CCB_END()

/**
 *  @brief BBDO initialization routine.
 *
 *  Initialize BBDO mappings.
 */
void bbdo::initialize() {
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
