/*
** Copyright 2015,2017 Centreon
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

#include <cstring>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include "com/centreon/broker/misc/filesystem.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;

/**
 *  Fill a strings list with the files listed in the directory.
 *
 * @param path The directory name
 *
 * @return a list of names.
 */
std::list<std::string> filesystem::dir_content(std::string const& path, bool recursive) {
  std::list<std::string> retval;
  DIR* dir{opendir(path.c_str())};
  if (dir) {
    struct dirent* ent;
    while ((ent = readdir(dir))) {
      if (strncmp(ent->d_name, ".", 2) == 0 ||
          strncmp(ent->d_name, "..", 3) == 0)
        continue;
      std::string fullname{path};
      fullname.append("/").append(ent->d_name);
      if (recursive && ent->d_type == DT_DIR) {
        std::list<std::string> res{filesystem::dir_content(fullname, true)};
        retval.splice(retval.end(), res);
      }
      else if (ent->d_type == DT_REG)
        retval.push_back(std::move(fullname));
    }
    closedir(dir);
  }
  else
    logging::error(logging::medium)
      << "directory_dumper: unable to read directory '" << path << "'";

  return retval;
}

/**
 *  Check if the directory exists.
 *
 * @param path The directory name
 *
 * @return a boolean.
 */
bool filesystem::dir_exists(std::string const& path) {
  struct stat info;
  if (stat(path.c_str(), &info) != 0)
    return false;

  return (info.st_mode & S_IFDIR);
}

/**
 *  Create a directory with all the needed parent directories
 *
 * @param path A directory name
 *
 * @return A boolean that is true on success.
 */
bool filesystem::mkpath(std::string const& path) {
  mode_t mode = 0755;

  /* In almost all the cases, the parent directories exist */
  int ret = mkdir(path.c_str(), mode);
  if (ret == 0)
    return true;

  switch (errno) {
    case ENOENT:
      // parent didn't exist, try to create it
      {
        int pos = path.find_last_of('/');
        if (pos == std::string::npos)
          return false;
        if (!mkpath(path.substr(0, pos)))
          return false;
      }
      // now, try to create again
      return mkdir(path.c_str(), mode) == 0;

    case EEXIST:
      return dir_exists(path);

    default:
      return false;
  }
}

int64_t filesystem::file_size(std::string const& path) {
    std::ifstream file{path, std::ios::binary | std::ios::ate};
    int64_t size{file.tellg()};
    return size;
}
