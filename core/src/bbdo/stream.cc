/*
** Copyright 2013,2015,2017 Centreon
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

#include "com/centreon/broker/bbdo/stream.hh"

#include <arpa/inet.h>

#include <algorithm>
#include <cassert>

#include "com/centreon/broker/bbdo/ack.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/timeout.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/misc.hh"
#include "com/centreon/broker/misc/string.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
 *                                     *
 *      Input static functions         *
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
      for (const mapping::entry* current_entry = info->get_mapping();
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
 *      Output static functions        *
 *                                     *
 **************************************/

/**
 *  Get a boolean from an object.
 */
static void get_boolean(io::data const& t,
                        mapping::entry const& member,
                        std::vector<char>& buffer) {
  char c(member.get_bool(t) ? 1 : 0);
  buffer.push_back(c);
}

/**
 *  Get a double from an object.
 */
static void get_double(io::data const& t,
                       mapping::entry const& member,
                       std::vector<char>& buffer) {
  char str[32];
  size_t strsz(snprintf(str, sizeof(str), "%f", member.get_double(t)) + 1);
  if (strsz > sizeof(str))
    strsz = sizeof(str);
  std::copy(str, str + strsz, std::back_inserter(buffer));
}

/**
 *  Get an integer from an object.
 */
static void get_integer(io::data const& t,
                        mapping::entry const& member,
                        std::vector<char>& buffer) {
  uint32_t value(htonl(member.get_int(t)));
  char* v(reinterpret_cast<char*>(&value));
  std::copy(v, v + sizeof(value), std::back_inserter(buffer));
}

/**
 *  Get a short from an object.
 */
static void get_short(io::data const& t,
                      mapping::entry const& member,
                      std::vector<char>& buffer) {
  uint16_t value(htons(member.get_short(t)));
  char* v(reinterpret_cast<char*>(&value));
  std::copy(v, v + sizeof(value), std::back_inserter(buffer));
}

/**
 *  Get a string from an object.
 */
static void get_string(io::data const& t,
                       mapping::entry const& member,
                       std::vector<char>& buffer) {
  std::string const& tmp(member.get_string(t));
  std::copy(tmp.c_str(), tmp.c_str() + tmp.size() + 1,
            std::back_inserter(buffer));
}

/**
 *  Get a timestamp from an object.
 */
static void get_timestamp(io::data const& t,
                          mapping::entry const& member,
                          std::vector<char>& buffer) {
  uint64_t ts(member.get_time(t).get_time_t());
  uint32_t high{htonl(ts >> 32)};
  uint32_t low{htonl(ts & 0xffffffff)};
  char* vh{reinterpret_cast<char*>(&high)};
  char* vl{reinterpret_cast<char*>(&low)};
  std::copy(vh, vh + sizeof(high), std::back_inserter(buffer));
  std::copy(vl, vl + sizeof(low), std::back_inserter(buffer));
}

/**
 *  Get an uint32_teger from an object.
 */
static void get_uint(io::data const& t,
                     mapping::entry const& member,
                     std::vector<char>& buffer) {
  uint32_t value{htonl(member.get_uint(t))};
  char* v{reinterpret_cast<char*>(&value)};
  std::copy(v, v + sizeof(value), std::back_inserter(buffer));
}

/**
 *  Serialize an event in the BBDO protocol.
 *
 *  @param[in] e  Event to serialize.
 *
 *  @return Serialized event.
 */
