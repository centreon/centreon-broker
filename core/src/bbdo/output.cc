/*
** Copyright 2013-2015,2017 Centreon
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

#include "com/centreon/broker/bbdo/output.hh"

#include <arpa/inet.h>
#include <stdint.h>

#include <cstdio>
#include <cstring>
#include <deque>
#include <memory>
#include <thread>

#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/event_info.hh"
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
output::output() {}

/**
 *  Destructor.
 */
output::~output() {}

/**
 *  Flush.
 *
 *  @return Number of events acknowledged (0).
 */
int output::flush() {
  _substream->flush();
  return 0;
}

/**
 *  Get statistics.
 *
 *  @param[out] tree Output tree.
 */
void output::statistics(json11::Json::object& tree) const {
  if (_substream)
    _substream->statistics(tree);
}

/**
 *  Send an event.
 *
 *  @param[in] e Event to send.
 *
 *  @return Number of events acknowledged.
 */
int output::write(std::shared_ptr<io::data> const& e) {
  if (!validate(e, "BBDO"))
    return 1;

  // Check if data exists.
  std::shared_ptr<io::raw> serialized(serialize(*e));
  if (serialized) {
    log_v2::bbdo()->debug("BBDO: serialized event of type {} to {} bytes",
                          e->type(), serialized->size());
    _substream->write(serialized);
  }

  // Event acknowledgement is done in the higher level bbdo::stream.
  return 0;
}
