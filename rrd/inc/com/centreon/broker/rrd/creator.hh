/*
** Copyright 2013 Centreon
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

#ifndef CCB_RRD_CREATOR_HH
#define CCB_RRD_CREATOR_HH

#include <sys/types.h>
#include <ctime>
#include <map>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace rrd {
/**
 *  @class creator creator.hh "com/centreon/broker/rrd/creator.hh"
 *  @brief RRD creator.
 *
 *  Create RRD objects.
 */
class creator {
 public:
  creator(std::string const& tmpl_path, unsigned int cache_size);
  creator(creator const& right);
  ~creator();
  creator& operator=(creator const& right);
  void clear();
  void create(std::string const& filename,
              unsigned int length,
              time_t from,
              unsigned int step,
              short value_type);

 private:
  struct tmpl_info {
    bool operator<(tmpl_info const& right) const {
      if (length != right.length)
        return (length < right.length);
      if (step != right.step)
        return (step < right.step);
      return (value_type < right.value_type);
    }
    unsigned int length;
    unsigned int step;
    short value_type;
  };

  struct fd_info {
    int fd;
    off_t size;
  };

  void _duplicate(std::string const& filename, fd_info const& in_fd);
  void _open(std::string const& filename,
             unsigned int length,
             time_t from,
             unsigned int step,
             short value_type);
  void _read_write(int out_fd,
                   int in_fd,
                   ssize_t size,
                   std::string const& filename);
#ifdef __linux__
  void _sendfile(int out_fd,
                 int in_fd,
                 off_t already_transferred,
                 ssize_t size,
                 std::string const& filename);
#endif  // Linux

  unsigned int _cache_size;
  std::map<tmpl_info, fd_info> _fds;
  std::string _tmpl_path;
};
}  // namespace rrd

CCB_END()

#endif  // !CCB_RRD_CREATOR_HH
