/*
** Copyright 2014-2020 Centreon
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

#ifndef CCB_BAM_MONITORING_STREAM_HH
#define CCB_BAM_MONITORING_STREAM_HH

#include <mutex>
#include <string>

#include "com/centreon/broker/bam/configuration/applier/state.hh"
#include "com/centreon/broker/database/mysql_stmt.hh"
#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class monitoring_stream monitoring_stream.hh
 * "com/centreon/broker/bam/monitoring_stream.hh"
 *  @brief bam monitoring_stream.
 *
 *  Handle perfdata and insert proper informations in index_data and
 *  metrics table of a centbam DB.
 */
class monitoring_stream : public io::stream {
 public:
  monitoring_stream(std::string const& ext_cmd_file,
                    database_config const& db_cfg,
                    database_config const& storage_db_cfg,
                    std::shared_ptr<persistent_cache> cache);
  ~monitoring_stream();
  int flush();
  void initialize();
  bool read(std::shared_ptr<io::data>& d, time_t deadline);
  void statistics(json11::Json::object& tree) const;
  void update();
  int write(std::shared_ptr<io::data> const& d);

 private:
  monitoring_stream(monitoring_stream const& other);
  monitoring_stream& operator=(monitoring_stream const& other);
  void _check_replication();
  void _prepare();
  void _rebuild();
  void _update_status(std::string const& status);
  void _write_external_command(std::string cmd);

  void _read_cache();
  void _write_cache();

  configuration::applier::state _applier;
  std::string _status;
  std::string _ext_cmd_file;
  ba_svc_mapping _ba_mapping;
  ba_svc_mapping _meta_mapping;
  mutable std::mutex _statusm;
  mysql _mysql;
  database::mysql_stmt _ba_update;
  database::mysql_stmt _kpi_update;
  database::mysql_stmt _meta_service_update;
  int _pending_events;
  database_config _storage_db_cfg;
  std::shared_ptr<persistent_cache> _cache;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_MONITORING_STREAM_HH
