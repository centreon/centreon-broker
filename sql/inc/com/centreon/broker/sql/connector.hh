/*
** Copyright 2011-2012,2015-2021 Centreon
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

#ifndef CCB_SQL_CONNECTOR_HH
#define CCB_SQL_CONNECTOR_HH

#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace sql {
/**
 *  @class connector connector.hh "com/centreon/broker/sql/connector.hh"
 *  @brief Connect to a database.
 *
 *  Send events to a SQL database.
 */
class connector : public io::endpoint {
  uint32_t _cleanup_check_interval;
  database_config _dbcfg;
  uint32_t _loop_timeout;
  uint32_t _instance_timeout;
  bool _with_state_events;
  bool _enable_cmd_cache;

 public:
  connector();
  ~connector() noexcept = default;
  connector(const connector&) = delete;
  connector& operator=(const connector&) = delete;
  void connect_to(database_config const& dbcfg,
                  uint32_t cleanup_check_interval = 0,
                  uint32_t loop_timeout = 10,
                  uint32_t instance_timeout = 15,
                  bool with_state_events = false,
                  bool enable_command_cache = false);
  std::unique_ptr<io::stream> open() override;
};
}  // namespace sql

CCB_END()

#endif  // !CCB_SQL_CONNECTOR_HH
