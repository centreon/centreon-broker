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

#ifndef CCB_FILE_FS_FILE_HH
#  define CCB_FILE_FS_FILE_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        file {
  /**
   *  @class fs_file fs_file.hh "com/centreon/broker/file/fs_file.hh"
   *  @brief File system (on-disk) file.
   *
   *  Interface to manipulate an on-disk file.
   */
  class          fs_file {
   public:
    enum         open_mode {
      open_read = 1,
      open_write,
      open_read_write_truncate,
      open_read_write_no_create
    };
    enum         seek_whence {
      seek_start = 1,
      seek_current,
      seek_end
    };

                 fs_file();
    virtual      ~fs_file();
    virtual void close() = 0;
    virtual long read(void* buffer, long max_size) = 0;
    virtual void seek(
                   long offset,
                   seek_whence whence = seek_start) = 0;
    virtual long tell() = 0;
    virtual long write(void const* buffer, long size) = 0;

   private:
                 fs_file(fs_file const& other);
    fs_file&     operator=(fs_file const& other);
  };

  /**
   *  @class fs_file_factory fs_file.hh "com/centreon/broker/file/fs_file.hh"
   *  @brief Interface to create fs_file.
   *
   *  Interface used to create fs_file without knowing the concrete type.
   */
  class              fs_file_factory {
   public:
    virtual fs_file* new_fs_file(
                       std::string const& path,
                       fs_file::open_mode mode) = 0;
    virtual ~fs_file_factory() {};
  };
}

CCB_END()

#endif // !CCB_FILE_FS_FILE_HH
