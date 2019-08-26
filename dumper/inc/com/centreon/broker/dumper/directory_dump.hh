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

#ifndef CCB_DUMPER_DIRECTORY_DUMP_HH
#  define CCB_DUMPER_DIRECTORY_DUMP_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               dumper {
  /**
   *  @class directory_dump directory_dump.hh "com/centreon/broker/dumper/directory_dump.hh"
   *  @brief Directory dump event.
   *
   *  Notify that a directory dump is starting or stopping.
   */
  class                 directory_dump : public io::data {
  public:
                        directory_dump();
                        directory_dump(directory_dump const& other);
                        ~directory_dump();
    directory_dump&     operator=(directory_dump const& other);
    unsigned int        type() const;
    static unsigned int static_type();

    std::string             tag;
    bool                started;
    std::string             req_id;

    static mapping::entry const
                        entries[];
    static io::event_info::event_operations const
                        operations;

  private:
    void                _internal_copy(directory_dump const& other);
  };
}

CCB_END()

#endif // !CCB_DUMPER_DIRECTORY_DUMP_HH
