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

#ifndef CCB_DUMPER_DIRECTORY_DUMPER_HH
#  define CCB_DUMPER_DIRECTORY_DUMPER_HH

#  include <QMutex>
#  include <list>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/persistent_cache.hh"
#  include "com/centreon/broker/dumper/dump.hh"
#  include "com/centreon/broker/dumper/directory_watcher.hh"
#  include "com/centreon/broker/dumper/timestamp_cache.hh"

CCB_BEGIN()

namespace              dumper {
  /**
   *  @class directory_dumper directory_dumper.hh "com/centreon/broker/dumper/directory_dumper.hh"
   *  @brief Directory dumper stream.
   *
   *  Watch over directories for files modifications, send them.
   */
  class                directory_dumper : public io::stream {
  public:
                       directory_dumper(
                         std::string const& path,
                         std::string const& tagname,
                         misc::shared_ptr<persistent_cache> cache);
                       ~directory_dumper();
    void               process(
                         bool in = false,
                         bool out = false);
    void               read(misc::shared_ptr<io::data>& d);
    unsigned int       write(misc::shared_ptr<io::data> const& d);

  private:
                       directory_dumper(directory_dumper const& s);
    directory_dumper&  operator=(directory_dumper const& s);

    QMutex             _mutex;
    std::string        _path;
    bool               _process_in;
    bool               _process_out;
    std::string        _tagname;
    misc::shared_ptr<persistent_cache>
                       _cache;

    std::list<std::pair<timestamp, misc::shared_ptr<dump> > >
                       _event_list;
    directory_watcher  _watcher;

    std::map<std::string, timestamp>
                        _last_modified_timestamps;

    void                _get_last_timestamps_from_cache();
    void                _save_last_timestamps_to_cache();
    void               _set_watch_over_directory();
    std::pair<timestamp, misc::shared_ptr<dump> >
                        _dump_a_file(std::string const& path);
  };
}

CCB_END()

#endif // !CCB_DUMPER_DIRECTORY_DUMPER_HH
