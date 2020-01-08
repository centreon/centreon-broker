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

#ifndef CC_IO_FILE_STREAM_HH
#define CC_IO_FILE_STREAM_HH

#include <cstdio>
#include <string>
#include "com/centreon/handle.hh"
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace io {
/**
 *  @class file_stream file_stream.hh "com/centreon/io/file_stream.hh"
 *  @brief Wrapper of libc's FILE streams.
 *
 *  Wrap standard FILE stream objects.
 */
class file_stream : public handle {
 public:
  file_stream(FILE* stream = NULL, bool auto_close = false);
  ~file_stream() throw();
  void close();
  static void copy(char const* src, char const* dst);
  static void copy(std::string const& src, std::string const& dst);
  static bool exists(char const* path);
  static bool exists(std::string const& path);
  void flush();
  native_handle get_native_handle();
  void open(char const* path, char const* mode);
  void open(std::string const& path, char const* mode);
  unsigned long read(void* data, unsigned long size);
  static bool remove(char const* path);
  static bool remove(std::string const& path);
  static bool rename(char const* old_filename, char const* new_filename);
  static bool rename(std::string const& old_filename,
                     std::string const& new_filename);
  unsigned long size();
  static char* temp_path();
  unsigned long write(void const* data, unsigned long size);

 private:
  file_stream(file_stream const& fs);
  file_stream& operator=(file_stream const& fs);

  bool _auto_close;
  FILE* _stream;
};
}

CC_END()

#endif  // !CC_IO_FILE_STREAM_HH
