/*
** Copyright 2011-2013,2015 Centreon
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

#ifndef CCB_COMPRESSION_STREAM_HH
#  define CCB_COMPRESSION_STREAM_HH

#  include <QByteArray>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        compression {
  /**
   *  @class stream stream.hh "com/centreon/broker/compression/stream.hh"
   *  @brief Compression stream.
   *
   *  Compress and uncompress data.
   */
  class          stream : public io::stream {
  public:
                 stream(
                   int level = -1,
                   unsigned int size = 0);
                 stream(stream const& other);
                 ~stream();
    stream&      operator=(stream const& other);
    int          flush();
    bool         read(
                   misc::shared_ptr<io::data>& d,
                   time_t deadline = (time_t)-1);
    void         statistics(io::properties& tree) const;
    int          write(misc::shared_ptr<io::data> const& d);

  private:
    void         _flush();
    bool         _get_data(
                   unsigned int size,
                   time_t timeout);
    void         _internal_copy(stream const& other);

    int          _level;
    QByteArray   _rbuffer;
    unsigned int _size;
    QByteArray   _wbuffer;
  };
}

CCB_END()

#endif // !CCB_COMPRESSION_STREAM_HH
