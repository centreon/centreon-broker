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

#ifndef CC_IO_DIRECTORY_ENTRY_HH
#define CC_IO_DIRECTORY_ENTRY_HH

#include <list>
#include <string>
#include "com/centreon/io/file_entry.hh"
#include "com/centreon/handle.hh"
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace io {
/**
 *  @class directory_entry directory_entry.hh
 *"com/centreon/io/directory_entry.hh"
 *  @brief Wrapper of libc's directory_entryectory.
 *
 *  Wrap standard directory_entryectory objects.
 */
class directory_entry {
 public:
  directory_entry(char const* path = NULL);
  directory_entry(std::string const& path);
  directory_entry(directory_entry const& right);
  directory_entry& operator=(directory_entry const& right);
  bool operator==(directory_entry const& right) const throw();
  bool operator!=(directory_entry const& right) const throw();
  ~directory_entry() throw();
  static std::string current_path();
  file_entry const& entry() const throw();
  std::list<file_entry> const& entry_list(std::string const& filter = "");

 private:
  void _internal_copy(directory_entry const& right);
  static int _nmatch(char const* str, char const* pattern);

  file_entry _entry;
  std::list<file_entry> _entry_lst;
};
}

CC_END()

#endif  // !CC_IO_DIRECTORY_ENTRY_HH
