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

#ifndef CCB_IO_EVENTS_HH
#define CCB_IO_EVENTS_HH

#include <unordered_map>

#include "com/centreon/broker/io/event_info.hh"

CCB_BEGIN()

namespace io {
/**
 *  @class events events.hh "com/centreon/broker/io/events.hh"
 *  @brief Data events registration.
 *
 *  Maintain the set of existing events.
 */
class events {
 public:
  typedef std::unordered_map<uint32_t, event_info> events_container;
  struct category_info {
    std::string name;
    events_container events;
  };
  typedef std::unordered_map<uint16_t, category_info> categories_container;
  // Reserved categories, for reference.
  // Their values are very important to maintain the bbdo protocol retro
  // compatible.
  enum data_category {
    neb = 1,
    bbdo = 2,
    storage = 3,
    dumper = 5,
    bam = 6,
    extcmd = 7,
    generator = 8,
    internal = 65535
  };
  // Internal events used by the core.
  enum internal_event_category { de_raw = 1, de_instance_broadcast, de_buffer };
  // Extcmd events used by the core.
  enum extcmd_event_category {
    de_command_request = 1,
    de_command_result,
  };

  template <uint16_t category, uint16_t element>
  struct data_type {
    enum { value = static_cast<uint32_t>(category << 16 | element) };
  };

  // Singleton.
  static events& instance();
  static void load();
  static void unload();

  // Category.
  uint16_t register_category(std::string const& name, uint16_t hint);
  void unregister_category(uint16_t category_id);

  // Events.
  uint32_t register_event(uint16_t category_id,
                          uint16_t event_id,
                          std::string const& name = std::string(),
                          event_info::event_operations const* ops = nullptr,
                          mapping::entry const* entries = nullptr,
                          std::string const& table_v2 = std::string());
  void unregister_event(uint32_t type_id);

  // ID manipulations.
  static uint16_t category_of_type(uint32_t type) noexcept {
    return static_cast<uint16_t>(type >> 16);
  }
  static uint16_t element_of_type(uint32_t type) noexcept {
    return static_cast<uint16_t>(type);
  }
  static uint32_t make_type(uint16_t category_id,
                            uint16_t element_id) noexcept {
    return (static_cast<uint32_t>(category_id) << 16) | element_id;
  }

  // Event browsing.
  categories_container::const_iterator begin() const;
  categories_container::const_iterator end() const;
  events_container get_events_by_category_name(std::string const& name) const;
  event_info const* get_event_info(uint32_t type);
  events_container get_matching_events(std::string const& name) const;

 private:
  events();
  events(events const& other);
  ~events();
  events& operator=(events const& other);

  categories_container _elements;
};
}  // namespace io

CCB_END()

#endif  // !CCB_IO_EVENTS_HH
