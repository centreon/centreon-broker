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

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <memory>

#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

class log_v2 {
  static std::map<std::string, spdlog::level::level_enum> _levels_map;
  std::string _log_name;
  std::shared_ptr<spdlog::logger> _bam_log;
  std::shared_ptr<spdlog::logger> _bbdo_log;
  std::shared_ptr<spdlog::logger> _config_log;
  std::shared_ptr<spdlog::logger> _core_log;
  std::shared_ptr<spdlog::logger> _graphite_log;
  std::shared_ptr<spdlog::logger> _notification_log;
  std::shared_ptr<spdlog::logger> _rrd_log;
  std::shared_ptr<spdlog::logger> _stats_log;
  std::shared_ptr<spdlog::logger> _influxdb_log;
  std::shared_ptr<spdlog::logger> _lua_log;
  std::shared_ptr<spdlog::logger> _neb_log;
  std::shared_ptr<spdlog::logger> _perfdata_log;
  std::shared_ptr<spdlog::logger> _processing_log;
  std::shared_ptr<spdlog::logger> _sql_log;
  std::shared_ptr<spdlog::logger> _tcp_log;
  std::shared_ptr<spdlog::logger> _tls_log;
  std::mutex _load_m;

  log_v2();
  ~log_v2();

 public:
  static const std::array<std::string, 16> loggers;

  static log_v2& instance();
  void apply(const config::state& conf);
  const std::string& log_name() const;

  static std::shared_ptr<spdlog::logger> bam();
  static std::shared_ptr<spdlog::logger> bbdo();
  static std::shared_ptr<spdlog::logger> config();
  static std::shared_ptr<spdlog::logger> core();
  static std::shared_ptr<spdlog::logger> influxdb();
  static std::shared_ptr<spdlog::logger> graphite();
  static std::shared_ptr<spdlog::logger> notification();
  static std::shared_ptr<spdlog::logger> rrd();
  static std::shared_ptr<spdlog::logger> stats();
  static std::shared_ptr<spdlog::logger> lua();
  static std::shared_ptr<spdlog::logger> neb();
  static std::shared_ptr<spdlog::logger> perfdata();
  static std::shared_ptr<spdlog::logger> processing();
  static std::shared_ptr<spdlog::logger> sql();
  static std::shared_ptr<spdlog::logger> tcp();
  static std::shared_ptr<spdlog::logger> tls();
  static std::list<std::string> levels();
};

CCB_END();

#endif  // CENTREON_BROKER_CORE_INC_COM_CENTREON_BROKER_LOG_V2_HH_
