/*
** Copyright 2014-2017 Centreon
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

#include "com/centreon/broker/bam/monitoring_stream.hh"

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <limits>
#include <sstream>

#include "com/centreon/broker/bam/ba_status.hh"
#include "com/centreon/broker/bam/configuration/reader.hh"
#include "com/centreon/broker/bam/configuration/reader_v2.hh"
#include "com/centreon/broker/bam/configuration/state.hh"
#include "com/centreon/broker/bam/event_cache_visitor.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/bam/kpi_status.hh"
#include "com/centreon/broker/bam/meta_service_status.hh"
#include "com/centreon/broker/bam/rebuild.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/timestamp.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;
using namespace com::centreon::broker::database;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Constructor.
 *
 *  @param[in] ext_cmd_file    The command file to write into.
 *  @param[in] db_cfg          Main (centreon) database configuration.
 *  @param[in] storage_db_cfg  Storage (centreon_storage) database
 *                             configuration.
 *  @param[in] cache           The persistent cache.
 */
monitoring_stream::monitoring_stream(std::string const& ext_cmd_file,
                                     database_config const& db_cfg,
                                     database_config const& storage_db_cfg,
                                     std::shared_ptr<persistent_cache> cache)
    : io::stream("BAM"),
      _ext_cmd_file(ext_cmd_file),
      _mysql(db_cfg),
      _pending_events(0),
      _storage_db_cfg(storage_db_cfg),
      _cache(cache) {
  // Prepare queries.
  _prepare();

  // Simulate a configuration update.
  update();
  // Read cache.
  _read_cache();
}

/**
 *  Destructor.
 */
monitoring_stream::~monitoring_stream() {
  // save cache
  try {
    _write_cache();
  } catch (std::exception const& e) {
    logging::error(logging::medium)
        << "BAM: can't save cache: '" << e.what() << "'";
  }
  log_v2::sql()->debug("bam: monitoring_stream destruction");
}

/**
 *  Flush data.
 *
 *  @return Number of acknowledged events.
 */
int monitoring_stream::flush() {
  _mysql.commit();
  int retval = _pending_events;
  _pending_events = 0;
  return retval;
}

/**
 *  Generate default state.
 */
void monitoring_stream::initialize() {
  multiplexing::publisher pblshr;
  event_cache_visitor ev_cache;
  _applier.visit(&ev_cache);
  ev_cache.commit_to(pblshr);
}

/**
 *  Read from the datbase.
 *  Get the next available bam event.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool monitoring_stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw(exceptions::shutdown() << "cannot read from BAM monitoring stream");
  return true;
}
/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void monitoring_stream::statistics(json11::Json::object& tree) const {
  std::lock_guard<std::mutex> lock(_statusm);
  if (!_status.empty())
    tree["status"] = _status;
}

/**
 *  Rebuild index and metrics cache.
 */
