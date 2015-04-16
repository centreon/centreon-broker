/*
** Copyright 2015 Merethis
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

#ifndef CCB_FILE_DIRECTORY_WATCHER_HH
#  define CCB_FILE_DIRECTORY_WATCHER_HH

#  include <string>
#  include <map>
#  include <vector>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/file/directory_event.hh"

CCB_BEGIN()

namespace       file {
  /**
   *  @class directory_watcher directory_watcher.hh "com/centreon/broker/file/directory_watcher.hh"
   *  @brief Directory dumper stream.
   *
   *  Watch over directories for files modifications.
   */
  class         directory_watcher {
  public:
                directory_watcher();
                ~directory_watcher();

    void        add_directory(std::string const& directory);
    void        remove_directory(std::string const& directory);

    std::vector<directory_event>
                get_events();
    void        set_timeout(unsigned int msecs);

  private:
                directory_watcher(directory_watcher const& o);
    directory_watcher&
                operator=(directory_watcher const& o);

    int         _inotify_instance_id;
    unsigned int
                _timeout;
    std::map<std::string, int>
                _path_to_id;
    std::map<int, std::string>
                _id_to_path;
  };
}

CCB_END()

#endif // !CCB_FILE_DIRECTORY_WATCHER_HH
