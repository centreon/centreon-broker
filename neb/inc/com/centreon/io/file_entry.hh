/*
** Copyright 2012-2013 Centreon
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

#ifndef CC_IO_FILE_ENTRY_HH
#define CC_IO_FILE_ENTRY_HH

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include "com/centreon/handle.hh"
#include "com/centreon/namespace.hh"

#ifdef _WIN32
#define stat _stat
#endif  // _WIN32

CC_BEGIN()

namespace io {
/**
 *  @class file_entry file_entry.hh "com/centreon/io/file_entry.hh"
 *  @brief Wrapper of stat information.
 *
 *  Wrap standard stat information.
 */
class file_entry {
 public:
  file_entry(char const* path = NULL);
  file_entry(std::string const& path);
  file_entry(file_entry const& right);
  ~file_entry() throw();
  file_entry& operator=(file_entry const& right);
  bool operator==(file_entry const& right) const throw();
  bool operator!=(file_entry const& right) const throw();
  std::string base_name() const;
  std::string directory_name() const;
  std::string file_name() const;
  bool is_directory() const throw();
  bool is_link() const throw();
  bool is_regular() const throw();
  std::string const& path() const throw();
  void path(char const* path);
  void path(std::string const& path);
  void refresh();
  unsigned long long size() const throw();

 private:
  void _internal_copy(file_entry const& right);

  std::string _path;
  struct stat _sbuf;
};
}

CC_END()

#endif  // !CC_IO_FILE_ENTRY_HH