void monitoring_stream::update() {
  try {
    configuration::state s;
    configuration::reader_v2 r(_mysql, _storage_db_cfg);
    r.read(s);
    _applier.apply(s);
    _ba_mapping = s.get_ba_svc_mapping();
    _meta_mapping = s.get_meta_svc_mapping();
    _rebuild();
    initialize();
  } catch (std::exception const& e) {
    throw(exceptions::msg()
          << "BAM: could not process configuration update: " << e.what());
  }
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int monitoring_stream::write(std::shared_ptr<io::data> const& data) {
  // Take this event into account.
  ++_pending_events;
  if (!validate(data, get_name()))
    return 0;
  log_v2::bam()->trace("BAM: {} pending events", _pending_events);

  // Process service status events.
  switch (data->type()) {
    case neb::service_status::static_type():
    case neb::service::static_type(): {
      std::shared_ptr<neb::service_status> ss(
          std::static_pointer_cast<neb::service_status>(data));
      log_v2::bam()->trace(
          "BAM: processing service status (host {}, service {}, hard state {}, "
          "current state {})",
          ss->host_id, ss->service_id, ss->last_hard_state, ss->current_state);
      multiplexing::publisher pblshr;
      event_cache_visitor ev_cache;
      _applier.book_service().update(ss, &ev_cache);
      ev_cache.commit_to(pblshr);
    } break;
    case neb::acknowledgement::static_type(): {
      std::shared_ptr<neb::acknowledgement> ack(
          std::static_pointer_cast<neb::acknowledgement>(data));
      log_v2::bam()->trace(
          "BAM: processing acknowledgement (host {}, service {})", ack->host_id,
          ack->service_id);
      multiplexing::publisher pblshr;
      event_cache_visitor ev_cache;
      _applier.book_service().update(ack, &ev_cache);
      ev_cache.commit_to(pblshr);
    } break;
    case neb::downtime::static_type(): {
      std::shared_ptr<neb::downtime> dt(
          std::static_pointer_cast<neb::downtime>(data));
      log_v2::bam()->trace("BAM: processing downtime (host {}, service {})",
                           dt->host_id, dt->service_id);
      multiplexing::publisher pblshr;
      event_cache_visitor ev_cache;
      _applier.book_service().update(dt, &ev_cache);
      ev_cache.commit_to(pblshr);
    } break;
    case storage::metric::static_type(): {
      std::shared_ptr<storage::metric> m(
          std::static_pointer_cast<storage::metric>(data));
      log_v2::bam()->trace("BAM: processing metric (id {}, time {}, value {})",
                           m->metric_id, m->ctime, m->value);
      multiplexing::publisher pblshr;
      event_cache_visitor ev_cache;
      _applier.book_metric().update(m, &ev_cache);
      ev_cache.commit_to(pblshr);
    } break;
    case bam::ba_status::static_type(): {
      ba_status* status(static_cast<ba_status*>(data.get()));
      logging::debug(logging::low)
          << "BAM: processing BA status (id " << status->ba_id << ", level "
          << status->level_nominal << ", acknowledgement "
          << status->level_acknowledgement << ", downtime "
          << status->level_downtime << ")";
      _ba_update.bind_value_as_f64(0, status->level_nominal);
      _ba_update.bind_value_as_f64(1, status->level_acknowledgement);
      _ba_update.bind_value_as_f64(2, status->level_downtime);
      _ba_update.bind_value_as_u32(6, status->ba_id);
      if (status->last_state_change == (time_t)-1 ||
          status->last_state_change == 0)
        _ba_update.bind_value_as_null(3);
      else
        _ba_update.bind_value_as_u64(3, status->last_state_change.get_time_t());
      _ba_update.bind_value_as_bool(4, status->in_downtime);
      _ba_update.bind_value_as_i32(5, status->state);

      _mysql.run_statement(_ba_update, database::mysql_error::update_ba, true);

      if (status->state_changed) {
        std::pair<std::string, std::string> ba_svc_name(
            _ba_mapping.get_service(status->ba_id));
        if (ba_svc_name.first.empty() || ba_svc_name.second.empty()) {
          logging::error(logging::high)
              << "BAM: could not trigger check of virtual service of BA "
              << status->ba_id
              << ": host name and service description were not found";
        } else {
          time_t now = time(nullptr);
          std::string cmd(fmt::format("[{}] SCHEDULE_FORCED_SVC_CHECK;{};{};{}",
                                      now, ba_svc_name.first,
                                      ba_svc_name.second, now));
          _write_external_command(cmd);
        }
      }
    } break;
    case bam::kpi_status::static_type(): {
      kpi_status* status(static_cast<kpi_status*>(data.get()));
      logging::debug(logging::low)
          << "BAM: processing KPI status (id " << status->kpi_id << ", level "
          << status->level_nominal_hard << ", acknowledgement "
          << status->level_acknowledgement_hard << ", downtime "
          << status->level_downtime_hard << ")";

      _kpi_update.bind_value_as_f64(0, status->level_acknowledgement_hard);
      _kpi_update.bind_value_as_i32(1, status->state_hard);
      _kpi_update.bind_value_as_f64(2, status->level_downtime_hard);
      _kpi_update.bind_value_as_f64(3, status->level_nominal_hard);
      _kpi_update.bind_value_as_i32(4, 1 + 1);
      if (status->last_state_change == (time_t)-1 ||
          status->last_state_change == 0)
        _kpi_update.bind_value_as_null(5);
      else
        _kpi_update.bind_value_as_u64(5,
                                      status->last_state_change.get_time_t());
      _kpi_update.bind_value_as_f64(6, status->last_impact);
      _kpi_update.bind_value_as_bool(7, status->valid);
      _kpi_update.bind_value_as_bool(8, status->in_downtime);
      _kpi_update.bind_value_as_u32(9, status->kpi_id);

      _mysql.run_statement(_kpi_update, database::mysql_error::update_kpi,
                           true);
    } break;
    case inherited_downtime::static_type(): {
      std::string cmd;
      timestamp now = timestamp::now();
      inherited_downtime const& dwn =
          *std::static_pointer_cast<inherited_downtime const>(data);
      if (dwn.in_downtime)
        cmd = fmt::format(
            "[{}] "
            "SCHEDULE_SVC_DOWNTIME;_Module_BAM_{};ba_{};{};{};1;0;0;Centreon "
            "Broker BAM Module;Automatic downtime triggered by BA downtime "
            "inheritance",
            now, config::applier::state::instance().poller_id(), dwn.ba_id, now,
            std::numeric_limits<int32_t>::max());
      else
        cmd = fmt::format(
            "[{}] DEL_SVC_DOWNTIME_FULL;_Module_BAM_{};ba_{};;{};1;0;;Centreon "
            "Broker BAM Module;Automatic downtime triggered by BA downtime "
            "inheritance",
            now, config::applier::state::instance().poller_id(), dwn.ba_id,
            std::numeric_limits<int32_t>::max());
      _write_external_command(cmd);
    } break;
    default:
      break;
  }

  // Event acknowledgement.
  return 0;
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Prepare queries.
 */
void monitoring_stream::_prepare() {
  // BA status.
  {
    std::string query(
        "UPDATE mod_bam SET current_level=?,acknowledged=?,downtime=?,"
        "last_state_change=?,in_downtime=?,current_status=? WHERE ba_id=?");
    _ba_update = _mysql.prepare_query(query);
  }

  // KPI status.
  {
    std::string query(
        "UPDATE mod_bam_kpi SET acknowledged=?,current_status=?,"
        "downtime=?, last_level=?,state_type=?,last_state_change=?,"
        "last_impact=?, valid=?,in_downtime=? WHERE kpi_id=?");
    _kpi_update = _mysql.prepare_query(query);
  }
}

/**
 *  Rebuilds BA durations/availibities from BA events.
 */
void monitoring_stream::_rebuild() {
  // Get the list of the BAs that should be rebuild.
  std::vector<uint32_t> bas_to_rebuild;
  {
    std::string query("SELECT ba_id FROM mod_bam WHERE must_be_rebuild='1'");
    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result(query, &promise);
    try {
      mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res))
        bas_to_rebuild.push_back(res.value_as_u32(0));
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "BAM: could not select the list of BAs to rebuild: " << e.what();
    }
  }

  // Nothing to rebuild.
  if (bas_to_rebuild.empty())
    return;

  logging::debug(logging::medium)
      << "BAM: rebuild asked, sending the rebuild signal";

  std::shared_ptr<rebuild> r(std::make_shared<rebuild>(
      fmt::format("{}", fmt::join(bas_to_rebuild, ", "))));
  std::unique_ptr<io::stream> out(new multiplexing::publisher);
  out->write(r);

  // Set all the BAs to should not be rebuild.
  {
    std::string query("UPDATE mod_bam SET must_be_rebuild='0'");
    _mysql.run_query(query, database::mysql_error::rebuild_ba);
  }
}

