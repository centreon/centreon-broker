/*
** Copyright 2011-2015 Merethis
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

#ifndef CCB_FILE_FIFO_HH
#  define CCB_FILE_FIFO_HH

#  include <QMutex>
#  include "com/centreon/broker/file/cfile.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              file {
  /**
   *  @class fifo fifo.hh "com/centreon/broker/file/fifo.hh"
   *  @brief Fifo utility class.
   *
   *  This class manage a fifo.
   */
  class     fifo {
  public:
            fifo(std::string const& path);
            ~fifo();

    std::string
            read_line(int usecs_timeout = 3000000);

  private:
    std::string
            _path;
    int     _fd;

    std::string
            _polled_line;

    void    _open_fifo();
  };
}

CCB_END()

#endif // !CCB_FILE_FIFO_HH
