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
#ifndef CENTREON_BROKER_CORE_INC_COM_CENTREON_BROKER_LOG_V2_HH_
#define CENTREON_BROKER_CORE_INC_COM_CENTREON_BROKER_LOG_V2_HH_

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/common.h>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/config/state.hh"

CCB_BEGIN()

class log_v2 {
 private:
  std::shared_ptr<spdlog::logger> _tcp_log;
  std::shared_ptr<spdlog::logger> _bbdo_log;
  std::shared_ptr<spdlog::logger> _tls_log;
  log_v2();

 public:
  static log_v2& instance();
  bool load(std::string const& file, config::state const& state);

  std::shared_ptr<spdlog::logger> tls();
  std::shared_ptr<spdlog::logger> bbdo();
  std::shared_ptr<spdlog::logger> tcp();
};

CCB_END();

#endif  // CENTREON_BROKER_CORE_INC_COM_CENTREON_BROKER_LOG_V2_HH_
