/*
** Copyright 2017 Centreon
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

#ifndef CCB_FILE_FS_BROWSER_HH
#define CCB_FILE_FS_BROWSER_HH

#include <list>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace file {
/**
 *  @class fs_browser fs_browser.hh "com/centreon/broker/file/fs_browser.hh"
 *  @brief File system browser.
 *
 *  Perform operations on file system.
 */
class fs_browser {
 public:
  typedef std::list<std::string> entry_list;

  fs_browser();
  virtual ~fs_browser();
  virtual entry_list read_directory(std::string const& path,
                                    std::string const& filters = "") = 0;
  virtual void remove(std::string const& path) = 0;

 private:
  fs_browser(fs_browser const& other);
  fs_browser& operator=(fs_browser const& other);
};
}  // namespace file

CCB_END()

#endif  // !CCB_FILE_FS_BROWSER_HH
