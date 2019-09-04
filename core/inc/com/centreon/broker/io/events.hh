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
  typedef std::unordered_map<unsigned int, event_info> events_container;
  struct category_info {
    std::string name;
    events_container events;
  };
  typedef std::unordered_map<unsigned short, category_info>
      categories_container;
  // Reserved categories, for reference.
  enum data_category {
    neb = 1,
    bbdo,
    storage,
    correlation,
    dumper,
    bam,
    extcmd,
    generator,
    internal = 65535
  };
  // Internal events used by the core.
  enum internal_event_category { de_raw = 1, de_instance_broadcast, de_buffer };
  // Extcmd events used by the core.
  enum extcmd_event_category {
    de_command_request = 1,
    de_command_result,
  };

  template <unsigned short category, unsigned short element>
  struct data_type {
    enum { value = static_cast<unsigned int>(category << 16 | element) };
  };

  // Singleton.
  static events& instance();
  static void load();
  static void unload();

  // Category.
  unsigned short register_category(std::string const& name,
                                   unsigned short hint = 0);
  void unregister_category(unsigned short category_id);

  // Events.
  unsigned int register_event(unsigned short category_id,
                              unsigned short event_id,
                              event_info const& info);
  void unregister_event(unsigned int type_id);

  // ID manipulations.
  static unsigned short category_of_type(unsigned int type) throw() {
    return (static_cast<unsigned short>(type >> 16));
  }
  static unsigned short element_of_type(unsigned int type) throw() {
    return (static_cast<unsigned short>(type));
  }
  static unsigned int make_type(unsigned short category_id,
                                unsigned short element_id) throw() {
    return ((static_cast<unsigned int>(category_id) << 16) | element_id);
  }

  // Event browsing.
  categories_container::const_iterator begin() const;
  categories_container::const_iterator end() const;
  events_container get_events_by_category_name(std::string const& name) const;
  event_info const* get_event_info(unsigned int type);
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
