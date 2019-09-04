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

#include "com/centreon/broker/file/stl_fs_browser.hh"
#include "com/centreon/broker/misc/filesystem.hh"

using namespace com::centreon::broker::file;

/**
 *  Default constructor.
 */
stl_fs_browser::stl_fs_browser() {}

/**
 *  Destructor.
 */
stl_fs_browser::~stl_fs_browser() {}

/**
 *  Read directory elements.
 *
 *  @param[in] path     Directory path.
 *  @param[in] filters  Entries filters.
 *
 *  @return List of elements within the directory.
 */
#include <iostream>
fs_browser::entry_list stl_fs_browser::read_directory(
    std::string const& path,
    std::string const& filters) {
  std::list<std::string> entries;
  entries = misc::filesystem::dir_content(path, filters);

  fs_browser::entry_list retval;
  for (std::list<std::string>::iterator it(entries.begin()), end(entries.end());
       it != end; ++it) {
    std::size_t pos;
    pos = it->rfind('/');
    if (pos == std::string::npos)
      retval.push_back(*it);
    else
      retval.push_back(it->substr(pos));
  }
  return (retval);
}

/**
 *  Remove a file.
 *
 *  @param[in] path  Path to file.
 */
void stl_fs_browser::remove(std::string const& path) {
  std::remove(path.c_str());
  return;
}
