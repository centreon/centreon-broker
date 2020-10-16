/*
** Copyright 2020 Centreon
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
#ifndef CENTREON_BROKER_CORE_INC_COM_CENTREON_BROKER_POOL_HH_
#define CENTREON_BROKER_CORE_INC_COM_CENTREON_BROKER_POOL_HH_

#include <asio.hpp>
#include <cstdlib>

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

class pool {
  static size_t _pool_size;
  asio::io_context _io_context;
  asio::io_service::work _worker;
  std::vector<std::thread> _pool;
  bool _closed;
  mutable std::mutex _closed_m;

  pool();
  void _start();
  void _stop();

 public:
  ~pool() noexcept;
  pool(const pool&) = delete;
  pool& operator=(const pool&) = delete;

  static void set_size(size_t size) noexcept;
  static pool& instance();
  static asio::io_context& io_context();
  size_t get_current_size() const;
};

CCB_END()

#endif  // CENTREON_BROKER_CORE_INC_COM_CENTREON_BROKER_POOL_HH_
