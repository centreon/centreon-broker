/*
** Copyright 2013,2015 Merethis
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
#include "com/centreon/broker/bbdo/factory.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/mapping/entry.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

// Global BBDO mapping.
umap<unsigned int, bbdo_mapped_type> bbdo::bbdo_mapping;

/**
 *  Get a boolean from an object.
 */
static void get_boolean(
              io::data const& t,
              mapping::entry const& member,
              QByteArray& buffer) {
  char c(member.get_bool(t) ? 1 : 0);
  buffer.append(&c, 1);
  return ;
}

/**
 *  Get a double from an object.
 */
static void get_double(
              io::data const& t,
              mapping::entry const& member,
              QByteArray& buffer) {
  char str[32];
  size_t strsz(snprintf(str, sizeof(str), "%f", member.get_double(t)) + 1);
  if (strsz > sizeof(str))
    strsz = sizeof(str);
  buffer.append(str, strsz);
  return ;
}

/**
 *  Get an integer from an object.
 */
static void get_integer(
              io::data const& t,
              mapping::entry const& member,
              QByteArray& buffer) {
  uint32_t value(htonl(member.get_int(t)));
  buffer.append(
           static_cast<char*>(static_cast<void*>(&value)),
           sizeof(value));
  return ;
}

/**
 *  Get a short from an object.
 */
static void get_short(
              io::data const& t,
              mapping::entry const& member,
              QByteArray& buffer) {
  uint16_t value(htons(member.get_short(t)));
  buffer.append(
           static_cast<char*>(static_cast<void*>(&value)),
           sizeof(value));
  return ;
}

/**
 *  Get a string from an object.
 */
static void get_string(
              io::data const& t,
              mapping::entry const& member,
              QByteArray& buffer) {
  QByteArray tmp(member.get_string(t).toUtf8());
  buffer.append(tmp.constData(), tmp.size() + 1);
  return ;
}

/**
 *  Get a timestamp from an object.
 */
