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
  cfile() = delete;
  cfile(cfile const& other) = delete;
  cfile(std::string const& path, fs_file::open_mode mode);
  cfile& operator=(cfile const& other) = delete;
  ~cfile();
  void close() override final;
  long read(void* buffer, long max_size) override;
  void seek(long offset, fs_file::seek_whence = fs_file::seek_start) override;
  long tell() override;
  long write(void const* buffer, long size) override;
  void flush() override;

 private:
  void _open();

  FILE* _stream;
  std::string _path;
  open_mode _mode;
};
}  // namespace file

CCB_END()

#endif  // !CCB_FILE_CFILE_HH
