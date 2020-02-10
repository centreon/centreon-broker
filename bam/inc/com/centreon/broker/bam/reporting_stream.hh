/*
** Copyright 2014-2015 Centreon
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

#ifndef CCB_BAM_REPORTING_STREAM_HH
#define CCB_BAM_REPORTING_STREAM_HH

#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "com/centreon/broker/bam/availability_thread.hh"
#include "com/centreon/broker/bam/ba_event.hh"
#include "com/centreon/broker/bam/timeperiod_map.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/time/timeperiod.hh"

CCB_BEGIN()

// Forward declaration.
class database_config;

namespace bam {
// Forward declarations.
class dimension_timeperiod;
class dimension_timeperiod_exception;
class dimension_timeperiod_exclusion;

/**
 *  @class reporting_stream reporting_stream.hh
 * "com/centreon/broker/bam/reporting_stream.hh"
 *  @brief bam reporting_stream.
 *
 *  Handle perfdata and insert proper informations in index_data and
 *  metrics table of a centbam DB.
 */
class reporting_stream : public io::stream {
 public:
  reporting_stream(database_config const& db_cfg);
  ~reporting_stream();
  int flush();
  bool read(std::shared_ptr<io::data>& d, time_t deadline);
  void statistics(json11::Json::object& tree) const;
  int write(std::shared_ptr<io::data> const& d);

 private:
  reporting_stream(reporting_stream const& other);
  reporting_stream& operator=(reporting_stream const& other);
  void _apply(dimension_timeperiod const& tp);
  void _apply(dimension_timeperiod_exception const& tpe);
  void _apply(dimension_timeperiod_exclusion const& tpe);
  void _close_inconsistent_events(char const* event_type,
                                  char const* table,
                                  char const* id);
  void _close_all_events();
  void _load_timeperiods();
  void _prepare();
  void _process_ba_event(std::shared_ptr<io::data> const& e);
  void _process_ba_duration_event(std::shared_ptr<io::data> const& e);
  void _process_kpi_event(std::shared_ptr<io::data> const& e);
  void _process_dimension(std::shared_ptr<io::data> const& e);
  void _dimension_dispatch(std::shared_ptr<io::data> const& e);
  std::shared_ptr<io::data> _dimension_copy(std::shared_ptr<io::data> const& e);
  void _process_dimension_ba(std::shared_ptr<io::data> const& e);
  void _process_dimension_bv(std::shared_ptr<io::data> const& e);
  void _process_dimension_ba_bv_relation(std::shared_ptr<io::data> const& e);
  void _process_dimension_truncate_signal(std::shared_ptr<io::data> const& e);
  void _process_dimension_kpi(std::shared_ptr<io::data> const& e);
  void _process_dimension_timeperiod(std::shared_ptr<io::data> const& e);
  void _process_dimension_timeperiod_exception(
      std::shared_ptr<io::data> const& e);
  void _process_dimension_timeperiod_exclusion(
      std::shared_ptr<io::data> const& e);
  void _process_dimension_ba_timeperiod_relation(
      std::shared_ptr<io::data> const& e);
  void _process_rebuild(std::shared_ptr<io::data> const& e);
  void _update_status(std::string const& status);
  void _compute_event_durations(std::shared_ptr<ba_event> const& ev,
                                io::stream* visitor);

  uint32_t _ack_events;
  uint32_t _pending_events;
  uint32_t _queries_per_transaction;
  std::string _status;
  mutable std::mutex _statusm;
  uint32_t _transaction_queries;
  mysql _mysql;
  database::mysql_stmt _ba_full_event_insert;
  database::mysql_stmt _ba_event_update;
  database::mysql_stmt _ba_duration_event_insert;
  database::mysql_stmt _ba_duration_event_update;
  database::mysql_stmt _kpi_full_event_insert;
  database::mysql_stmt _kpi_event_update;
  database::mysql_stmt _kpi_event_link;
  database::mysql_stmt _kpi_event_link_update;
  database::mysql_stmt _dimension_ba_insert;
  database::mysql_stmt _dimension_bv_insert;
  database::mysql_stmt _dimension_ba_bv_relation_insert;
  database::mysql_stmt _dimension_timeperiod_insert;
  database::mysql_stmt _dimension_timeperiod_exception_insert;
  database::mysql_stmt _dimension_timeperiod_exclusion_insert;
  database::mysql_stmt _dimension_ba_timeperiod_insert;
  database::mysql_stmt _dimension_kpi_insert;
  std::vector<database::mysql_stmt> _dimension_truncate_tables;
  std::unique_ptr<availability_thread> _availabilities;

  // Timeperiods by BAs, with an option is default timeperiod.
  timeperiod_map _timeperiods;

  std::vector<std::shared_ptr<io::data>> _dimension_data_cache;
  std::unordered_map<uint32_t, std::map<std::time_t, uint64_t>>
      _last_inserted_kpi;  // ba_id => <time, row>
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_REPORTING_STREAM_HH