static void get_timestamp(
              io::data const& t,
              mapping::entry const& member,
              QByteArray& buffer) {
  uint64_t ts(member.get_time(t).get_time_t());
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
static void get_uint(
              io::data const& t,
              mapping::entry const& member,
              QByteArray& buffer) {
  uint32_t value(htonl(member.get_uint(t)));
  buffer.append(
           static_cast<char*>(static_cast<void*>(&value)),
           sizeof(value));
  return ;
}

/**
 *  Set a boolean within an object.
 */
static unsigned int set_boolean(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  if (!size)
    throw (exceptions::msg() << "BBDO: cannot extract boolean value: "
           << "0 bytes left in packet");
  member.set_bool(t, *static_cast<char const*>(data));
  return (1);
}

/**
 *  Set a double within an object.
 */
static unsigned int set_double(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  char const* str(static_cast<char const*>(data));
  unsigned int len(strlen(str));
  if (len >= size)
    throw (exceptions::msg() << "BBDO: cannot extract double value: "
           << "not terminating '\0' in remaining " << size
           << " bytes of packet");
  member.set_double(t, strtod(str, NULL));
  return (len + 1);
}

/**
 *  Set an integer within an object.
 */
static unsigned int set_integer(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  if (size < sizeof(uint32_t))
    throw (exceptions::msg() << "BBDO: cannot extract integer value: "
           << size << " bytes left in packet");
  member.set_int(t, ntohl(*static_cast<uint32_t const*>(data)));
  return (sizeof(uint32_t));
}

/**
 *  Set a short within an object.
 */
static unsigned int set_short(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  if (size < sizeof(uint16_t))
    throw (exceptions::msg() << "BBDO: cannot extract short value: "
           << size << " bytes left in packet");
  member.set_short(t, ntohs(*static_cast<uint16_t const*>(data)));
  return (sizeof(uint16_t));
}

/**
 *  Set a string within an object.
 */
static unsigned int set_string(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  char const* str(static_cast<char const*>(data));
  unsigned int len(strlen(str));
  if (len >= size)
    throw (exceptions::msg() << "BBDO: cannot extract string value: "
           << "no terminating '\0' in remaining " << size
           << " bytes of packet");
  member.set_string(t, str);
  return (len + 1);
}

/**
 *  Set a timestamp within an object.
 */
static unsigned int set_timestamp(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  if (size < 2 * sizeof(uint32_t))
    throw (exceptions::msg()
           << "BBDO: cannot extract timestamp value: "
           << size << " bytes left in packet");
  uint32_t const* ptr(static_cast<uint32_t const*>(data));
  uint64_t val(ntohl(*ptr));
  ++ptr;
  val <<= 32;
  val |= ntohl(*ptr);
  member.set_time(t, val);
  return (2 * sizeof(uint32_t));
}

/**
 *  Set an unsigned integer within an object.
 */
static unsigned int set_uint(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  if (size < sizeof(uint32_t))
    throw (exceptions::msg()
           << "BBDO: cannot extract unsigned integer value: "
           << size << " bytes left in packet");
  member.set_uint(t, ntohl(*static_cast<uint32_t const*>(data)));
  return (sizeof(uint32_t));
}

/**
 *  Initialize type mapping for BBDO serialization.
 *
 *  @param[out] entries      BBDO mapping.
 *  @param[in]  mapped_type  Type information.
 */
static void init_mapping(
              std::vector<getter_setter>& entries,
              io::event_info const& mapped_type) {
  for (mapping::entry const* current_entry(mapped_type.get_mapping());
       current_entry->get_type() != mapping::source::UNKNOWN;
       ++current_entry)
    if (current_entry->get_number()) {
      entries.resize(entries.size() + 1);
      getter_setter& gs(entries.back());
      switch (current_entry->get_type()) {
      case mapping::source::BOOL:
        gs.getter = &get_boolean;
        gs.setter = &set_boolean;
        break ;
      case mapping::source::DOUBLE:
        gs.getter = &get_double;
        gs.setter = &set_double;
        break ;
      case mapping::source::INT:
        gs.getter = &get_integer;
        gs.setter = &set_integer;
        break ;
      case mapping::source::SHORT:
        gs.getter = &get_short;
        gs.setter = &set_short;
        break ;
      case mapping::source::STRING:
        gs.getter = &get_string;
        gs.setter = &set_string;
        break ;
      case mapping::source::TIME:
        gs.getter = &get_timestamp;
        gs.setter = &set_timestamp;
        break ;
      case mapping::source::UINT:
        gs.getter = &get_uint;
        gs.setter = &set_uint;
        break ;
      default:
        throw (exceptions::msg() << "BBDO: invalid mapping for object '"
               << mapped_type.get_name () << "': "
               << current_entry->get_type() << " is not a valid type ID");
      }
    }
  return ;
}

/**
 *  Create all the event mappings.
 */
void bbdo::create_mappings() {
  // Clear mappings.
  bbdo_mapping.clear();
  // Create mappings.
  for (io::events::categories_container::const_iterator
         itc(io::events::instance().begin()),
         endc(io::events::instance().end());
       itc != endc;
       ++itc) {
    for (io::events::events_container::const_iterator
           ite(itc->second.events.begin()),
           ende(itc->second.events.end());
         ite != ende;
         ++ite) {
      logging::debug(logging::low)
        << "BBDO: creating mapping for event " << ite->first;
      bbdo_mapped_type& m(bbdo_mapping[ite->first]);
      m.mapped_type = &ite->second;
      init_mapping(m.bbdo_entries, *m.mapped_type);
    }
  }

}

/**************************************
*                                     *
*           Global Objects            *
*                                     *
**************************************/

/**
 *  @brief BBDO initialization routine.
 *
 *  Initialize BBDO mappings and register BBDO protocol.
 */
void bbdo::load() {
  io::events& e(io::events::instance());

  // Register BBDO category.
  int bbdo_category(e.register_category("bbdo", io::events::bbdo));
  if (bbdo_category != io::events::bbdo) {
    e.unregister_category(bbdo_category);
    throw (exceptions::msg() << "BBDO: category " << io::events::bbdo
           << " is already registered whereas it should be "
           << "reserved for the bbdo core");
  }

  // Register BBDO events.
  e.register_event(
      io::events::bbdo,
      bbdo::de_version_response,
      io::event_info(
            "version_response",
            &version_response::operations,
            version_response::entries));

  // Create mappings.
  create_mappings();

  // Register BBDO protocol.
  io::protocols::instance().reg(
                              "BBDO",
                              bbdo::factory(),
                              7,
                              7);

  return ;
}

/**
 *  @brief BBDO cleanup routine.
 *
 *  Delete BBDO mappings and unregister BBDO protocol.
 */
void bbdo::unload() {
  // Unregister protocol.
  io::protocols::instance().unreg("BBDO");

  // Unregister category.
  io::events::instance().unregister_category(io::events::bbdo);

  // Clean mappings.
  bbdo_mapping.clear();

  return ;
}
