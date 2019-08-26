/*
** Copyright 2015 Centreon
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

#ifndef CCB_DUMPER_DIRECTORY_DUMPER_HH
#  define CCB_DUMPER_DIRECTORY_DUMPER_HH

#  include <QMutex>
#  include <map>
#  include <memory>
#  include <string>
#  include "com/centreon/broker/io/stream.hh"
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
                         std::shared_ptr<persistent_cache> cache);
                       ~directory_dumper();
    bool               read(
                         std::shared_ptr<io::data>& d,
                         time_t deadline);
    int                write(std::shared_ptr<io::data> const& d);

  private:
                       directory_dumper(directory_dumper const& s);
    directory_dumper&  operator=(directory_dumper const& s);

    QMutex             _mutex;
    std::string        _name;
    std::string        _path;
    std::string        _tagname;
    std::shared_ptr<persistent_cache>
                       _cache;

    std::map<std::string, timestamp_cache>
                        _files_cache;
    std::map<std::string, unsigned int>
                        _command_to_poller_id;

    void               _dump_dir(
                         std::string const& path,
                         std::string const& req_id);

    void               _load_cache();
    void               _save_cache();
  };
}

CCB_END()

#endif // !CCB_DUMPER_DIRECTORY_DUMPER_HH
