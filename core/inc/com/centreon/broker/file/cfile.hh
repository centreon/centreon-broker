/*
** Copyright 2012,2017 Centreon
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

#ifndef CCB_FILE_CFILE_HH
#define CCB_FILE_CFILE_HH

#include <cstdio>
#include "com/centreon/broker/file/fs_file.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace file {
/**
 *  @class cfile cfile.hh "com/centreon/broker/file/cfile.hh"
 *  @brief Wrapper for C-style FILE streams.
 *
 *  Wrap calls that work on C FILE streams.
 */
class cfile : public fs_file {
 public:
  cfile(std::string const& path, fs_file::open_mode mode);
  ~cfile();
  void close();
  long read(void* buffer, long max_size);
  void seek(long offset, fs_file::seek_whence = fs_file::seek_start);
  long tell();
  long write(void const* buffer, long size);

 private:
  cfile(cfile const& right);
  cfile& operator=(cfile const& right);
  void _open();

  FILE* _stream;
  std::string _path;
  open_mode _mode;
};

/**
 *  @class cfile_factory cfile.hh "com/centreon/broker/file/cfile.hh"
 *  @brief Build a new cfile.
 *
 *  Build a new cfile.
 */
class cfile_factory : public fs_file_factory {
 public:
  cfile* new_cfile(std::string const& path, fs_file::open_mode mode);
  fs_file* new_fs_file(std::string const& path, fs_file::open_mode mode);
};
}  // namespace file

CCB_END()

#endif  // !CCB_FILE_CFILE_HH
