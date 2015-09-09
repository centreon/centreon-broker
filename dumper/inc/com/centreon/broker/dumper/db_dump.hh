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

#ifndef CCB_DUMPER_DB_DUMP_HH
#  define CCB_DUMPER_DB_DUMP_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               dumper {
  /**
   *  @class db_dump db_dump.hh "com/centreon/broker/dumper/db_dump.hh"
   *  @brief DB dump event.
   *
   *  Notify that a DB dump is starting or stopping.
   */
  class                 db_dump : public io::data {
  public:
                        db_dump();
                        db_dump(db_dump const& other);
                        ~db_dump();
    db_dump&            operator=(db_dump const& other);
    unsigned int        type() const;

    bool                commit;
    bool                full;
    unsigned int        poller_id;

  private:
    void                _internal_copy(db_dump const& other);
  };
}

CCB_END()

#endif // !CCB_DUMPER_DB_DUMP_HH
