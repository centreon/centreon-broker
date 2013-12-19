/*
** Copyright 2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_RRD_CREATOR_HH
#  define CCB_RRD_CREATOR_HH

#  include <ctime>
#  include <map>
#  include <string>
#  include <sys/types.h>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         rrd {
  /**
   *  @class creator creator.hh "com/centreon/broker/rrd/creator.hh"
   *  @brief RRD creator.
   *
   *  Create RRD objects.
   */
  class            creator {
  public:
                   creator(
                     std::string const& tmpl_path,
                     unsigned int cache_size);
                   creator(creator const& right);
                   ~creator();
    creator&       operator=(creator const& right);
    void           clear();
    void           create(
                     std::string const& filename,
                     unsigned int length,
                     time_t from,
                     unsigned int step,
                     short value_type);

  private:
    struct         tmpl_info {
      bool         operator<(tmpl_info const& right) const {
        if (length != right.length)
          return (length < right.length);
        if (step != right.step)
          return (step < right.step);
        return (value_type < right.value_type);
      }
      unsigned int length;
      unsigned int step;
      short        value_type;
    };

    struct         fd_info {
      int          fd;
      off_t        size;
    };

    void           _duplicate(
                     std::string const& filename,
                     fd_info const& in_fd);
    void           _open(
                     std::string const& filename,
                     unsigned int length,
                     time_t from,
                     unsigned int step,
                     short value_type);
    void           _read_write(
                     int out_fd,
                     int in_fd,
                     ssize_t size,
                     std::string const& filename);
    void           _sendfile(
                     int out_fd,
                     int in_fd,
                     off_t already_transferred,
                     ssize_t size,
                     std::string const& filename);

    unsigned int   _cache_size;
    std::map<tmpl_info, fd_info>
                   _fds;
    std::string    _tmpl_path;
  };
}

CCB_END()

#endif // !CCB_RRD_CREATOR_HH
