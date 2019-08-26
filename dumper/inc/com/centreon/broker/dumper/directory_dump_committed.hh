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

#ifndef CCB_DUMPER_DIRECTORY_DUMP_COMMITTED_HH
#  define CCB_DUMPER_DIRECTORY_DUMP_COMMITTED_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace dumper {
/**
 *  @class directory_dump_committed directory_dump_committed.hh
 * "com/centreon/broker/dumper/directory_dump_committed.hh"
 *  @brief Directory dump committed event.
 *
 *  Notify that a directory dump has been properly committed.
 */
class directory_dump_committed : public io::data {
 public:
  directory_dump_committed();
  directory_dump_committed(directory_dump_committed const& other);
  ~directory_dump_committed();
  directory_dump_committed& operator=(directory_dump_committed const& other);
  unsigned int type() const;
  static unsigned int static_type();

  bool success;
  std::string req_id;
  std::string error_message;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(directory_dump_committed const& other);
};
}  // namespace dumper

CCB_END()

#endif // !CCB_DUMPER_DIRECTORY_DUMP_COMMITTED_HH
