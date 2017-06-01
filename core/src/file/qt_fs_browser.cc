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

#include <QDir>
#include <QFile>
#include "com/centreon/broker/file/qt_fs_browser.hh"

using namespace com::centreon::broker::file;

/**
 *  Default constructor.
 */
qt_fs_browser::qt_fs_browser() {}

/**
 *  Destructor.
 */
qt_fs_browser::~qt_fs_browser() {}

/**
 *  Read directory elements.
 *
 *  @param[in] path     Directory path.
 *  @param[in] filters  Entries filters.
 *
 *  @return List of elements within the directory.
 */
fs_browser::entry_list qt_fs_browser::read_directory(
                                        std::string const& path,
                                        std::string const& filters) {
  QDir dir(path.c_str());
  QStringList entries;
  {
    QStringList filters_list;
    filters_list << filters.c_str();
    entries = dir.entryList(filters_list);
  }
  fs_browser::entry_list retval;
  for (QStringList::iterator it(entries.begin()), end(entries.end());
       it != end;
       ++it)
    retval.push_back(it->toStdString());
  return (retval);
}

/**
 *  Remove a file.
 *
 *  @param[in] path  Path to file.
 */
void qt_fs_browser::remove(std::string const& path) {
  QFile::remove(path.c_str());
  return ;
}
