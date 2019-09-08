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

#ifndef CCB_FILE_DIRECTORY_EVENT_HH
#define CCB_FILE_DIRECTORY_EVENT_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace file {
/**
 *  @class directory_event directory_event.hh
 * "com/centreon/broker/file/directory_event.hh"
 *  @brief Event of a watched directory.
 *
 *  This represents an event happening to a watched directory.
 */
class directory_event {
 public:
  enum type { created, modified, deleted, directory_deleted };
  enum file_type { directory, file, other };

  directory_event();
  directory_event(std::string const& path, type type, file_type ft);
  directory_event(directory_event const& o);
  directory_event& operator=(directory_event const& o);
  ~directory_event();

  std::string const& get_path() const;
  type get_type() const;
  file_type get_file_type() const;

 private:
  std::string _path;
  type _type;
  file_type _file_type;
};
}  // namespace file

CCB_END()

#endif  // !CCB_FILE_DIRECTORY_EVENT_HH
