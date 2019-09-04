/*
** Copyright 2013-2017 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <stdint.h>
#include "com/centreon/broker/bbdo/ack.hh"
#include "com/centreon/broker/bbdo/input.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/timeout.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/misc/misc.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

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
           << "no terminating '\\0' in remaining " << size
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
 *  Unserialize an event in the BBDO protocol.
 *
 *  @param[in] event_type  Event type.
 *  @param[in] source_id   The source id.
 *  @param[in] destination The destination id.
 *  @param[in] buffer      Serialized data.
 *  @param[in] size        Buffer size.
 *
 *  @return Event.
 */
static io::data* unserialize(
                   unsigned int event_type,
                   unsigned int source_id,
                   unsigned int destination_id,
                   char const* buffer,
                   unsigned int size) {
  // Get event info (operations and mapping).
  io::event_info const*
    info(io::events::instance().get_event_info(event_type));
  if (info) {
    // Create object.
    std::unique_ptr<io::data> t(info->get_operations().constructor());
    if (t.get()) {
      t->source_id = source_id;
      t->destination_id = destination_id;
      // Browse all mapping to unserialize the object.
      for (mapping::entry const* current_entry(info->get_mapping());
           !current_entry->is_null();
           ++current_entry)
        // Skip entries that should not be serialized.
        if (current_entry->get_serialize()) {
          unsigned int rb;
          switch (current_entry->get_type()) {
          case mapping::source::BOOL:
            rb = set_boolean(*t, *current_entry, buffer, size);
            break ;
          case mapping::source::DOUBLE:
            rb = set_double(*t, *current_entry, buffer, size);
            break ;
          case mapping::source::INT:
            rb = set_integer(*t, *current_entry, buffer, size);
            break ;
          case mapping::source::SHORT:
            rb = set_short(*t, *current_entry, buffer, size);
            break ;
          case mapping::source::STRING:
            rb = set_string(*t, *current_entry, buffer, size);
            break ;
          case mapping::source::TIME:
            rb = set_timestamp(*t, *current_entry, buffer, size);
            break ;
          case mapping::source::UINT:
            rb = set_uint(*t, *current_entry, buffer, size);
            break ;
          default:
            throw (exceptions::msg() << "BBDO: invalid mapping for "
                   << "object of type '" << info->get_name() << "': "
                   << current_entry->get_type()
                   << " is not a known type ID");
          }
          buffer += rb;
          size -= rb;
        }
      return (t.release());
      }
      else
        throw (exceptions::msg() << "BBDO: cannot create object of ID "
               << event_type << " whereas it has been registered");
    }
  else
    logging::info(logging::high)
      << "BBDO: cannot unserialize event of ID " << event_type
      << ": event was not registered and will therefore be ignored";

  return (NULL);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
input::input() : _skipped(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
input::input(input const& other)
  : io::stream(other),
    _buffer(other._buffer),
    _skipped(other._skipped) {}

/**
 *  Destructor.
 */
input::~input() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
input& input::operator=(input const& other) {
  if (this != &other) {
    _buffer = other._buffer;
    _skipped = other._skipped;
  }
  return (*this);
}

/**
 *  @brief Get the next available event.
 *
 *  Extract the next available event on the input stream, NULL if the
 *  stream is closed.
 *
 *  @param[out] d         Next available event.
 *  @param[in]  deadline  Timeout in seconds.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool input::read(std::shared_ptr<io::data>& d, time_t deadline) {
  // Read event.
  d.reset();
  bool timed_out(!read_any(d, deadline));
  unsigned int event_id(!d ? 0 : d->type());
  while (!timed_out
         && ((event_id >> 16) == io::events::bbdo)) {
    // Version response.
    if ((event_id & 0xFFFF) == 1) {
      std::shared_ptr<version_response>
        version(std::static_pointer_cast<version_response>(d));
      if (version->bbdo_major != BBDO_VERSION_MAJOR)
        throw (exceptions::msg()
               << "BBDO: peer is using protocol version "
               << version->bbdo_major << "." << version->bbdo_minor
               << "." << version->bbdo_patch
               << " whereas we're using protocol version "
               << BBDO_VERSION_MAJOR << "." << BBDO_VERSION_MINOR << "."
               << BBDO_VERSION_PATCH);
      logging::info(logging::medium)
        << "BBDO: peer is using protocol version " << version->bbdo_major
        << "." << version->bbdo_minor << "." << version->bbdo_patch
        << ", we're using version " << BBDO_VERSION_MAJOR << "."
        << BBDO_VERSION_MINOR << "." << BBDO_VERSION_PATCH;
    }
    else if ((event_id & 0xFFFF) == 2) {
      logging::info(logging::medium)
        << "BBDO: received acknowledgement for "
        << std::static_pointer_cast<ack const>(d)->acknowledged_events << " events";
      acknowledge_events(std::static_pointer_cast<ack const>(d)->acknowledged_events);
    }

    // Control messages.
    logging::debug(logging::medium) << "BBDO: event with ID "
      << event_id << " was a control message, launching recursive read";
    timed_out = !read_any(d, deadline);
    event_id = !d ? 0 : d->type();
  }
  return (!timed_out);
}

/**
 *  @brief Get the next available event.
 *
 *  Extract the next available event on the input stream, NULL if the
 *  stream is closed.
 *
 *  @param[out] d         Next available event.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool input::read_any(
              std::shared_ptr<io::data>& d,
              time_t deadline) {
  try {
    // Return value.
    std::unique_ptr<io::data> e;
    d.reset();

    // Get header informations.
    unsigned int event_id(0);
    int packet_size;
    unsigned int source_id;
    unsigned int destination_id;
    std::string packet;
    int raw_size(0);
    do {
      // Extract header.
      std::string header;
      _buffer_must_have_unprocessed(
        raw_size + BBDO_HEADER_SIZE,
        deadline);
      _buffer.extract(header, raw_size, BBDO_HEADER_SIZE);

      // Extract header info.
      uint16_t chksum{ntohs(*reinterpret_cast<uint16_t const*>(header.data()))};
      packet_size =
          ntohs(*reinterpret_cast<uint16_t const*>(header.data() + 2));
      uint32_t current_event_id{
          ntohl(*reinterpret_cast<uint32_t const*>(header.data() + 4))};
      uint32_t current_source_id{
          ntohl(*reinterpret_cast<uint32_t const*>(header.data() + 8))};
      uint32_t current_dest_id{
          ntohl(*reinterpret_cast<uint32_t const*>(header.data() + 12))};
      uint16_t expected{
          misc::crc16_ccitt(header.data() + 2, BBDO_HEADER_SIZE - 2)};

      // Initial packet, extract info.
      if (!event_id) {
        event_id = current_event_id;
        source_id = current_source_id;
        destination_id = current_dest_id;
      }

      // Checksum and for multi-packet, assert same event.
      if (chksum != expected
          || event_id != current_event_id
          || source_id != current_source_id
          || destination_id != current_dest_id) {
        if (!_skipped) // First corrupted byte.
          logging::error(logging::high) << "BBDO: peer " << peer()
            << " is sending corrupted data: "
            << ((chksum != expected)
                ? "invalid CRC"
                : "invalid multi-packet event");
        ++_skipped;
        _buffer.erase(1);
        event_id = 0;
        packet.clear();
        raw_size = 0;
        packet_size = 0xFFFF; // Keep the loop running.
      }
      // All good, extract packet payload.
      else {
        _buffer_must_have_unprocessed(
           raw_size + BBDO_HEADER_SIZE + packet_size,
           deadline);
        _buffer.extract(packet, raw_size + BBDO_HEADER_SIZE, packet_size);
        raw_size += BBDO_HEADER_SIZE + packet_size;
      }
    } while (packet_size == 0xFFFF);

    // We now have a complete packet, print summary of corruption.
    if (_skipped) {
      logging::info(logging::high) << "BBDO: peer " << peer()
        << " sent " << _skipped
        << " corrupted payload bytes, resuming processing";
      _skipped = 0;
    }

    // Unserialize event.
    d.reset(unserialize(
          event_id,
          source_id,
          destination_id,
          packet.data(),
          packet.size()));
    if (d)
      logging::debug(logging::medium) << "BBDO: unserialized "
        << raw_size << " bytes for event of type " << event_id;
    else {
      logging::error(logging::medium)
        << "BBDO: unknown event type " << event_id
        << ": event cannot be decoded";
      logging::debug(logging::medium) << "BBDO: discarded "
        << raw_size << " bytes";
    }

    // Mark data as processed.
    _buffer.erase(raw_size);

    return (true);
  }
  catch (exceptions::timeout const& e) {
    (void)e;
    return (false);
  }
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Expect buffer to have a minimal size.
 *
 *  @param[in] bytes     Number of minimal buffer size.
 *  @param[in] deadline  Timeout in seconds.
 */
void input::_buffer_must_have_unprocessed(int bytes, time_t deadline) {
  // Read as much data as requested.
  bool timed_out(false);
  while (!timed_out && (_buffer.size() < bytes)) {
    std::shared_ptr<io::data> d;
    timed_out = !_substream->read(d, deadline);
    if (d && d->type() == io::raw::static_type())
      _buffer.append(std::static_pointer_cast<io::raw>(d));
  }
  if (timed_out)
    throw (exceptions::timeout());
  return ;
}
