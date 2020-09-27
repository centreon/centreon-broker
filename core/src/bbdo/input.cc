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

#include "com/centreon/broker/bbdo/input.hh"

#include <arpa/inet.h>
#include <stdint.h>

#include <cstdlib>
#include <cstring>
#include <memory>
#include <thread>

#include "com/centreon/broker/bbdo/ack.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/timeout.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/log_v2.hh"
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
static uint32_t set_boolean(io::data& t,
                            mapping::entry const& member,
                            void const* data,
                            uint32_t size) {
  if (!size) {
    log_v2::bbdo()->error(
        "cannot extract boolean value: 0 bytes left in "
        "packet");
    throw exceptions::msg() << "cannot extract boolean value: "
                            << "0 bytes left in packet";
  }
  member.set_bool(t, *static_cast<char const*>(data));
  return 1;
}

/**
 *  Set a double within an object.
 */
static uint32_t set_double(io::data& t,
                           mapping::entry const& member,
                           void const* data,
                           uint32_t size) {
  char const* str(static_cast<char const*>(data));
  uint32_t len(strlen(str));
  if (len >= size) {
    log_v2::bbdo()->error(
        "cannot extract double value: not terminating '\0' in remaining "
        "{} bytes of packet",
        size);
    throw exceptions::msg()
        << "cannot extract double value: "
        << "not terminating '\0' in remaining " << size << " bytes of packet";
  }
  member.set_double(t, strtod(str, nullptr));
  return len + 1;
}

/**
 *  Set an integer within an object.
 */
static uint32_t set_integer(io::data& t,
                            mapping::entry const& member,
                            void const* data,
                            uint32_t size) {
  if (size < sizeof(uint32_t)) {
    log_v2::bbdo()->error(
        "cannot extract integer value: {} bytes left in packet", size);
    throw exceptions::msg() << "BBDO: cannot extract integer value: " << size
                            << " bytes left in packet";
  }
  member.set_int(t, ntohl(*static_cast<uint32_t const*>(data)));
  return sizeof(uint32_t);
}

/**
 *  Set a short within an object.
 */
static uint32_t set_short(io::data& t,
                          mapping::entry const& member,
                          void const* data,
                          uint32_t size) {
  if (size < sizeof(uint16_t)) {
    log_v2::bbdo()->error(
        "BBDO: cannot extract short value: {} bytes left in packet", size);
    throw exceptions::msg() << "BBDO: cannot extract short value: " << size
                            << " bytes left in packet";
  }
  member.set_short(t, ntohs(*static_cast<uint16_t const*>(data)));
  return sizeof(uint16_t);
}

/**
 *  Set a string within an object.
 */
static uint32_t set_string(io::data& t,
                           mapping::entry const& member,
                           void const* data,
                           uint32_t size) {
  char const* str(static_cast<char const*>(data));
  uint32_t len(strlen(str));
  if (len >= size) {
    log_v2::bbdo()->error(
        "BBDO: cannot extract string value: no terminating '\\0' in remaining "
        "{} bytes left in packet",
        size);

    throw exceptions::msg()
        << "BBDO: cannot extract string value: "
        << "no terminating '\\0' in remaining " << size << " bytes of packet";
  }
  member.set_string(t, str);
  return len + 1;
}

/**
 *  Set a timestamp within an object.
 */
static uint32_t set_timestamp(io::data& t,
                              mapping::entry const& member,
                              void const* data,
                              uint32_t size) {
  if (size < 2 * sizeof(uint32_t)) {
    log_v2::bbdo()->error(
        "BBDO: cannot extract timestamp value: {} bytes left in packet", size);
    throw exceptions::msg() << "BBDO: cannot extract timestamp value: " << size
                            << " bytes left in packet";
  }
  uint32_t const* ptr(static_cast<uint32_t const*>(data));
  uint64_t val(ntohl(*ptr));
  ++ptr;
  val <<= 32;
  val |= ntohl(*ptr);
  member.set_time(t, val);
  return 2 * sizeof(uint32_t);
}

/**
 *  Set an uint32_teger within an object.
 */
