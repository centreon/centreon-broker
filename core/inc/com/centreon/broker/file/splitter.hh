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

#ifndef CCB_FILE_SPLITTER_HH
#define CCB_FILE_SPLITTER_HH

#include <memory>
#include <string>
#include "com/centreon/broker/file/fs_file.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace file {
/**
 *  @class splitter splitter.hh "com/centreon/broker/file/splitter.hh"
 *  @brief Manage multi-file splitting.
 *
 *  Handle logical file splitting across multiple real files to
 *  provide easier file management.
 */
class splitter : public fs_file {
 public:
  splitter(std::string const& path,
           fs_file::open_mode mode,
           long max_file_size = 100000000,
           bool auto_delete = false);
  ~splitter();
  void close() override;
  long read(void* buffer, long max_size) override;
  void remove_all_files();
  void seek(long offset,
            fs_file::seek_whence whence = fs_file::seek_start) override;
  long tell() override;
  long write(void const* buffer, long size) override;
  void flush() override;

  std::string get_file_path(int id = 0) const;
  long get_max_file_size() const;
  int get_rid() const;
  long get_roffset() const;
  int get_wid() const;
  long get_woffset() const;

 private:
  splitter(splitter const& other);
  splitter& operator=(splitter const& other);
  void _open_read_file();
  void _open_write_file();

  bool _auto_delete;
  std::string _base_path;
  long _max_file_size;
  std::shared_ptr<fs_file> _rfile;
  int _rid;
  long _roffset;
  std::shared_ptr<fs_file> _wfile;
  int _wid;
  long _woffset;
};
}  // namespace file

CCB_END()

#endif  // !CCB_FILE_SPLITTER_HH
