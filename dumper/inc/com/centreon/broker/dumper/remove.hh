/*
** Copyright 2013 Centreon
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

#ifndef CCB_DUMPER_REMOVE_HH
#  define CCB_DUMPER_REMOVE_HH

#  include <string>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace               dumper {
  /**
   *  @class remove remove.hh "com/centreon/broker/dumper/remove.hh"
   *  @brief Remove a file.
   *
   *  Remove a file transmitted by some provider.
   */
  class                 remove : public io::data {
  public:
                        remove();
                        remove(remove const& right);
                        ~remove();
    remove&             operator=(remove const& right);
    unsigned int        type() const;
    static unsigned int static_type();

    std::string             tag;
    std::string             filename;
    std::string             req_id;

    static mapping::entry const
                        entries[];
    static io::event_info::event_operations const
                        operations;

  private:
    void                _internal_copy(remove const& right);
  };
}

CCB_END()

#endif // !CCB_DUMPER_REMOVE_HH
