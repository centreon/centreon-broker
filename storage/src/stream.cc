/*
** Copyright 2011-2019 Centreon
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

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <QSqlDriver>
#include <QThread>
#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/storage/exceptions/perfdata.hh"
#include "com/centreon/broker/storage/index_mapping.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/metric_mapping.hh"
#include "com/centreon/broker/storage/parser.hh"
#include "com/centreon/broker/storage/perfdata.hh"
#include "com/centreon/broker/storage/remove_graph.hh"
#include "com/centreon/broker/storage/status.hh"
#include "com/centreon/broker/storage/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::storage;

#define BAM_NAME "_Module_"
#define EPSILON 0.0001

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  Check that the floating point value is a NaN, in which case return a
 *  NULL QVariant.
 *
 *  @param[in] f Floating point value.
 *
 *  @return NULL QVariant if f is a NaN, f casted as QVariant otherwise.
 */
static inline QVariant check_double(double f) {
  return (
    std::isnan(f) || std::isinf(f) ? QVariant(QVariant::Double) : QVariant(f)
  );
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] db_cfg                  Database configuration.
 *  @param[in] rrd_len                 RRD length.
 *  @param[in] interval_length         Length in seconds of a time unit.
 *  @param[in] rebuild_check_interval  How often the stream must check
 *                                     for graph rebuild.
 *  @param[in] store_in_db             Should we insert data in
 *                                     data_bin ?
 *  @param[in] insert_in_index_data    Create entries in index_data or
 *                                     not.
 */
stream::stream(
          database_config const& db_cfg,
          unsigned int rrd_len,
          unsigned int interval_length,
          unsigned int rebuild_check_interval,
          bool store_in_db,
          bool insert_in_index_data)
  : _insert_in_index_data(insert_in_index_data),
    _interval_length(interval_length),
    _pending_events(0),
    _rebuild_thread(
      db_cfg,
      rebuild_check_interval,
      rrd_len,
      interval_length),
    _rrd_len(rrd_len ? rrd_len : 15552000),
    _store_in_db(store_in_db),
    _db(db_cfg),
    _data_bin_insert(_db),
    _update_metrics(_db),
    _index_data_insert(_db),
    _index_data_update(_db) {
  // Prepare queries.
  _prepare();

  // Run rebuild thread.
  _rebuild_thread.start();
}

/**
 *  Destructor.
 */
stream::~stream() {
  // Stop rebuild thread.
  _rebuild_thread.exit();
  _rebuild_thread.wait(-1);
}

/**
 *  Flush the stream.
 *
 *  @return Number of events acknowledged.
 */
int stream::flush() {
  logging::info(logging::medium)
    << "storage: committing transaction";
  _update_status("status=committing current transaction\n");
  _insert_perfdatas();
  _db.commit();
  _db.clear_committed_flag();
  int retval(_pending_events);
  _pending_events = 0;
  _update_status("");
  return (retval);
}

/**
 *  Read from the datbase.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw (broker::exceptions::shutdown()
         << "cannot read from a storage stream");
  return true;
}

/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(io::properties& tree) const {
  std::lock_guard<std::mutex> lock(_statusm);
  if (!_status.empty())
    tree.add_property("status", io::property("status", _status));
}

/**
 *  Rebuild index and metrics cache.
 */
