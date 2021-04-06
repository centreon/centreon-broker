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
#include <mutex>
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
 *
 *  How does it work.
 *  A splitter is made of several files. They all have the same base name but
 *  from the second one, this name is follow by a number which is incremented
 *  each time a new file is created.
 *
 *  We don't want to lock accesses to those files each time a reading or a
 *  writing is done. But we must lock access when we read and write on the same
 *  file. To aquieve this, we introduce two mutexes, _mutex1 and _mutex2. At
 *  the beginning, when the splitter is open for an action (read or write),
 *  there are two cases:
 *  * The file we want is not already open, so we open it and we associate one
 *    mutex to it (for reading, mutex1 is chosen and we set its pointer to
 *    _rmutex, whereas for writing, mutex2 is chosen and set to _wmutex).
 *  * The file to access is already open. We get the same file and we get the
 *    same mutex pointer.
 *
 *  _rid and _wid are indexes to the current files, _rid for reading and _wid
 *  for writing., _rfile and _wfile are shared pointers to FILE structs.
 *
 *  _base_path is the base name of files, it may be followed by a number that
 *  is _rid or _wid.
 *
 *  _woffset and _roffset are offsets from the files begin to write or read.
 *
 *  A third mutex id_m is set essentially when it is time to open a file. It
 *  is the _wid and _rid lock. _rmutex and _wmutex are set while it is locked.
 *
 *  FIXME: Maybe a better algorithm would allow us to avoid it.
 */
class splitter : public fs_file {
  bool _auto_delete;
  std::string _base_path;
  long _max_file_size;
  std::shared_ptr<FILE> _rfile;
  std::mutex* _rmutex;
  int32_t _rid;
  long _roffset;
  std::shared_ptr<FILE> _wfile;
  std::mutex* _wmutex;
  int32_t _wid;
  long _woffset;
  std::mutex _mutex1;
  std::mutex _mutex2;
  std::mutex _id_m;

  void _open_read_file();
  void _open_write_file();

 public:
  splitter(std::string const& path,
           fs_file::open_mode mode,
           long max_file_size = 100000000,
           bool auto_delete = false);
  ~splitter();
  splitter(const splitter&) = delete;
  splitter& operator=(const splitter&) = delete;
  void close() override final;
  long read(void* buffer, long max_size) override;
  void remove_all_files();
  void seek(long offset,
            fs_file::seek_whence whence = fs_file::seek_start) override;
  long tell() override;
  long write(void const* buffer, long size) override;
  void flush() override;

  std::string get_file_path(int id = 0) const;
  long get_max_file_size() const;
  int32_t get_rid() const;
  long get_roffset() const;
  int32_t get_wid() const;
  long get_woffset() const;
};
}  // namespace file

CCB_END()

#endif  // !CCB_FILE_SPLITTER_HH
