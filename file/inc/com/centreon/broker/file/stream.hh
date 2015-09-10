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

#ifndef CCB_FILE_STREAM_HH
#  define CCB_FILE_STREAM_HH

#  include <QMutex>
#  include "com/centreon/broker/file/cfile.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              file {
  /**
   *  @class stream stream.hh "com/centreon/broker/file/stream.hh"
   *  @brief File stream.
   *
   *  Read and write data to a stream.
   */
  class                stream : public io::stream {
  public:
                       stream(
                         std::string const& path,
                         unsigned long long max_size = 0);
                       ~stream();
    unsigned long long get_max_size() const throw ();
    void               process(
                         bool in = false,
                         bool out = false);
    void               read(misc::shared_ptr<io::data>& d);
    void               statistics(io::properties& tree) const;
    unsigned int       write(misc::shared_ptr<io::data> const& d);

  private:
                       stream(stream const& s);
    stream&            operator=(stream const& s);
    std::string        _file_path(unsigned int id) const;
    void               _open_first_read();
    void               _open_first_write();
    void               _open_next_read();
    void               _open_next_write(bool truncate = true);

    mutable unsigned long long
                       _last_read_offset;
    mutable time_t     _last_time;
    mutable unsigned long long
                       _last_write_offset;
    long               _max_size;
    QMutex             _mutex;
    std::string        _path;
    bool               _process_in;
    bool               _process_out;
    misc::shared_ptr<cfile>
                       _rfile;
    unsigned int       _rid;
    long               _roffset;
    misc::shared_ptr<cfile>
                       _wfile;
    unsigned int       _wid;
    long               _woffset;
  };
}

CCB_END()

#endif // !CCB_FILE_STREAM_HH
