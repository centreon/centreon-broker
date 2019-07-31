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

#ifndef CCB_DUMPER_STREAM_HH
#  define CCB_DUMPER_STREAM_HH

#  include <map>
#  include <memory>
#  include <vector>
#  include <QMutex>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              dumper {
  // Forward declaration.
  class                dump;
  class                directory_dump;
  class                remove;

  /**
   *  @class stream stream.hh "com/centreon/broker/dumper/stream.hh"
   *  @brief Dumper stream.
   *
   *  Read and write data to a stream.
   */
  class                stream : public io::stream {
  public:
                       stream(
                         std::string const& path,
                         std::string const& tagname);
                       ~stream();
    bool               read(
                         std::shared_ptr<io::data>& d,
                         time_t deadline);
    int                write(std::shared_ptr<io::data> const& d);

  private:
                       stream(stream const& s);
    stream&            operator=(stream const& s);

    void               _process_dump_event(dump const& dmp);
    void               _process_remove_event(remove const& rmv);
    void               _process_directory_dump_event(directory_dump const& dd);
    void               _add_to_directory_cache(
                         std::string const& req_id,
                         std::shared_ptr<io::data> event);

    QMutex             _mutex;
    std::string        _path;
    std::string        _tagname;

    typedef std::map<
                   std::string,
                   std::vector<std::shared_ptr<io::data> > >
      directory_dump_cache;
      directory_dump_cache
                       _cached_directory_dump;
  };
}

CCB_END()

#endif // !CCB_DUMPER_STREAM_HH
