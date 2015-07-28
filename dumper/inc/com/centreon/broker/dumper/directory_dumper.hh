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

#ifndef CCB_DUMPER_DIRECTORY_DUMPER_HH
#  define CCB_DUMPER_DIRECTORY_DUMPER_HH

#  include <map>
#  include <QMutex>
#  include <QString>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/persistent_cache.hh"
#  include "com/centreon/broker/dumper/dump.hh"
#  include "com/centreon/broker/file/directory_watcher.hh"
#  include "com/centreon/broker/dumper/timestamp_cache.hh"

CCB_BEGIN()

namespace              dumper {
  /**
   *  @class directory_dumper directory_dumper.hh "com/centreon/broker/dumper/directory_dumper.hh"
   *  @brief Directory dumper stream.
   *
   *  Send the files of a directory on command.
   */
  class                directory_dumper : public io::stream {
  public:
                       directory_dumper(
                         std::string const& name,
                         std::string const& path,
                         std::string const& tagname,
                         misc::shared_ptr<persistent_cache> cache);
                       ~directory_dumper();
    bool               read(
                         misc::shared_ptr<io::data>& d,
                         time_t deadline);
    unsigned int       write(misc::shared_ptr<io::data> const& d);

  private:
                       directory_dumper(directory_dumper const& s);
    directory_dumper&  operator=(directory_dumper const& s);

    QMutex             _mutex;
    QString            _name;
    std::string        _path;
    std::string        _tagname;
    misc::shared_ptr<persistent_cache>
                       _cache;

    std::map<std::string, timestamp_cache>
                        _files_cache;

    void               _dump_dir(std::string const& path);

    void               _load_cache();
    void               _save_cache();
  };
}

CCB_END()

#endif // !CCB_DUMPER_DIRECTORY_DUMPER_HH