static uint32_t set_uint(io::data& t,
                         mapping::entry const& member,
                         void const* data,
                         uint32_t size) {
  if (size < sizeof(uint32_t)) {
    log_v2::bbdo()->error(
        "BBDO: cannot extract uint32_t integer value: {} bytes left in packet",
        size);
    throw exceptions::msg()
        << "BBDO: cannot extract uint32_teger value: " << size
        << " bytes left in packet";
  }
  member.set_uint(t, ntohl(*static_cast<uint32_t const*>(data)));
  return sizeof(uint32_t);
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
static io::data* unserialize(uint32_t event_type,
                             uint32_t source_id,
                             uint32_t destination_id,
                             char const* buffer,
                             uint32_t size) {
  // Get event info (operations and mapping).
  io::event_info const* info(io::events::instance().get_event_info(event_type));
  if (info) {
    // Create object.
    std::unique_ptr<io::data> t(info->get_operations().constructor());
    if (t) {
      t->source_id = source_id;
      t->destination_id = destination_id;
      // Browse all mapping to unserialize the object.
      for (mapping::entry const* current_entry(info->get_mapping());
           !current_entry->is_null(); ++current_entry)
        // Skip entries that should not be serialized.
        if (current_entry->get_serialize()) {
          uint32_t rb;
          switch (current_entry->get_type()) {
            case mapping::source::BOOL:
              rb = set_boolean(*t, *current_entry, buffer, size);
              break;
            case mapping::source::DOUBLE:
              rb = set_double(*t, *current_entry, buffer, size);
              break;
            case mapping::source::INT:
              rb = set_integer(*t, *current_entry, buffer, size);
              break;
            case mapping::source::SHORT:
              rb = set_short(*t, *current_entry, buffer, size);
              break;
            case mapping::source::STRING:
              rb = set_string(*t, *current_entry, buffer, size);
              break;
            case mapping::source::TIME:
              rb = set_timestamp(*t, *current_entry, buffer, size);
              break;
            case mapping::source::UINT:
              rb = set_uint(*t, *current_entry, buffer, size);
              break;
            default:
              log_v2::bbdo()->error(
                  "BBDO: invalid mapping for object of type '{0}': {1} is not "
                  "a known type ID",
                  info->get_name(), current_entry->get_type());
              throw exceptions::msg() << "BBDO: invalid mapping for "
                                      << "object of type '" << info->get_name()
                                      << "': " << current_entry->get_type()
                                      << " is not a known type ID";
          }
          buffer += rb;
          size -= rb;
        }
      return t.release();
    } else {
      log_v2::bbdo()->error(
          "BBDO: cannot create object of ID {} whereas it has been registered",
          event_type);
      throw exceptions::msg()
          << "BBDO: cannot create object of ID " << event_type
          << " whereas it has been registered";
    }
  } else {
    log_v2::bbdo()->info(
        "BBDO: cannot unserialize event of ID {}: event was not registered and "
        "will therefore be ignored",
        event_type);
    logging::info(logging::high)
        << "BBDO: cannot unserialize event of ID " << event_type
        << ": event was not registered and will therefore be ignored";
  }

  return nullptr;
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
 *  Destructor.
 */
input::~input() {}

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

  /* This lock is needed because the same stream is used by several threads.
   * In the case of long events (split in several parts), if two threads read
   * at the same time, the second one could get a part of the long event... */
  bool timed_out(!read_any(d, deadline));
  uint32_t event_id(!d ? 0 : d->type());
  while (!timed_out && ((event_id >> 16) == io::events::bbdo)) {
    // Version response.
    if ((event_id & 0xFFFF) == 1) {
      std::shared_ptr<version_response> version(
          std::static_pointer_cast<version_response>(d));
      if (version->bbdo_major != BBDO_VERSION_MAJOR) {
        log_v2::bbdo()->error(
            "BBDO: peer is using protocol version {0}.{1}.{2} , whereas we're "
            "using protocol version "
            "{3}.{4}.{5}",
            version->bbdo_major, version->bbdo_minor, version->bbdo_patch,
            BBDO_VERSION_MAJOR, BBDO_VERSION_MINOR, BBDO_VERSION_PATCH);
        throw exceptions::msg()
            << "BBDO: peer is using protocol version " << version->bbdo_major
            << "." << version->bbdo_minor << "." << version->bbdo_patch
            << " whereas we're using protocol version " << BBDO_VERSION_MAJOR
            << "." << BBDO_VERSION_MINOR << "." << BBDO_VERSION_PATCH;
      }
      log_v2::bbdo()->info(
          "BBDO: peer is using protocol version {0}.{1}.{2} , we're using "
          "version "
          "{3}.{4}.{5}",
          version->bbdo_major, version->bbdo_minor, version->bbdo_patch,
          BBDO_VERSION_MAJOR, BBDO_VERSION_MINOR, BBDO_VERSION_PATCH);
      logging::info(logging::medium)
          << "BBDO: peer is using protocol version " << version->bbdo_major
          << "." << version->bbdo_minor << "." << version->bbdo_patch
          << ", we're using version " << BBDO_VERSION_MAJOR << "."
          << BBDO_VERSION_MINOR << "." << BBDO_VERSION_PATCH;
    } else if ((event_id & 0xFFFF) == 2) {
      log_v2::bbdo()->info(
          "BBDO: received acknowledgement for {} events",
          std::static_pointer_cast<ack const>(d)->acknowledged_events);
      logging::info(logging::medium)
          << "BBDO: received acknowledgement for "
          << std::static_pointer_cast<ack const>(d)->acknowledged_events
          << " events";
      acknowledge_events(
          std::static_pointer_cast<ack const>(d)->acknowledged_events);
    }

    // Control messages.
    log_v2::bbdo()->debug(
        "BBDO: event with ID {} was a control message, launching recursive "
        "read",
        event_id);
    logging::debug(logging::medium)
        << "BBDO: event with ID " << event_id
        << " was a control message, launching recursive read";
    timed_out = !read_any(d, deadline);
    event_id = !d ? 0 : d->type();
  }
  return !timed_out;
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
bool input::read_any(std::shared_ptr<io::data>& d, time_t deadline) {
  try {
    // Return value.
    std::unique_ptr<io::data> e;
    d.reset();

    for (;;) {
      /* Maybe we have to complete the header. */
      _read_packet(BBDO_HEADER_SIZE, deadline);

      // Packet size is now at least BBDO_HEADER_SIZE and maybe contains
      // already a full BBDO packet.

      const char* pack = _packet.data();
      uint16_t chksum = ntohs(*reinterpret_cast<uint16_t const*>(pack));
      uint32_t packet_size =
          ntohs(*reinterpret_cast<uint16_t const*>(pack + 2));
      uint32_t event_id = ntohl(*reinterpret_cast<uint32_t const*>(pack + 4));
      uint32_t source_id = ntohl(*reinterpret_cast<uint32_t const*>(pack + 8));
      uint32_t dest_id = ntohl(*reinterpret_cast<uint32_t const*>(pack + 12));
      uint16_t expected = misc::crc16_ccitt(pack + 2, BBDO_HEADER_SIZE - 2);

      // FIXME DBR: We must check correctly this error.
      log_v2::bbdo()->trace(
          "Reading: header eventID {} sourceID {} destID {} checksum {:x} and "
          "expected {:x}",
          event_id, source_id, dest_id, chksum, expected);

      if (expected != chksum) {
        // The packet is corrupted.
        if (_skipped == 0) {
          // First corrupted byte.
          log_v2::bbdo()->error(
              "peer {} is sending corrupted data: invalid CRC: {:04x} != "
              "{:04x}",
              peer(), chksum, expected);
        }
        ++_skipped;
        _packet.erase(_packet.begin());
        continue;
      } else if (_skipped) {
        log_v2::bbdo()->info(
            "peer {} sent {} corrupted payload bytes, resuming processing",
            peer(), _skipped);
        _skipped = 0;
      }

      // It is time to finish to read the packet.

      _read_packet(BBDO_HEADER_SIZE + packet_size, deadline);
      // Now, _packet contains at least BBDO_HEADER_SIZE + packet_size bytes.

      std::vector<char> content;
      if (_packet.size() == BBDO_HEADER_SIZE + packet_size) {
        log_v2::bbdo()->trace(
            "packet matches header + content => extracting content");
        // We remove the header from the packet: FIXME DBR this is not
        // beautiful...

        content = std::vector<char>(_packet.begin() + BBDO_HEADER_SIZE,
                                    _packet.end());
        _packet.clear();
        // The size should be of only packet_size now.
      } else {
        /* we have _packet.size() > BBDO_HEADER_SIZE + packet_size */

        size_t previous_packet_size = _packet.size();
        // packet contains more than one BBDO packet...
        content =
            std::vector<char>(_packet.begin() + BBDO_HEADER_SIZE,
                              _packet.begin() + BBDO_HEADER_SIZE + packet_size);
        _packet.erase(_packet.begin(),
                      _packet.begin() + BBDO_HEADER_SIZE + packet_size);
        log_v2::bbdo()->trace(
            "packet longer than header + content => splitting the whole of "
            "size {} to content of size {} and remaining of size {}",
            previous_packet_size, content.size(), _packet.size());
      }

      if (packet_size != 0xffff) {
        // Cool we can work with it!

        // Is it the next part of an already known input buffer?
        for (auto it = _buffer.begin(); it != _buffer.end(); ++it) {
          auto& b = *it;
          if (b.matches(event_id, source_id, dest_id)) {
            // Good, we've found it.
            b.push_back(std::move(content));

            content = b.to_vector();
            _buffer.erase(it);
            break;
          }
        }
        /* There is no reason to have this but no one knows. */
        if (_buffer.size() > 0) {
          log_v2::bbdo()->error(
              "There are still {} long BBDO packets that cannot be sent, this "
              "maybe be due to a corrupted retention file.",
              _buffer.size());
          /* In case of too many long events stored in memory, we purge the
           * oldest ones. */
          while (_buffer.size() > 3) {
            log_v2::bbdo()->info(
                "One too old long event part of type {} removed from memory",
                _buffer.front().get_event_id());
            _buffer.pop_front();
          }
        }

        pack = content.data();

        // Maybe it is bigger now.
        packet_size = content.size();
        d.reset(unserialize(event_id, source_id, dest_id, pack, packet_size));
        if (d) {
          log_v2::bbdo()->debug("unserialized {} bytes for event of type {}",
                                BBDO_HEADER_SIZE + packet_size, event_id);
        } else {
          log_v2::bbdo()->error("unknown event type {} event cannot be decoded",
                                event_id);
          log_v2::bbdo()->debug("discarded {} bytes",
                                BBDO_HEADER_SIZE + packet_size);

          logging::error(logging::medium) << "unknown event type " << event_id
                                          << ": event cannot be decoded";
          logging::debug(logging::medium)
              << "discarded " << BBDO_HEADER_SIZE + packet_size << " bytes";
        }
        return true;
      } else {
        // Is it the next part of an already known input buffer?
        bool done = false;
        for (auto it = _buffer.begin(); it != _buffer.end(); ++it) {
          auto& b = *it;
          if (b.matches(event_id, source_id, dest_id)) {
            // Good, we've found it.
            b.push_back(std::move(content));
            content.clear();
            done = true;
            break;
          }
        }
        if (!done)
          _buffer.emplace_back(
              input::buffer(event_id, source_id, dest_id, std::move(content)));

        /* There is no reason to have this but no one knows. */
        if (_buffer.size() > 1) {
          log_v2::bbdo()->error(
              "There are {} long BBDO packets waiting for their missing parts "
              "in memory, this may be due to a corrupted retention file.",
              _buffer.size());
          /* In case of too many long events stored in memory, we purge the
           * oldest ones. */
          while (_buffer.size() > 4) {
            log_v2::bbdo()->info(
                "One too old long event part of type {} removed from memory",
                _buffer.front().get_event_id());
            _buffer.pop_front();
          }
        }
      }
    }
  } catch (const exceptions::timeout& e) {
    return false;
  }
  return false;
}

/**
 * @brief Fill the internal _packet vector until it reaches the given size. It
 * may be bigger. The deadline is the limit time after that an exception is
 * thrown. Even if an exception is thrown the vector may begin to be fill, it is
 * just not finished, and so no data are lost. Received packets are BBDO packets
 * or maybe pieces of BBDO packets, so we keep vectors as is because usually a
 * vector should just represent a packet.
 *
 * @param size The wanted final size
 * @param deadline A time_t.
 */
void input::_read_packet(size_t size, time_t deadline) {
  // Read as much data as requested.
  while (_packet.size() < size) {
    std::shared_ptr<io::data> d;
    bool timeout = !_substream->read(d, deadline);

    if (d && d->type() == io::raw::static_type()) {
      std::vector<char>& new_v = std::static_pointer_cast<io::raw>(d)->_buffer;
      if (!new_v.empty()) {
        if (_packet.size() == 0)
          _packet = std::move(new_v);
        else
          _packet.insert(_packet.end(), new_v.begin(), new_v.end());
      }
    }
    if (timeout)
      throw exceptions::timeout();
  }
}
