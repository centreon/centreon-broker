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

#ifndef CCB_FILE_DIRECTORY_WATCHER_HH
#define CCB_FILE_DIRECTORY_WATCHER_HH

#include <map>
#include <string>
#include <vector>
#include "com/centreon/broker/file/directory_event.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace file {
/**
 *  @class directory_watcher directory_watcher.hh
 * "com/centreon/broker/file/directory_watcher.hh"
 *  @brief Directory watcher.
 *
 *  Watch over directories for files modifications.
 */
class directory_watcher {
 public:
  directory_watcher();
  ~directory_watcher();

  void add_directory(std::string const& directory);
  void remove_directory(std::string const& directory);
  bool is_watched(std::string const& directory);

  std::vector<directory_event> get_events();
  void set_timeout(unsigned int msecs);

 private:
  directory_watcher(directory_watcher const& o);
  directory_watcher& operator=(directory_watcher const& o);

  int _inotify_instance_id;
  unsigned int _timeout;
  std::map<std::string, int> _path_to_id;
  std::map<int, std::string> _id_to_path;
};
}  // namespace file

CCB_END()

#endif  // !CCB_FILE_DIRECTORY_WATCHER_HH
