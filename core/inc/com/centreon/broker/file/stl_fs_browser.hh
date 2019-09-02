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

#ifndef CCB_FILE_STL_FS_BROWSER_HH
#  define CCB_FILE_STL_FS_BROWSER_HH

#  include "com/centreon/broker/file/fs_browser.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          file {
  /**
   *  @class stl_fs_browser stl_fs_browser.hh "com/centreon/broker/file/stl_fs_browser.hh"
   *  @brief Qt-base implementation of fs_browser.
   *
   *  Implements fs_browser interface using Qt.
   */
  class            stl_fs_browser : public fs_browser {
   public:
                   stl_fs_browser();
                   ~stl_fs_browser();
    fs_browser::entry_list
                   read_directory(
                     std::string const& path,
                     std::string const& filters = "");
    void           remove(std::string const& path);

   private:
                   stl_fs_browser(stl_fs_browser const& other);
    stl_fs_browser& operator=(stl_fs_browser const& other);
  };
}

CCB_END()

#endif // !CCB_FILE_STL_FS_BROWSER_HH
