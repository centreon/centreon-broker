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
#define CCB_RRD_CACHED_HH

#include <asio.hpp>
#include <memory>
#include <string>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/rrd/backend.hh"
#include "com/centreon/broker/rrd/lib.hh"

CCB_BEGIN()

namespace rrd {
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
class cached : public backend {
 public:
  enum cached_type { uninitialized, local, tcp };
  cached(std::string const& tmpl_path, uint32_t cache_size, cached_type type);
  cached(cached const& c) = delete;
  cached& operator=(cached const& c) = delete;
  ~cached() = default;
  void begin();
  void clean();
  void close();
  void commit();
  void connect_local(std::string const& name);
  void connect_remote(std::string const& address, unsigned short port);
  void open(std::string const& filename);
  void open(std::string const& filename,
            uint32_t length,
            time_t from,
            uint32_t step,
            short value_type = 0);
  void remove(std::string const& filename);
  void update(time_t t, std::string const& value);

 private:
  template <typename T>
  void _send_to_cached(std::string const& command, T const& socket);

  bool _batch;
  std::string _filename;
  lib _lib;
  cached_type _type;
  asio::io_context _io_context;
  std::unique_ptr<asio::ip::tcp::socket> _tcp_socket;
  std::unique_ptr<asio::local::stream_protocol::socket> _local_socket;
};
}  // namespace rrd

CCB_END()

#endif  // !CCB_RRD_CACHED_HH
