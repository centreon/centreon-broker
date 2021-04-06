/*
** Copyright 2014-2015, 2020-2021 Centreon
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

#ifndef CCB_BAM_CONNECTOR_HH
#define CCB_BAM_CONNECTOR_HH

#include <memory>
#include <string>

#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class connector connector.hh "com/centreon/broker/bam/connector.hh"
 *  @brief Connect to a database.
 *
 *  Send perfdata in a Centreon bam database.
 */
class connector : public io::endpoint {
 public:
  connector();
  ~connector() noexcept = default;
  connector(const connector&) = delete;
  connector& operator=(const connector&) = delete;
  void connect_monitoring(std::string const& ext_cmd_file,
                          database_config const& db_cfg,
                          std::string const& storage_db_name,
                          std::shared_ptr<persistent_cache> cache);
  void connect_reporting(database_config const& db_cfg);
  std::unique_ptr<io::stream> open() override;

 private:
  enum stream_type { bam_monitoring_type = 1, bam_reporting_type };

  database_config _db_cfg;
  std::string _ext_cmd_file;
  std::string _storage_db_name;
  stream_type _type;
  std::shared_ptr<persistent_cache> _cache;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_CONNECTOR_HH
