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

#ifndef CCB_DUMPER_TIMESTAMP_CACHE_HH
#  define CCB_DUMPER_TIMESTAMP_CACHE_HH

#  include <string>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace               dumper {
  /**
   *  @class timestamp_cache timestamp_cache.hh "com/centreon/broker/dumper/timestamp_cache.hh"
   *  @brief Cache the last modified time of a file.
   *
   *  This is used to cache this information in the persistant cache.
   */
  class                 timestamp_cache : public io::data {
  public:
                        timestamp_cache();
                        timestamp_cache(timestamp_cache const& right);
                        ~timestamp_cache();
    timestamp_cache&    operator=(timestamp_cache const& right);
    unsigned int        type() const;
    static unsigned int static_type();

    std::string             filename;
    timestamp           last_modified;

    static mapping::entry const
                        entries[];
    static io::event_info::event_operations const
                        operations;

  private:
    void                _internal_copy(timestamp_cache const& right);
  };
}

CCB_END()

#endif // !CCB_DUMPER_TIMESTAMP_CACHE_HH
