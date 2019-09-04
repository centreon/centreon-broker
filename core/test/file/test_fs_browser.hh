/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */
#ifndef TEST_FS_BROWSER_HH
#  define TEST_FS_BROWSER_HH

#  include <list>
#  include "com/centreon/broker/file/fs_browser.hh"

class              test_fs_browser : public com::centreon::broker::file::fs_browser {
 public:
                   test_fs_browser();
                   ~test_fs_browser();
  void             add_result(
                     com::centreon::broker::file::fs_browser::entry_list const& result);
  com::centreon::broker::file::fs_browser::entry_list
                   read_directory(
                     std::string const& path,
                     std::string const& filters = "");
  std::list<std::string> const&
                   get_removed() const;
  void             remove(std::string const& path);

 private:
                   test_fs_browser(test_fs_browser const& other);
  test_fs_browser& operator=(test_fs_browser const& other);

  std::list<std::string>
                   _removed;
  std::list<com::centreon::broker::file::fs_browser::entry_list>
                   _results;
};

#endif // !TEST_FS_BROWSER_HH
