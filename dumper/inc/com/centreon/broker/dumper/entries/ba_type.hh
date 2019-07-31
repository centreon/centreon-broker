/*
** Copyright 2015 Centreon
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

#ifndef CCB_DUMPER_ENTRIES_BA_TYPE_HH
#  define CCB_DUMPER_ENTRIES_BA_TYPE_HH

#  include <string>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 dumper {
  namespace               entries {
    /**
     *  @class ba_type ba_type.hh "com/centreon/broker/dumper/entries/ba_type.hh"
     *  @brief BA type.
     *
     *  BA type, used by UI.
     */
    class                 ba_type : public io::data {
    public:
                          ba_type();
                          ba_type(ba_type const& other);
                          ~ba_type();
      ba_type&            operator=(ba_type const& other);
      bool                operator==(ba_type const& other) const;
      bool                operator!=(ba_type const& other) const;
      unsigned int        type() const;
      static unsigned int static_type();

      bool                enable;
      unsigned int        ba_type_id;
      std::string             description;
      std::string             name;
      std::string             slug;

      static mapping::entry const
                          entries[];
      static io::event_info::event_operations const
                          operations;

    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_BA_TYPE_HH
