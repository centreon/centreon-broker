/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_RRD_CACHED_HH
#  define CCB_RRD_CACHED_HH

#  include <memory>
#  include <QIODevice>
#  include <string>
#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/rrd/backend.hh"
#  include "com/centreon/broker/rrd/lib.hh"

CCB_BEGIN()

namespace   rrd {
  /**
   *  @class cached cached.hh "com/centreon/broker/rrd/cached.hh"
   *  @brief Handle RRD file access through rrdcached.
   *
   *  Handle creation, deletion, tuning and update of an RRD file with
   *  rrdcached. The advantage of rrdcached is to have a batch mode that
   *  allows bulk operations.
   *
   *  @see begin()
   *  @see commit()
   */
  class     cached : public backend {
  public:
            cached(
              std::string const& tmpl_path,
              unsigned int cache_size);
            ~cached();
    void    begin();
    void    clean();
    void    close();
    void    commit();
#  if QT_VERSION >= 0x040400
    void    connect_local(std::string const& name);
#  endif // Qt >= 4.4.0
    void    connect_remote(
              std::string const& address,
              unsigned short port);
    void    open(std::string const& filename);
    void    open(
              std::string const& filename,
              unsigned int length,
              time_t from,
              unsigned int step,
              short value_type = 0);
    void    remove(std::string const& filename);
    void    update(time_t t, std::string const& value);

  private:
            cached(cached const& c);
    cached& operator=(cached const& c);
    void    _send_to_cached(
              char const* command,
              unsigned int size = 0);

    bool    _batch;
    std::string
            _filename;
    lib     _lib;
    std::unique_ptr<QIODevice>
            _socket;
  };
}

CCB_END()

#endif // !CCB_RRD_CACHED_HH
