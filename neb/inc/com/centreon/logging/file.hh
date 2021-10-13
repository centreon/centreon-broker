/*
** Copyright 2011-2014 Centreon
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

#ifndef CC_LOGGING_FILE_HH
#define CC_LOGGING_FILE_HH

#include <cstdio>
#include <string>
#include "com/centreon/logging/backend.hh"
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace logging {
/**
 *  @class file file.hh "com/centreon/logging/file.hh"
 *  @brief Log messages to file.
 */
class file : public backend {
 public:
  file(FILE* file,
       bool is_sync = true,
       bool show_pid = true,
       time_precision show_timestamp = second,
       bool show_thread_id = false,
       uint64_t max_size = 0);
  file(std::string const& path,
       bool is_sync = true,
       bool show_pid = true,
       time_precision show_timestamp = second,
       bool show_thread_id = false,
       uint64_t max_size = 0);
  virtual ~file() noexcept;
  void close() noexcept;
  std::string const& filename() const noexcept;
  void log(uint64_t types,
           uint32_t verbose,
           char const* msg,
           uint32_t size) noexcept;
  void open();
  void reopen();

 protected:
  virtual void _max_size_reached();

 private:
  file(file const& right);
  file& operator=(file const& right);
  void _flush() noexcept;

  uint64_t _max_size;
  std::string _path;
  FILE* _out;
  uint64_t _size;
};
}  // namespace logging

CC_END()

#endif  // !CC_LOGGING_FILE_HH
