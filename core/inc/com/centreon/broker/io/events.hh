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

#ifndef CCB_IO_EVENTS_HH
#  define CCB_IO_EVENTS_HH

#  include <map>
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               io {
  /**
   *  @class events events.hh "com/centreon/broker/io/events.hh"
   *  @brief Data events registration.
   *
   *  Maintain the set of existing events.
   */
  class                 events {
  public:
    typedef std::map<unsigned int, event_info>  events_container;
    struct              category_info {
      std::string       name;
      events_container  events;
    };
    typedef std::map<unsigned short, category_info> categories_container;
    // Reserved categories, for reference.
    enum                data_category {
      neb = 1,
      bbdo,
      storage,
      correlation,
      dumper,
      internal = 65535
    };
    template <unsigned short category, unsigned short element>
    struct data_type {
      enum { value = static_cast<unsigned int>(category << 16 | element) };
    };

    // Singleton.
    static events&      instance();
    static void         load();
    static void         unload();

    // Category.
    unsigned short      register_category(
                          std::string const& name,
                          unsigned short hint = 0);
    void                unregister_category(unsigned short category_id);

    // Events.
    unsigned int        register_event(
                          unsigned short category_id,
                          unsigned short event_id,
                          event_info const& info);
    void                unregister_event(unsigned int type_id);

    // ID manipulations.
    static unsigned short
                        category_of_type(unsigned int type) throw () {
      return (static_cast<unsigned short>(type >> 16));
    }
    static unsigned short
                        element_of_type(unsigned int type) throw () {
      return (static_cast<unsigned short>(type));
    }
    static unsigned int
                        make_type(
                          unsigned short category_id,
                          unsigned short element_id) throw () {
      return ((static_cast<unsigned int>(category_id) << 16)
              | element_id);
    }

    // Event browsing.
    categories_container::const_iterator
                        begin() const;
    categories_container::const_iterator
                        end() const;
    events_container const&
                        get_events_by_category_name(
                          std::string const& name) const;
    events_container   get_matching_events(
                          std::string const& name) const;

  private:
                        events();
                        events(events const& other);
                        ~events();
    events&             operator=(events const& other);

    categories_container
                        _elements;
  };
}

CCB_END()

#endif // !CCB_IO_EVENTS_HH
