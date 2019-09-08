/*
** Copyright 2011-2015,2017 Centreon
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

#ifndef CCB_FILE_FIFO_HH
#define CCB_FILE_FIFO_HH

#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace file {
/**
 *  @class fifo fifo.hh "com/centreon/broker/file/fifo.hh"
 *  @brief Fifo utility class.
 *
 *  This class manage a fifo.
 */
class fifo {
 public:
  fifo(std::string const& path);
  ~fifo();

  std::string read_line(int usecs_timeout = 3000000);

 private:
  void _open_fifo();

  std::string _path;
  int _fd;

  std::string _polled_line;
};
}  // namespace file

CCB_END()

#endif  // !CCB_FILE_FIFO_HH