/**
 *  Update status of endpoint.
 *
 *  @param[in] status New status.
 */
void monitoring_stream::_update_status(std::string const& status) {
  std::lock_guard<std::mutex> lock(_statusm);
  _status = status;
}

/**
 *  Write an external command to Engine.
 *
 *  @param[in] cmd  Command to write to the external command pipe.
 */
void monitoring_stream::_write_external_command(std::string cmd) {
  cmd.append("\n");
  std::ofstream ofs;
  ofs.open(_ext_cmd_file.c_str());
  if (!ofs.good()) {
    logging::error(logging::medium)
        << "BAM: could not write BA check result to command file '"
        << _ext_cmd_file << "'";
  } else {
    ofs.write(cmd.c_str(), cmd.size());
    if (!ofs.good())
      logging::error(logging::medium)
          << "BAM: could not write BA check result to command file '"
          << _ext_cmd_file << "'";
    else
      logging::debug(logging::medium)
          << "BAM: sent external command '" << cmd << "'";
    ofs.close();
  }
}

/**
 *  Get inherited downtime from the cache.
 */
void monitoring_stream::_read_cache() {
  if (_cache == nullptr)
    return;

  _applier.load_from_cache(*_cache);
}

/**
 *  Save inherited downtime to the cache.
 */
void monitoring_stream::_write_cache() {
  if (_cache == nullptr) {
    logging::debug(logging::medium) << "BAM: no cache configured";
    return;
  }

  logging::debug(logging::medium) << "BAM: loading cache";

  _applier.save_to_cache(*_cache);
}