static io::raw* serialize(const io::data& e) {
  std::deque<std::vector<char>> queue;

  // Get event info (mapping).
  const io::event_info* info = io::events::instance().get_event_info(e.type());
  if (info) {
    // Serialization buffer.
    queue.emplace_back(std::vector<char>());
    auto* header = &queue.back();
    header->resize(BBDO_HEADER_SIZE);
    queue.emplace_back(std::vector<char>());
    auto* content = &queue.back();

    // Serialize properties of the object.
    for (mapping::entry const* current_entry(info->get_mapping());
         !current_entry->is_null(); ++current_entry) {
      // Skip entries that should not be serialized.
      if (current_entry->get_serialize())
        switch (current_entry->get_type()) {
          case mapping::source::BOOL:
            get_boolean(e, *current_entry, *content);
            break;
          case mapping::source::DOUBLE:
            get_double(e, *current_entry, *content);
            break;
          case mapping::source::INT:
            get_integer(e, *current_entry, *content);
            break;
          case mapping::source::SHORT:
            get_short(e, *current_entry, *content);
            break;
          case mapping::source::STRING:
            get_string(e, *current_entry, *content);
            break;
          case mapping::source::TIME:
            get_timestamp(e, *current_entry, *content);
            break;
          case mapping::source::UINT:
            get_uint(e, *current_entry, *content);
            break;
          default:
            log_v2::bbdo()->error(
                "BBDO: invalid mapping for object of type '{}': {} is not a "
                "known type ID",
                info->get_name(), current_entry->get_type());
            throw exceptions::msg() << "BBDO: invalid mapping for object"
                                    << " of type '" << info->get_name()
                                    << "': " << current_entry->get_type()
                                    << " is not a known type ID";
        }

      // Packet splitting.
      while (content->size() >= 0xffff) {
        queue.emplace_back(std::vector<char>());
        auto* new_header = &queue.back();
        new_header->resize(BBDO_HEADER_SIZE);
        queue.emplace_back(content->begin() + 0xffff, content->end());
        content->resize(0xffff);
        auto* new_content = &queue.back();
        *(reinterpret_cast<uint16_t*>(header->data() + 2)) = 0xffff;
        *(reinterpret_cast<uint32_t*>(header->data() + 4)) = htonl(e.type());
        *(reinterpret_cast<uint32_t*>(header->data() + 8)) = htonl(e.source_id);
        *(reinterpret_cast<uint32_t*>(header->data() + 12)) =
            htonl(e.destination_id);

        *(reinterpret_cast<uint16_t*>(header->data())) =
            htons(misc::crc16_ccitt(header->data() + 2, BBDO_HEADER_SIZE - 2));
        content = new_content;
        header = new_header;
      }
    }

    *(reinterpret_cast<uint16_t*>(header->data() + 2)) = htons(content->size());
    *(reinterpret_cast<uint32_t*>(header->data() + 4)) = htonl(e.type());
    *(reinterpret_cast<uint32_t*>(header->data() + 8)) = htonl(e.source_id);
    *(reinterpret_cast<uint32_t*>(header->data() + 12)) =
        htonl(e.destination_id);

    *(reinterpret_cast<uint16_t*>(header->data())) =
        htons(misc::crc16_ccitt(header->data() + 2, BBDO_HEADER_SIZE - 2));

    // Finalization: concatenation of all the vectors in the queue.
    size_t size = 0;
    for (auto& v : queue)
      size += v.size();

    // Serialization buffer.
    std::unique_ptr<io::raw> buffer(new io::raw);
    std::vector<char>& data(buffer->get_buffer());
    data.reserve(size);
    for (auto& v : queue)
      data.insert(data.end(), v.begin(), v.end());

    return buffer.release();
  } else {
    log_v2::bbdo()->info(
        "BBDO: cannot serialize event of ID {}: event was not registered and "
        "will therefore be ignored",
        e.type());
    logging::info(logging::high)
        << "BBDO: cannot serialize event of ID " << e.type()
        << ": event was not registered and will therefore be ignored";
  }

  return nullptr;
}

/**
 *  Default constructor.
 */
stream::stream()
    : io::stream("BBDO"),
      _skipped(0),
      _coarse(false),
      _negotiate(true),
      _negotiated(false),
      _timeout(5),
      _acknowledged_events(0),
      _ack_limit(1000),
      _events_received_since_last_ack(0) {}

/**
 *  Destructor.
 */
stream::~stream() noexcept {}

/**
 *  Flush stream data.
 *
 *  @return Number of acknowledged events.
 */
int stream::flush() {
  _substream->flush();
  int retval(_acknowledged_events);
  _acknowledged_events = 0;
  return retval;
}

/**
 *  Negotiate features with peer.
 *
 *  @param[in] neg  Negotiation type.
 */