void stream::update() {
  _check_deleted_index();
  _rebuild_cache();
  return ;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(std::shared_ptr<io::data> const& data) {
  // Take this event into account.
  ++_pending_events;
  if (!validate(data, "storage"))
    return 0;

  // Process service status events.
  if (data->type() == neb::service_status::static_type()) {
    std::shared_ptr<neb::service_status>
      ss(std::static_pointer_cast<neb::service_status>(data));
    logging::debug(logging::high)
      << "storage: processing service status event of service "
      << ss->service_id << " of host " << ss->host_id
      << " (ctime " << ss->last_check << ")";

    unsigned int rrd_len;
    bool index_locked(false);
    unsigned int index_id(_find_index_id(
                            ss->host_id,
                            ss->service_id,
                            ss->host_name,
                            ss->service_description,
                            &rrd_len,
                            &index_locked));
    if (index_id != 0) {
      // Generate status event.
      logging::debug(logging::low)
        << "storage: generating status event for (" << ss->host_id
        << ", " << ss->service_id << ") of index " << index_id;
      std::shared_ptr<storage::status> status(new storage::status);
      status->ctime = ss->last_check;
      status->index_id = index_id;
      status->interval
        = static_cast<unsigned int>(ss->check_interval * _interval_length);
      status->is_for_rebuild = false;
      status->rrd_len = rrd_len;
      status->state = ss->last_hard_state;
      multiplexing::publisher().write(status);

      if (!ss->perf_data.empty()) {
        // Parse perfdata.
        std::list<perfdata> pds;
        parser p;
        try {
          p.parse_perfdata(ss->perf_data, pds);
        }
        catch (storage::exceptions::perfdata const& e) { // Discard parsing errors.
          logging::error(logging::medium)
            << "storage: error while parsing perfdata of service ("
            << ss->host_id << ", " << ss->service_id << "): "
            << e.what();
          return 0;
        }

        // Loop through all metrics.
        for (std::list<perfdata>::iterator
               it(pds.begin()),
               end(pds.end());
             it != end;
             ++it) {
          perfdata& pd(*it);

          // Find metric_id.
          unsigned int metric_type(pd.value_type());
          bool metric_locked(false);
          unsigned int metric_id(_find_metric_id(
                                   index_id,
                                   pd.name(),
                                   pd.unit(),
                                   pd.warning(),
                                   pd.warning_low(),
                                   pd.warning_mode(),
                                   pd.critical(),
                                   pd.critical_low(),
                                   pd.critical_mode(),
                                   pd.min(),
                                   pd.max(),
                                   pd.value(),
                                   &metric_type,
                                   &metric_locked));

          if (_store_in_db) {
            // Append perfdata to queue.
            metric_value val;
            val.c_time = ss->last_check;
            val.metric_id = metric_id;
            val.status = ss->current_state;
            val.value = pd.value();
            _perfdata_queue.push_back(val);
          }

          if (!index_locked && !metric_locked) {
            // Send perfdata event to processing.
            std::shared_ptr<storage::metric>
              perf(new storage::metric);
            perf->ctime = ss->last_check;
            perf->interval
              = static_cast<unsigned int>(ss->check_interval * _interval_length);
            perf->is_for_rebuild = false;
            perf->metric_id = metric_id;
            perf->name = pd.name();
            perf->rrd_len = rrd_len;
            perf->value = pd.value();
            perf->value_type = metric_type;
            perf->host_id = ss->host_id;
            perf->service_id = ss->service_id;
            logging::debug(logging::high)
              << "storage: generating perfdata event for metric "
              << perf->metric_id << " (name " << perf->name
              << ", ctime " << perf->ctime << ", value "
              << perf->value << ")";
            multiplexing::publisher().write(perf);
          }
        }
      }
    }
  }

  // Event acknowledgement.
  logging::debug(logging::low)
    << "storage: " << _pending_events << " have not yet been acknowledged";
  if (_db.committed()) {
    _db.clear_committed_flag();
    _insert_perfdatas();
    int retval(_pending_events);
    _pending_events = 0;
    return (retval);
  }
  else
    return (0);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Check for deleted index.
 */
void stream::_check_deleted_index() {
  // Info.
  logging::info(logging::medium) << "storage: starting DB cleanup";
  unsigned long long deleted_index(0);
  unsigned long long deleted_metrics(0);
  _update_status(
    "status=deleting old performance data (might take a while)\n");

  // Database schema version.
  bool db_v2(_db.schema_version() == database::v2);

  // Delete index.
  while (1) {
    // Fetch next index to delete.
    unsigned long long index_id;
    {
      std::ostringstream query;
      query << "SELECT " << (db_v2 ? "id" : "index_id")
            << "  FROM " << (db_v2 ? "index_data" : "rt_index_data")
            << "  WHERE to_delete=1"
               "  LIMIT 1";
      database_query q(_db);
      q.run_query(
          query.str(),
          "storage: could not query index table to get index to delete");
      if (!q.next())
        break ;
      index_id = q.value(0).toULongLong();
    }

    // Get associated metrics.
    std::list<unsigned long long> metrics_to_delete;
    {
      std::ostringstream oss;
      oss << "SELECT metric_id"
             "  FROM " << (db_v2 ? "metrics" : "rt_metrics")
          << "  WHERE index_id=" << index_id;
      database_query q(_db);
      try { q.run_query(oss.str()); }
      catch (std::exception const& e) {
        throw (broker::exceptions::msg()
               << "storage: could not get metrics of index "
               << index_id << ": " << e.what());
      }
      while (q.next())
        metrics_to_delete.push_back(q.value(0).toULongLong());
    }

    // Delete metrics.
    _delete_metrics(metrics_to_delete);
    deleted_metrics += metrics_to_delete.size();

    // Delete index from DB.
    {
      std::ostringstream oss;
      oss << "DELETE FROM " << (db_v2 ? "index_data" : "rt_index_data")
          << "  WHERE " << (db_v2 ? "id" : "index_id")
          << "        =" << index_id;
      database_query q(_db);
      try { q.run_query(oss.str()); }
      catch (std::exception const& e) {
        logging::error(logging::low) << "storage: cannot delete index "
          << index_id << ": " << e.what();
      }
    }
    ++deleted_index;

    // Remove associated graph.
    std::shared_ptr<remove_graph> rg(new remove_graph);
    rg->id = index_id;
    rg->is_index = true;
    multiplexing::publisher().write(rg);
  }

  // Search standalone metrics to delete.
  std::list<unsigned long long> metrics_to_delete;
  {
    std::ostringstream oss;
    oss << "SELECT metric_id"
           "  FROM " << (db_v2 ? "metrics" : "rt_metrics")
        << "  WHERE to_delete=1";
    database_query q(_db);
    q.run_query(
        oss.str(),
        "storage: could not get the list of metrics to delete");
    while (q.next())
      metrics_to_delete.push_back(q.value(0).toULongLong());
  }

  // Delete standalone metrics.
  _delete_metrics(metrics_to_delete);
  deleted_metrics += metrics_to_delete.size();

  // End.
  logging::info(logging::medium) << "storage: end of DB cleanup: "
    << deleted_metrics << " metrics and "
    << deleted_index << " index removed";
  _update_status("");

  return ;
}

/**
 *  Delete specified metrics.
 *
 *  @param[in] metrics_to_delete Metrics to delete.
 */
void stream::_delete_metrics(
               std::list<unsigned long long> const& metrics_to_delete) {
  // Database schema version.
  bool db_v2(_db.schema_version() == database::v2);

  // Delete metrics.
  for (std::list<unsigned long long>::const_iterator
         it(metrics_to_delete.begin()),
         end(metrics_to_delete.end());
       it != end;
       ++it) {
    // Current metric.
    unsigned long long metric_id(*it);

    // Do not delete entries from data_bin as the MyISAM engine used by
    // this table might lock it for a very long time. Orphaned entries
    // will eventually get deleted later.
    // // Delete associated data.
    // {
    //   std::ostringstream oss;
    //   oss << "DELETE FROM data_bin WHERE id_metric=" << metric_id;
    //   QSqlQuery q(*_storage_db);
    //   if (!q.exec(oss.str().c_str()) || q.lastError().isValid())
    //     logging::error(logging::low)
    //       << "storage: cannot remove data of metric " << metric_id
    //       << ": " << q.lastError().text();
    // }

    // Delete from DB.
    {
      std::ostringstream oss;
      oss << "DELETE FROM " << (db_v2 ? "metrics" : "rt_metrics")
          << "  WHERE metric_id=" << metric_id;
      database_query q(_db);
      try { q.run_query(oss.str()); }
      catch (std::exception const& e) {
        logging::error(logging::low)
          << "storage: cannot remove metric " << metric_id << ": "
          << e.what();
      }
    }

    // Remove associated graph.
    std::shared_ptr<remove_graph> rg(new remove_graph);
    rg->id = metric_id;
    rg->is_index = false;
    multiplexing::publisher().write(rg);
  }
}

/**
 *  @brief Find index ID.
 *
 *  Look through the index cache for the specified index. If it cannot
 *  be found, insert an entry in the database.
 *
 *  @param[in]  host_id      Host ID associated to the index.
 *  @param[in]  service_id   Service ID associated to the index.
 *  @param[in]  host_name    Host name associated to the index.
 *  @param[in]  service_desc Service description associated to the index.
 *  @param[out] rrd_len      Index RRD length.
 *  @param[out] locked       Locked flag.
 *
 *  @return Index ID matching host and service ID.
 */
unsigned int stream::_find_index_id(
                       uint64_t host_id,
                       uint64_t service_id,
                       std::string const& host_name,
                       std::string const& service_desc,
                       unsigned int* rrd_len,
                       bool* locked) {
  unsigned int retval;

  // Database schema version.
  bool db_v2(_db.schema_version() == database::v2);

  // Look in the cache.
  std::map<std::pair<uint64_t, uint64_t>, index_info>::iterator it{
      _index_cache.find({host_id, service_id})};

  // Special.
  bool special(!strncmp(
                  host_name.c_str(),
                  BAM_NAME,
                  sizeof(BAM_NAME) - 1) == 0 ? 1 : 0);

  // Found in cache.
  if (it != _index_cache.end()) {
    logging::debug(logging::low) << "storage: found index "
      << it->second.index_id << " of (" << host_id << ", "
      << service_id << ") in cache";
    // Should we update index_data ?
    if ((it->second.host_name != host_name)
        || (it->second.service_description != service_desc)
        || (it->second.special != special)) {
      logging::info(logging::medium) << "storage: updating index "
        << it->second.index_id << " of (" << host_id << ", "
        << service_id << ") (host: " << host_name << ", service: "
        << service_desc << ", special: " << special << ")";
      // Update index_data table.
      if (!_index_data_update.prepared()) {
        _index_data_update.prepare(
          "UPDATE index_data"
          " SET host_name=:host_name,"
          " service_description=:service_description,"
          " special=:special"
          " WHERE host_id=:host_id"
          " AND service_id=:service_id"
        );
      }
      try {
        database_query q(_db);
        q.prepare(query.str());
        q.bind_value(":host_name", QString::fromStdString(host_name));
        q.bind_value(":service_description", QString::fromStdString(service_desc));
        q.bind_value(":special", special);
        q.bind_value(":host_id", (qulonglong)host_id);
        q.bind_value(":service_id", (qulonglong)service_id);
        q.run_statement();
      }
      catch (std::exception const& e) {
        throw (broker::exceptions::msg() << "storage: could not update "
                  "service information in index_data (host_id "
               << host_id << ", service_id " << service_id
               << ", host_name " << host_name
               << ", service_description " << service_desc
               << "): " << e.what());
      }

      // Update cache entry.
      it->second.host_name = host_name;
      it->second.service_description = service_desc;
      it->second.special = special;
    }
    // Anyway, we found index ID.
    retval = it->second.index_id;
    if (rrd_len)
      *rrd_len = it->second.rrd_retention;
    *locked = it->second.locked;
  }
  // Can't find in cache, discard data or insert in DB.
  else {
    logging::info(logging::medium) << "storage: index not found for ("
      << host_id << ", " << service_id << ")";
    // Discard.
    if (!_insert_in_index_data) {
      retval = 0;
      *locked = true;
    }
    // Insert in index_data.
    else {
      logging::info(logging::medium)
        << "storage: creating new index for (" << host_id << ", "
        << service_id << ")";
      // Build query.
      if (!_index_data_insert.prepared()) {
        std::ostringstream oss;
        oss << "INSERT INTO index_data (host_id, host_name, service_id,"
               " service_description, must_be_rebuild, special)"
               " VALUES (:host_id, :host_name, :service_id, :service_description, :must_be_rebuild, :special)";
      _index_data_insert.prepare(oss.str());
      }
      try {
        q.prepare(oss.str());
        q.bind_value(":host_name", QString::fromStdString(host_name));
        q.bind_value(":service_description", QString::fromStdString(service_desc));
        q.bind_value(":special", special);

        // Execute query.
        _index_data_insert.run_statement();
      }
      catch (std::exception const& e) {
        throw (broker::exceptions::msg() << "storage: insertion of "
                  "index (" << host_id << ", " << service_id
               << ") failed: " << e.what());
      }

      // Fetch insert ID with query if possible.
      if (!_db.get_qt_driver()->hasFeature(QSqlDriver::LastInsertId)
          || !(retval = _index_data_insert.last_insert_id().toUInt())) {
        _index_data_insert.finish();
        std::ostringstream oss2;
        oss2 << "SELECT " << (db_v2 ? "id" : "index_id")
             << "  FROM " << (db_v2 ? "index_data" : "rt_index_data")
             << "  WHERE host_id=" << host_id
             << "    AND service_id=" << service_id;
        database_query q(_db);
        try {
          q.run_query(oss2.str());
          if (!q.next())
            throw (broker::exceptions::msg()
                   << "no ID was returned");
        }
        catch (std::exception const& e) {
          throw (broker::exceptions::msg() << "storage: could not "
                    "fetch index_id of newly inserted index ("
                 << host_id << ", " << service_id << "): "
                 << e.what());
        }
        retval = q.value(0).toUInt();
        if (!retval)
          throw (broker::exceptions::msg() << "storage: index_data " \
                    "table is corrupted: got 0 as index_id");
      }

      // Insert index in cache.
      logging::info(logging::medium) << "storage: new index " << retval
        << " for (" << host_id << ", " << service_id << ")";
      index_info info;
      info.host_name = host_name;
      info.index_id = retval;
      info.locked = false;
      info.service_description = service_desc;
      info.special = special;
      info.rrd_retention = _rrd_len;
      _index_cache[std::make_pair(host_id, service_id)] = info;

      // Create the metric mapping.
      std::shared_ptr<index_mapping> im(new index_mapping);
      im->index_id = retval;
      im->host_id = host_id;
      im->service_id = service_id;
      multiplexing::publisher pblshr;
      pblshr.write(im);

      // Provide RRD retention.
      if (rrd_len)
        *rrd_len = info.rrd_retention;
      *locked = info.locked;
    }
  }

  return retval;
}

/**
 *  @brief Find metric ID.
 *
 *  Look through the metric cache for the specified metric. If it cannot
 *  be found, insert an entry in the database.
 *
 *  @param[in]     index_id    Index ID of the metric.
 *  @param[in]     metric_name Name of the metric.
 *  @param[in]     unit_name   Metric unit.
 *  @param[in]     warn        High warning threshold.
 *  @param[in]     warn_low    Low warning threshold.
 *  @param[in]     warn_mode   Warning range mode.
 *  @param[in]     crit        High critical threshold.
 *  @param[in]     crit_low    Low critical threshold.
 *  @param[in]     crit_mode   Critical range mode.
 *  @param[in]     min         Minimal metric value.
 *  @param[in]     max         Maximal metric value.
 *  @param[in]     value       Most recent value.
 *  @param[in,out] type        If not null, set to the metric type.
 *  @param[in,out] locked      Whether or not the metric is locked.
 *
 *  @return Metric ID requested, 0 if it could not be found not
 *          inserted.
 */
uint64_t stream::_find_metric_id(uint64_t index_id,
                                 std::string metric_name,
                                 std::string const& unit_name,
                                 double warn,
                                 double warn_low,
                                 bool warn_mode,
                                 double crit,
                                 double crit_low,
                                 bool crit_mode,
                                 double min,
                                 double max,
                                 double value,
                                 unsigned int* type,
                                 bool* locked) {
  uint64_t retval;

  // Trim metric_name.
  // Is the following really needed, I don't think so.
  //metric_name = metric_name.trimmed();

  // Look in the cache.
  std::map<std::pair<uint64_t, std::string>, metric_info>::iterator
    it(_metric_cache.find(std::make_pair(index_id, metric_name)));
  if (it != _metric_cache.end()) {
    logging::debug(logging::low) << "storage: found metric "
      << it->second.metric_id << " of (" << index_id << ", "
      << metric_name << ") in cache";
    // Should we update metrics ?
    if ((check_double(it->second.value) != check_double(value))
        || (it->second.unit_name != unit_name)
        || (check_double(it->second.warn) != check_double(warn))
        || (check_double(it->second.warn_low) != check_double(warn_low))
        || (it->second.warn_mode != warn_mode)
        || (check_double(it->second.crit) != check_double(crit))
        || (check_double(it->second.crit_low) != check_double(crit_low))
        || (it->second.crit_mode != crit_mode)
        || (check_double(it->second.min) != check_double(min))
        || (check_double(it->second.max) != check_double(max))) {
      logging::info(logging::medium) << "storage: updating metric "
        << it->second.metric_id << " of (" << index_id << ", "
        << metric_name << ") (unit: " << unit_name << ", warning: "
        << warn_low << ":" << warn << ", critical: " << crit_low << ":"
        << crit << ", min: " << min << ", max: " << max << ")";
      // Update metrics table.
      _update_metrics.bind_value(":unit_name", QString::fromStdString(unit_name));
      _update_metrics.bind_value(":warn", check_double(warn));
      _update_metrics.bind_value(":warn_low", check_double(warn_low));
      _update_metrics.bind_value(":warn_threshold_mode", warn_mode);
      _update_metrics.bind_value(":crit", check_double(crit));
      _update_metrics.bind_value(":crit_low", check_double(crit_low));
      _update_metrics.bind_value(":crit_threshold_mode", crit_mode);
      _update_metrics.bind_value(":min", check_double(min));
      _update_metrics.bind_value(":max", check_double(max));
      _update_metrics.bind_value(":current_value", check_double(value));
      _update_metrics.bind_value(":index_id", (qulonglong)index_id);
      _update_metrics.bind_value(":metric_name", QString::fromStdString(metric_name));
      try { _update_metrics.run_statement(); }
      catch (std::exception const& e) {
        throw (broker::exceptions::msg() << "storage: could not "
                  "update metric (index_id " << index_id
               << ", metric " << metric_name << "): " << e.what());
      }

      // Fill cache.
      it->second.value = value;
      it->second.unit_name = unit_name;
      it->second.warn = warn;
      it->second.warn_low = warn_low;
      it->second.warn_mode = warn_mode;
      it->second.crit = crit;
      it->second.crit_low = crit_low;
      it->second.crit_mode = crit_mode;
      it->second.min = min;
      it->second.max = max;
    }

    // Remember, we found the metric ID.
    retval = it->second.metric_id;
    if (it->second.type != perfdata::automatic)
      *type = it->second.type;
    *locked = it->second.locked;
  }

  // Can't find in cache, insert in DB.
  else {
    logging::debug(logging::low)
      << "storage: creating new metric for (" << index_id
      << ", " << metric_name << ")";

    // Database schema version.
    bool db_v2(_db.schema_version() == database::v2);

    // Build query.
    if (*type == perfdata::automatic)
      *type = perfdata::gauge;
    std::ostringstream query;
    query << "INSERT INTO " << (db_v2 ? "metrics" : "rt_metrics")
          << "  (index_id, metric_name, unit_name, warn, warn_low,"
             "   warn_threshold_mode, crit, crit_low, "
             "   crit_threshold_mode, min, max, current_value,"
             "   data_source_type)"
             " VALUES (:index_id, :metric_name, :unit_name, :warn, "
             "         :warn_low, :warn_threshold_mode, :crit, "
             "         :crit_low, :crit_threshold_mode, :min, :max, "
             "         :current_value, :data_source_type)";
    database_query q(_db);
    q.prepare(
        query.str(),
        "storage: could not prepare metric insertion query");
    q.bind_value(":index_id", (qulonglong)index_id);
    q.bind_value(":metric_name", QString::fromStdString(metric_name));
    q.bind_value(":unit_name", QString::fromStdString(unit_name));
    q.bind_value(":warn", check_double(warn));
    q.bind_value(":warn_low", check_double(warn_low));
    q.bind_value(":warn_threshold_mode", warn_mode);
    q.bind_value(":crit", check_double(crit));
    q.bind_value(":crit_low", check_double(crit_low));
    q.bind_value(":crit_threshold_mode", crit_mode);
    q.bind_value(":min", check_double(min));
    q.bind_value(":max", check_double(max));
    q.bind_value(":current_value", check_double(value));
    q.bind_value(":data_source_type", *type + (db_v2 ? 1 : 0));

    // Execute query.
    try { q.run_statement(); }
    catch (std::exception const& e) {
      throw (broker::exceptions::msg() << "storage: insertion of "
                "metric '" << metric_name << "' of index " << index_id
             << " failed: " << e.what());
    }

    // Fetch insert ID with query if possible.
    if (!_db.get_qt_driver()->hasFeature(QSqlDriver::LastInsertId)
        || !(retval = q.last_insert_id().toUInt())) {
      q.finish();
      std::ostringstream query;
      query << "SELECT metric_id"
               "  FROM " << (db_v2 ? "metrics" : "rt_metrics")
            << "  WHERE index_id=:index_id"
               "    AND metric_name=:metric_name";
      database_query q2(_db);
      q2.prepare(
           query.str(),
           "storage: could not prepare metric ID fetching query");
      q2.bind_value(":index_id", (qulonglong)index_id);
      q2.bind_value(":metric_name", QString::fromStdString(metric_name));
      try {
        q2.run_statement();
        if (!q2.next())
          throw (broker::exceptions::msg() << "no ID was returned");
      }
      catch (std::exception const& e) {
        throw (broker::exceptions::msg() << "storage: could not fetch"
                  " metric_id of newly inserted metric '"
               << metric_name << "' of index " << index_id << ": "
               << e.what());
      }
      retval = q2.value(0).toUInt();
      if (!retval)
        throw (broker::exceptions::msg() << "storage: metrics table "
                 "is corrupted: got 0 as metric_id");
    }

    // Insert metric in cache.
    logging::info(logging::medium) << "storage: new metric "
      << retval << " for (" << index_id << ", " << metric_name << ")";
    metric_info info;
    info.locked = false;
    info.metric_id = retval;
    info.type = *type;
    info.value = value;
    info.unit_name = unit_name;
    info.warn = warn;
    info.warn_low = warn_low;
    info.warn_mode = warn_mode;
    info.crit = crit;
    info.crit_low = crit_low;
    info.crit_mode = crit_mode;
    info.min = min;
    info.max = max;
    _metric_cache[std::make_pair(index_id, metric_name)] = info;

    // Create the metric mapping.
    std::shared_ptr<metric_mapping> mm(new metric_mapping);
    mm->index_id = index_id;
    mm->metric_id = info.metric_id;
    multiplexing::publisher pblshr;
    pblshr.write(mm);

    *locked = info.locked;
  }

  return retval;
}

/**
 *  Insert performance data entries in the data_bin table.
 */
void stream::_insert_perfdatas() {
  if (!_perfdata_queue.empty()) {
    // Status.
    _update_status("status=inserting performance data\n");

    // Database schema version.
    bool db_v2(_db.schema_version() == database::v2);

    // Insert first entry.
    std::ostringstream query;
    {
      metric_value& mv(_perfdata_queue.front());
      query.precision(10);
      query << std::scientific
            << "INSERT INTO " << (db_v2 ? "data_bin" : "log_data_bin")
            << "  (" << (db_v2 ? "id_metric" : "metric_id")
            << "   , ctime, status, value)"
               "  VALUES (" << mv.metric_id << ", " << mv.c_time << ", '"
            << mv.status << "', '";
      if (std::isinf(mv.value))
        query << ((mv.value < 0.0) ? -FLT_MAX : FLT_MAX);
      else if (std::isnan(mv.value))
        query << "NULL";
      else
        query << mv.value;
      query << "')";
      _perfdata_queue.pop_front();
    }

    // Insert perfdata in data_bin.
    while (!_perfdata_queue.empty()) {
      metric_value& mv(_perfdata_queue.front());
      query << ", (" << mv.metric_id << ", " << mv.c_time << ", '"
            << mv.status << "', ";
      if (std::isinf(mv.value))
        query << ((mv.value < 0.0) ? -FLT_MAX : FLT_MAX);
      else if (std::isnan(mv.value))
        query << "NULL";
      else
        query << mv.value;
      query << ")";
      _perfdata_queue.pop_front();
    }

    // Execute query.
    database_query q(_db);
    q.run_query(
        query.str(),
        "storage: could not insert data in log_data_bin");
    _update_status("");
  }

  return ;
}

/**
 *  Prepare queries.
 */
void stream::_prepare() {
  // Build cache.
  _rebuild_cache();

  // Database schema version.
  bool db_v2(_db.schema_version() == database::v2);

  // Prepare metrics update query.
  std::ostringstream query;
  query << "UPDATE " << (db_v2 ? "metrics" : "rt_metrics")
        << " SET unit_name=:unit_name,"
           "     warn=:warn,"
           "     warn_low=:warn_low,"
           "     warn_threshold_mode=:warn_threshold_mode,"
           "     crit=:crit,"
           "     crit_low=:crit_low,"
           "     crit_threshold_mode=:crit_threshold_mode,"
           "     min=:min,"
           "     max=:max,"
           "     current_value=:current_value"
           "  WHERE index_id=:index_id"
           "    AND metric_name=:metric_name";
  _update_metrics.prepare(
    query.str(),
    "storage: could not prepare metrics update query");

  return ;
}

/**
 *  Rebuild cache.
 */
void stream::_rebuild_cache() {
  // Status.
  _update_status("status=rebuilding index and metrics cache\n");

  // Create multiplexing publisher for metric and status mappings.
  multiplexing::publisher pblshr;

  // Delete old cache.
  _index_cache.clear();
  _metric_cache.clear();

  // Database schema version.
  bool db_v2(_db.schema_version() == database::v2);

  // Fill index cache.
  {
    // Execute query.
    std::ostringstream query;
    query << "SELECT " << (db_v2 ? "id" : "index_id")
          << "       , host_id, service_id, host_name,"
             "       rrd_retention, service_description, special,"
             "       locked"
             " FROM " << (db_v2 ? "index_data" : "rt_index_data");
    database_query q(_db);
    q.run_query(
        query.str(),
        "storage: could not fetch index list from data DB");

    // Loop through result set.
    while (q.next()) {
      index_info info;
      info.index_id = q.value(0).toUInt();
      unsigned int host_id(q.value(1).toUInt());
      unsigned int service_id(q.value(2).toUInt());
      info.host_name = q.value(3).toString().toStdString();
      info.rrd_retention = (q.value(4).isNull() ? 0 : q.value(4).toUInt());
      if (!info.rrd_retention)
        info.rrd_retention = _rrd_len;
      info.service_description = q.value(5).toString().toStdString();
      if (db_v2)
        info.special = (q.value(6).toUInt() == 2);
      else
        info.special = q.value(6).toBool();
      info.locked = q.value(7).toBool();
      logging::debug(logging::high) << "storage: loaded index "
        << info.index_id << " of (" << host_id << ", "
        << service_id << ")";
      _index_cache[std::make_pair(host_id, service_id)] = info;

      // Create the metric mapping.
      std::shared_ptr<index_mapping> im(new index_mapping);
      im->index_id = info.index_id;
      im->host_id = host_id;
      im->service_id = service_id;
      pblshr.write(im);
    }
  }

  // Fill metric cache.
  {
    // Execute query.
    std::ostringstream query;
    query << "SELECT metric_id, index_id, metric_name,"
             "       data_source_type,"
             "       locked, current_value, unit_name, warn, warn_low,"
             "       warn_threshold_mode, crit, crit_low,"
             "       crit_threshold_mode, min, max"
             "  FROM " << (db_v2 ? "metrics" : "rt_metrics");
    database_query q(_db);
    q.run_query(
        query.str(),
        "storage: could not fetch metric list from data DB");

    // Loop through result set.
    while (q.next()) {
      metric_info info;
      info.metric_id = q.value(0).toUInt();
      uint64_t index_id(q.value(1).toUInt());
      std::string name(q.value(2).toString().toStdString());
      info.type = (q.value(3).isNull()
                   ? static_cast<unsigned int>(perfdata::automatic)
                   : q.value(3).toUInt());
      info.locked = q.value(4).toBool();
      info.value = (q.value(5).isNull() ? NAN : q.value(5).toDouble());
      info.unit_name = q.value(6).toString().toStdString();
      info.warn = (q.value(7).isNull() ? NAN : q.value(7).toDouble());
      info.warn_low = (q.value(8).isNull() ? NAN : q.value(8).toDouble());
      info.warn_mode = q.value(9).toBool();
      info.crit = (q.value(10).isNull() ? NAN : q.value(10).toDouble());
      info.crit_low = (q.value(11).isNull() ? NAN : q.value(11).toDouble());
      info.crit_mode = q.value(12).toBool();
      info.min = (q.value(13).isNull() ? NAN : q.value(13).toDouble());
      info.max = (q.value(14).isNull() ? NAN : q.value(14).toDouble());
      logging::debug(logging::high) << "storage: loaded metric "
        << info.metric_id << " of (" << index_id << ", " << name
        << "), type " << info.type;
      _metric_cache[{index_id, name}] = info;

      // Create the metric mapping.
      std::shared_ptr<metric_mapping> mm(new metric_mapping);
      mm->index_id = index_id;
      mm->metric_id = info.metric_id;
      pblshr.write(mm);
    }
  }

  // Status.
  _update_status("");

  return ;
}

/**
 *  Update status of endpoint.
 *
 *  @param[in] status New status.
 */
void stream::_update_status(std::string const& status) {
  std::lock_guard<std::mutex> lock(_statusm);
  _status = status;
}
