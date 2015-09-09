/*
** Copyright 2013-2014 Centreon
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
#  define CCB_IO_EVENTS_HH

#  include <map>
#  include <set>
#  include <string>
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
    enum                data_category {
      neb = 1,
      bbdo,
      storage,
      correlation,
      dumper,
      bam,
      internal = 65535
    };
    enum                internal_data_element {
      de_raw = 1,
      de_command_request,
      de_command_result
    };


    template <unsigned short category, unsigned short element>
    struct data_type {
      enum { value = static_cast<unsigned int>(category << 16 | element) };
    };

    std::map<std::string, std::set<unsigned int> >::const_iterator
                        begin() const;
    static unsigned short
                        category_of_type(unsigned int type) throw () {
      return (static_cast<unsigned short>(type >> 16));
    }
    static unsigned short
                        element_of_type(unsigned int type) throw () {
      return (static_cast<unsigned short>(type));
    }
    std::map<std::string, std::set<unsigned int> >::const_iterator
                        end() const;
    std::set<unsigned int> const&
                        get(std::string const& name) const;
    static events&      instance();
    static void         load();
    void                reg(
                          std::string const& name,
                          std::set<unsigned int> const& elems);
    static void         unload();
    void                unreg(std::string const& name);

  private:
                        events();
                        events(events const& right);
                        ~events();
    events&             operator=(events const& right);

    std::map<std::string, std::set<unsigned int> >
                        _elements;
  };
}

CCB_END()

#endif // !CCB_IO_EVENTS_HH