void stream::negotiate(stream::negotiation_type neg) {
  log_v2::bbdo()->trace("BBDO: negotiate");
  std::string* extensions;
  if (!_negotiate) {
    log_v2::bbdo()->info("BBDO: negotiation disabled.");
    extensions = &_extensions.second;
  } else
    extensions = &_extensions.first;

  // Send our own packet if we should be first.
  if (neg == negotiate_first) {
    log_v2::bbdo()->debug(
        "BBDO: sending welcome packet (available extensions: {})", *extensions);
    /* if _negotiate, we send all the extensions we would like to have,
     * otherwise we only send the mandatory extensions */
    std::shared_ptr<version_response> welcome_packet(
        std::make_shared<version_response>(*extensions));
    _write(welcome_packet);
    //_substream->flush();
  }

  // Read peer packet.
  log_v2::bbdo()->debug("BBDO: retrieving welcome packet of peer");
  std::shared_ptr<io::data> d;
  time_t deadline;
  if (_timeout == (time_t)-1)
    deadline = (time_t)-1;
  else
    deadline = time(nullptr) + _timeout;

  // FIXME DBR
  _read_any(d, deadline);
  if (!d || d->type() != version_response::static_type()) {
    std::string msg;
    if (d)
      msg = fmt::format(
          "BBDO: invalid protocol header, aborting connection: waiting for "
          "message of type 'version_response' but received type is {}",
          d->type());
    else
      msg = fmt::format(
          "BBDO: invalid protocol header, aborting connection: waiting for "
          "message of type 'version_response' but nothing received");
    log_v2::bbdo()->error(msg);
    throw exceptions::msg() << msg;
  }

  // Handle protocol version.
  std::shared_ptr<version_response> v(
      std::static_pointer_cast<version_response>(d));
  if (v->bbdo_major != BBDO_VERSION_MAJOR) {
    log_v2::bbdo()->error(
        "BBDO: peer is using protocol version {}.{}.{} whereas we're using "
        "protocol version {}.{}.{}",
        v->bbdo_major, v->bbdo_minor, v->bbdo_patch, BBDO_VERSION_MAJOR,
        BBDO_VERSION_MINOR, BBDO_VERSION_PATCH);
    throw exceptions::msg()
        << "BBDO: peer is using protocol version " << v->bbdo_major << "."
        << v->bbdo_minor << "." << v->bbdo_patch
        << " whereas we're using protocol version " << BBDO_VERSION_MAJOR << "."
        << BBDO_VERSION_MINOR << "." << BBDO_VERSION_PATCH;
  }
  log_v2::bbdo()->info(
      "BBDO: peer is using protocol version {0}.{1}.{2}, we're using version "
      "{3}.{4}.{5}",
      v->bbdo_major, v->bbdo_minor, v->bbdo_patch, BBDO_VERSION_MAJOR,
      BBDO_VERSION_MINOR, BBDO_VERSION_PATCH);

  // Send our own packet if we should be second.
  if (neg == negotiate_second) {
    log_v2::bbdo()->debug(
        "BBDO: sending welcome packet (available extensions: {})", *extensions);
    /* if _negotiate, we send all the extensions we would like to have,
     * otherwise we only send the mandatory extensions */
    std::shared_ptr<version_response> welcome_packet(
        std::make_shared<version_response>(*extensions));
    _write(welcome_packet);
    _substream->flush();
  }

  // Negotiation.
  std::list<std::string> running_config = get_running_config();

  // Apply negotiated extensions.
  log_v2::bbdo()->info("BBDO: we have extensions '{}' and peer has '{}'",
                       *extensions, v->extensions);
  std::list<std::string> own_ext(misc::string::split(*extensions, ' '));
  std::list<std::string> own_mandatory(
      misc::string::split(_extensions.second, ' '));
  std::list<std::string> peer_ext(misc::string::split(v->extensions, ' '));
  for (auto& ext : own_ext) {
    // Find matching extension in peer extension list.
    std::list<std::string>::const_iterator peer_it{
        std::find(peer_ext.begin(), peer_ext.end(), ext)};
    // Apply extension if found.
    if (peer_it != peer_ext.end()) {
      if (std::find(running_config.begin(), running_config.end(), ext) ==
          running_config.end()) {
        log_v2::bbdo()->info("BBDO: applying extension '{}'", ext);
        for (std::map<std::string, io::protocols::protocol>::const_iterator
                 proto_it{io::protocols::instance().begin()},
             proto_end{io::protocols::instance().end()};
             proto_it != proto_end; ++proto_it)
          if (proto_it->first == ext) {
            std::shared_ptr<io::stream> s{
                proto_it->second.endpntfactry->new_stream(
                    _substream, neg == negotiate_second, ext)};
            set_substream(s);
            break;
          }
      } else
        log_v2::bbdo()->info("BBDO: extension '{}' already configured", ext);
    } else if (!ext.empty()) {
      if (std::find(own_mandatory.begin(), own_mandatory.end(), ext) !=
          own_mandatory.end()) {
        log_v2::bbdo()->error(
            "BBDO: extension '{}' is set to 'yes' in the configuration but "
            "cannot be activated because of peer configuration.",
            ext);
        logging::error(logging::medium)
            << "BBDO: extension '" << ext
            << "' is set to 'yes' in the configuration but cannot be activated "
               "because of peer configuration.";
      }
      if (std::find(running_config.begin(), running_config.end(), ext) !=
          running_config.end()) {
        log_v2::bbdo()->info("BBDO: extension '{}' no more needed", ext);
        auto substream = get_substream();
        if (substream->get_name() == ext) {
          auto subsubstream = substream->get_substream();
          set_substream(subsubstream);
        } else {
          while (substream) {
            auto parent = substream;
            substream = substream->get_substream();
            if (substream->get_name() == ext) {
              parent->set_substream(substream->get_substream());
              break;
            }
          }
        }
      }
    }
  }

  // Stream has now negotiated.
  _negotiated = true;
  log_v2::bbdo()->trace("Negotiation done.");
}

std::list<std::string> stream::get_running_config() {
  std::list<std::string> retval;
  std::shared_ptr<io::stream> substream = get_substream();
  while (substream) {
    retval.push_back(substream->get_name());
    substream = substream->get_substream();
  }
  return retval;
}

/**
 *  Read data from stream.
 *
 *  @param[out] d         Next available event.
 *  @param[in]  deadline  Deadline.
 *
 *  @return Respect io::stream::read() return value.
 *
 *  @see input::read()
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  d.reset();

  // Read event.
  d.reset();

  /* This lock is needed because the same stream is used by several threads.
   * In the case of long events (split in several parts), if two threads read
   * at the same time, the second one could get a part of the long event... */
  bool timed_out(!_read_any(d, deadline));
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
    timed_out = !_read_any(d, deadline);
    event_id = !d ? 0 : d->type();
  }

  if (!timed_out && d)
    ++_events_received_since_last_ack;
  if (_events_received_since_last_ack >= _ack_limit)
    send_event_acknowledgement();
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
bool stream::_read_any(std::shared_ptr<io::data>& d, time_t deadline) {
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
              buffer(event_id, source_id, dest_id, std::move(content)));

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
void stream::_read_packet(size_t size, time_t deadline) {
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
    if (timeout) {
      log_v2::bbdo()->trace("_read_packet timeout!!, size = {}, deadline = {}",
                            size, deadline);
      throw exceptions::timeout();
    }
  }
}

/**
 *  Set the limit of events received before an ack should be sent.
 *
 *  @param limit  The limit of events received before an ack should be sent.
 */
void stream::set_ack_limit(uint32_t limit) {
  _ack_limit = limit;
}

/**
 *  Set whether this stream is coarse or not.
 *
 *  @param[in] coarse  True if coarse.
 */
void stream::set_coarse(bool coarse) {
  _coarse = coarse;
}

/**
 *  Set whether or not the stream should negotiate features.
 *
 *  @param[in] negotiate   True if the stream should negotiate features.
 *  @param[in] extensions  Extensions supported by this stream.
 */
void stream::set_negotiate(
    bool negotiate,
    const std::pair<std::string, std::string>& extensions) {
  _negotiate = negotiate;
  _extensions = extensions;
}

/**
 *  Set the timeout supported by this stream.
 *
 *  @param[in] timeout  Timeout in seconds.
 */
void stream::set_timeout(int timeout) {
  _timeout = timeout;
}

/**
 *  Get statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(json11::Json::object& tree) const {
  tree["bbdo_input_ack_limit"] = static_cast<double>(_ack_limit);
  tree["bbdo_unacknowledged_events"] =
      static_cast<double>(_events_received_since_last_ack);

  if (_substream)
    _substream->statistics(tree);
}

void stream::_write(std::shared_ptr<io::data> const& d) {
  assert(d);

  // Check if data exists.
  std::shared_ptr<io::raw> serialized(serialize(*d));
  if (serialized) {
    log_v2::bbdo()->debug("BBDO: serialized event of type {} to {} bytes",
                          d->type(), serialized->size());
    _substream->write(serialized);
  }
}

/**
 *  Write data to stream.
 *
 *  @param[in] d Data to send.
 *
 *  @return Number of events acknowledged.
 */
int32_t stream::write(std::shared_ptr<io::data> const& d) {
  _write(d);

  int32_t retval(_acknowledged_events);
  _acknowledged_events -= retval;
  return retval;
}

/**
 *  Acknowledge a certain amount of events.
 *
 *  @param[in] events  The amount of event.
 */
void stream::acknowledge_events(uint32_t events) {
  _acknowledged_events += events;
}

/**
 *  Send an acknowledgement for all the events received.
 */
void stream::send_event_acknowledgement() {
  if (!_coarse) {
    std::shared_ptr<ack> acknowledgement(
        std::make_shared<ack>(_events_received_since_last_ack));
    _write(acknowledgement);
    _events_received_since_last_ack = 0;
  }
}
