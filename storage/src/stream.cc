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
#include <sstream>
#include <QMutexLocker>
#include <QVariant>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/mysql_manager.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/instance.hh"
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
    _ack_events(0),
    _pending_events(0),
    _rebuilder(
      db_cfg,
      rebuild_check_interval,
      rrd_len,
      interval_length),
    _rrd_len(rrd_len ? rrd_len : 15552000),
    _store_in_db(store_in_db),
    _mysql(db_cfg) {
  // Prepare queries.
  _prepare();
}

/**
 *  Destructor.
 */
stream::~stream() {
  mysql_manager::instance().clear();
  logging::debug(logging::low)
    << "storage: stream closed.";
}

/**
 *  Flush the stream.
 *
 *  @return Number of events acknowledged.
 */
int stream::flush() {
  logging::info(logging::low)
    << "storage: committing transaction";
  _update_status("status=committing current transaction\n");
  _insert_perfdatas();
  _mysql.commit();

  int retval(_ack_events + _pending_events);
  _ack_events = 0;
  _pending_events = 0;
  logging::debug(logging::medium)
    << "storage: flush ack events count: " << retval;
  _update_status("");
  return retval;
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
  QMutexLocker lock(&_statusm);
  if (!_status.empty())
    tree.add_property("status", io::property("status", _status));
}

/**
 *  Rebuild index and metrics cache.
 */
void stream::update() {
  _check_deleted_index();
  _rebuild_cache();
  _host_instance_cache_create();
}

/**
 * Create the cache to link host id to instance id.
 */
void stream::_host_instance_cache_create() {
  _cache_host_instance.clear();
  std::ostringstream oss;

  std::promise<database::mysql_result> promise;
  _mysql.run_query_and_get_result("SELECT host_id, instance_id FROM hosts",
           &promise,
           "SQL: could not get the list of host/instance pairs");
  database::mysql_result res(promise.get_future().get());
  while (_mysql.fetch_row(res))
    _cache_host_instance[res.value_as_u32(0)] = res.value_as_u32(1);
}

void stream::_process_instance(
               std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::instance const& i(*static_cast<neb::instance const*>(e.get()));

  // Log message.
  logging::info(logging::medium) << "SQL: processing poller event "
    << "(id: " << i.poller_id << ", name: " << i.name << ", running: "
    << (i.is_running ? "yes" : "no") << ")";

  if (i.is_running) {
    for (std::map<unsigned int, unsigned int>::iterator
           it(_cache_host_instance.begin()),
           end(_cache_host_instance.end());
         it != end ; ) {
      if (it->second == i.poller_id)
        _cache_host_instance.erase(it++);
      else
        ++it;
    }
  }
}

/**
 *  Process an host event.
 *
 *  @param[in] e Uncasted host.
 */
void stream::_process_host(
               std::shared_ptr<io::data> const& e) {
  // Cast object.
  neb::host const& h(*static_cast<neb::host const*>(e.get()));

  // Log message.
  logging::info(logging::medium) << "storage: processing host event"
       " (poller: " << h.poller_id << ", id: "
    << h.host_id << ", name: " << h.host_name << ")";

  // Processing
  if (h.host_id) {
    if (h.enabled)
      _cache_host_instance[h.host_id] = h.poller_id;
    else
      _cache_host_instance.erase(h.host_id);
  }
  else
    logging::error(logging::high) << "storage: host '" << h.host_name
      << "' of poller " << h.poller_id << " has no ID";
}
/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(std::shared_ptr<io::data> const& data) {
  ++_pending_events;
  logging::info(logging::low)
    << "storage: write pending_events = " << _pending_events;
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
  else if (data->type() == neb::instance::static_type())
    _process_instance(data);
  else if (data->type() == neb::host::static_type())
    _process_host(data);

  // Event acknowledgement.
  logging::debug(logging::low)
    << "storage: " << _pending_events << " events have not yet been acknowledged";

  int retval(_ack_events);
  _ack_events = 0;
  logging::debug(logging::low)
    << "storage: ack events count: " << retval;
  return retval;
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
  bool db_v2(_mysql.schema_version() == mysql::v2);

  std::ostringstream oss;
  oss << "SELECT " << (db_v2 ? "id" : "index_id")
        << "  FROM " << (db_v2 ? "index_data" : "rt_index_data")
        << "  WHERE to_delete=1"
           "  LIMIT 1";
  std::string query(oss.str());

  // Delete index.
  while (1) {
    // Fetch next index to delete.
    unsigned long long index_id;
    {
      std::promise<database::mysql_result> promise;
      _mysql.run_query_and_get_result(
               query, &promise,
               "storage: could not query index table to get index to delete: ");
      database::mysql_result res(promise.get_future().get());
      if (!_mysql.fetch_row(res))
        break ;
      index_id = res.value_as_u64(0);
    }

    // Get associated metrics.
    std::list<unsigned long long> metrics_to_delete;
    {
      std::ostringstream oss;
      oss << "SELECT metric_id"
             "  FROM " << (db_v2 ? "metrics" : "rt_metrics")
          << "  WHERE index_id=" << index_id;

      std::ostringstream oss_err;
      oss_err << "storage: could not get metrics at index "
              << index_id << ": ";

      std::promise<database::mysql_result> promise;
      _mysql.run_query_and_get_result(oss.str(), &promise, oss_err.str());
      database::mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res))
        metrics_to_delete.push_back(res.value_as_u64(0));
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
      std::ostringstream oss_error;
      oss_error << "storage: cannot delete index " << index_id << ": ";
      _mysql.run_query(oss.str(), oss_error.str());
      if (_mysql.commit_if_needed())
        _set_ack_events();
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
    std::promise<database::mysql_result> promise;
    _mysql.run_query_and_get_result(
             oss.str(),
             &promise,
             "storage: could not get the list of metrics to delete");
    database::mysql_result res(promise.get_future().get());
    while (_mysql.fetch_row(res))
      metrics_to_delete.push_back(res.value_as_u64(0));
  }

  // Delete standalone metrics.
  _delete_metrics(metrics_to_delete);
  deleted_metrics += metrics_to_delete.size();

  // End.
  logging::info(logging::medium) << "storage: end of DB cleanup: "
    << deleted_metrics << " metrics and "
    << deleted_index << " index removed";
  _update_status("");
}

/**
 *  Delete specified metrics.
 *
 *  @param[in] metrics_to_delete Metrics to delete.
 */
void stream::_delete_metrics(
               std::list<unsigned long long> const& metrics_to_delete) {
  // Database schema version.
  bool db_v2(_mysql.schema_version() == mysql::v2);

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
      std::ostringstream oss_error;
      oss_error << "storage: cannot remove metric " << metric_id << ": ";
      _mysql.run_query(oss.str(), oss_error.str());
      if (_mysql.commit_if_needed())
        _set_ack_events();
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
  bool db_v2(_mysql.schema_version() == mysql::v2);

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
    // Should we update index_data?
    if (it->second.host_name != host_name
        || it->second.service_description != service_desc
        || it->second.special != special) {
      logging::info(logging::medium) << "storage: updating index "
        << it->second.index_id << " of (" << host_id << ", "
        << service_id << ") (host: " << host_name << ", service: "
        << service_desc << ", special: " << special << ")";

      // Update index_data table.
      _update_index_data_stmt.bind_value_as_str(0, host_name);
      _update_index_data_stmt.bind_value_as_str(1, service_desc);
      _update_index_data_stmt.bind_value_as_str(2, special ? "1" : "0");
      _update_index_data_stmt.bind_value_as_i32(3, host_id);
      _update_index_data_stmt.bind_value_as_i32(4, service_id);

      _mysql.run_statement(
               _update_index_data_stmt,
               "UPDATE index_data",
               true,
               _cache_host_instance[host_id] % _mysql.connections_count());
      if (_mysql.commit_if_needed())
        _set_ack_events();

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
      std::ostringstream oss;
      oss << "INSERT INTO " << (db_v2 ? "index_data" : "rt_index_data")
          << "  (host_id, host_name, service_id, service_description,"
             "   must_be_rebuild, special)"
             "  VALUES (" << host_id << ", '" << host_name << "', " << service_id
          << ", '" << service_desc << "', " << (db_v2 ? "'0'" : "0")
          << ", '" << special << "')";

      std::stringstream err_oss;
      err_oss << "storage: insertion of "
                 "index (" << host_id << ", " << service_id
              << ") failed: ";

      std::promise<int> promise;
      _mysql.run_query_and_get_int(
                      oss.str(),
                      &promise,
                      database::mysql_task::LAST_INSERT_ID,
                      err_oss.str());
      // Let's get the index id
      retval = promise.get_future().get();
      if (retval == 0) {
        throw broker::exceptions::msg() << "storage: could not "
                  "fetch index_id of newly inserted index ("
               << host_id << ", " << service_id << ")";
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
      << mtc_name << ") in cache";
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
        << mtc_name << ") (unit: " << unit_name << ", warning: "
        << warn_low << ":" << warn << ", critical: " << crit_low << ":"
        << crit << ", min: " << min << ", max: " << max << ")";
      // Update metrics table.
      _update_metrics_stmt.bind_value_as_str(0, unit_name);
      _update_metrics_stmt.bind_value_as_f32(1, warn);
      _update_metrics_stmt.bind_value_as_f32(2, warn_low);
      _update_metrics_stmt.bind_value_as_tiny(3, warn_mode);
      _update_metrics_stmt.bind_value_as_f32(4, crit);
      _update_metrics_stmt.bind_value_as_f32(5, crit_low);
      _update_metrics_stmt.bind_value_as_tiny(6, crit_mode);
      _update_metrics_stmt.bind_value_as_f32(7, min);
      _update_metrics_stmt.bind_value_as_f32(8, max);
      _update_metrics_stmt.bind_value_as_f32(9, value);
      _update_metrics_stmt.bind_value_as_i32(10, it->second.metric_id);

      // Only use the thread_id 0
      _mysql.run_statement(_update_metrics_stmt, "UPDATE metrics", true);
      if (_mysql.commit_if_needed())
        _set_ack_events();

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
      << ", " << mtc_name << ")";

    // Database schema version.
    bool db_v2(_mysql.schema_version() == mysql::v2);

    // Build query.
    if (*type == perfdata::automatic)
      *type = perfdata::gauge;

    _insert_metrics_stmt.bind_value_as_i32(0, index_id);
    _insert_metrics_stmt.bind_value_as_str(1, mtc_name);
    _insert_metrics_stmt.bind_value_as_str(2, unit_name);
    _insert_metrics_stmt.bind_value_as_f32(3, warn);
    _insert_metrics_stmt.bind_value_as_f32(4, warn_low);
    _insert_metrics_stmt.bind_value_as_tiny(5, warn_mode);
    _insert_metrics_stmt.bind_value_as_f32(6, crit);
    _insert_metrics_stmt.bind_value_as_f32(7, crit_low);
    _insert_metrics_stmt.bind_value_as_tiny(8, crit_mode);
    _insert_metrics_stmt.bind_value_as_f32(9, min);
    _insert_metrics_stmt.bind_value_as_f32(10, max);
    _insert_metrics_stmt.bind_value_as_f32(11, value);
    char t[2];
    t[0] = '0' + *type + (db_v2 ? 1 : 0);
    t[1] = 0;
    _insert_metrics_stmt.bind_value_as_str(12, t);

    // Execute query.
    std::ostringstream oss;
    oss << "storage: insertion of "
           "metric '" << mtc_name.toStdString() << "' of index " << index_id
        << " failed: ";

    std::promise<int> promise;
    _mysql.run_statement_and_get_int(
             _insert_metrics_stmt,
             &promise,
             database::mysql_task::LAST_INSERT_ID,
             oss.str());
    retval = promise.get_future().get();

    // Insert metric in cache.
    logging::info(logging::medium) << "storage: new metric "
      << retval << " for (" << index_id << ", " << mtc_name << ")";
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
    _metric_cache[std::make_pair(index_id, mtc_name)] = info;

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
    bool db_v2(_mysql.schema_version() == mysql::v2);

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
      query << ")";
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
    _mysql.run_query(
             query.str(),
             "storage: could not insert data in data_bin");
    if (_mysql.commit_if_needed())
      _set_ack_events();

    _update_status("");
  }
}

/**
 *  Prepare queries.
 */
void stream::_prepare() {
  // Database schema version.
  bool db_v2(_mysql.schema_version() == mysql::v2);

  // Prepare metrics update query.
  std::ostringstream query;
  query << "UPDATE " << (db_v2 ? "metrics" : "rt_metrics")
        << " SET unit_name=?,"
           "     warn=?,"
           "     warn_low=?,"
           "     warn_threshold_mode=?,"
           "     crit=?,"
           "     crit_low=?,"
           "     crit_threshold_mode=?,"
           "     min=?,"
           "     max=?,"
           "     current_value=?"
           "  WHERE metric_id=?";
  _update_metrics_stmt = _mysql.prepare_query(query.str());

  query.str("");
  query << "INSERT INTO " << (db_v2 ? "metrics" : "rt_metrics")
        << "  (index_id, metric_name, unit_name, warn, warn_low,"
           "   warn_threshold_mode, crit, crit_low, "
           "   crit_threshold_mode, min, max, current_value,"
           "   data_source_type)"
           " VALUES (?, ?, ?, ?, "
           "         ?, ?, ?, "
           "         ?, ?, ?, ?, "
           "         ?, ?)";
  _insert_metrics_stmt = _mysql.prepare_query(query.str());

  query.str("");
  query << "UPDATE " << (db_v2 ? "index_data" : "rt_index_data")
        << "  SET host_name=?,"
           "    service_description=?,"
           "    special=?"
           "  WHERE host_id=?"
           "    AND service_id=?";
  _update_index_data_stmt = _mysql.prepare_query(query.str());

  // Build cache.
  _rebuild_cache();
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
  bool db_v2(_mysql.schema_version() == mysql::v2);

  // Fill index cache.
  {
    // Execute query.
    std::ostringstream query;
    query << "SELECT " << (db_v2 ? "id" : "index_id")
          << "       , host_id, service_id, host_name,"
             "       rrd_retention, service_description, special,"
             "       locked"
             " FROM " << (db_v2 ? "index_data" : "rt_index_data");
    std::promise<database::mysql_result> promise;
    _mysql.run_query_and_get_result(
             query.str(),
             &promise,
             "storage: could not fetch index list from data DB");
    database::mysql_result res(promise.get_future().get());

    // Loop through result set.
    while (_mysql.fetch_row(res)) {
      index_info info;

      info.index_id = res.value_as_u32(0);
      unsigned int host_id(res.value_as_u32(1));
      unsigned int service_id(res.value_as_u32(2));
      info.host_name = QString(res.value_as_str(3).c_str());
      info.rrd_retention = res.value_as_u32(4);
      if (!info.rrd_retention)
        info.rrd_retention = _rrd_len;
      info.service_description = QString(res.value_as_str(5).c_str());
      if (db_v2)
        info.special = (res.value_as_u32(6) == 2);
      else
        info.special = res.value_as_bool(6);
      info.locked = res.value_as_bool(7);
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
    std::promise<database::mysql_result> promise;
    _mysql.run_query_and_get_result(
             query.str(),
             &promise,
             "storage: could not fetch metric list from data DB");
    database::mysql_result res(promise.get_future().get());

    // Loop through result set.
    while (_mysql.fetch_row(res)) {
      metric_info info;

      info.metric_id = res.value_as_u32(0);
      uint64_t index_id(res.value_as_u32(1));
      std::string name(res.value_as_str(2).c_str());
      info.type = (res.value_is_null(3)
                   ? static_cast<unsigned int>(perfdata::automatic)
                   : res.value_as_u32(3));
      info.locked = res.value_as_bool(4);
      info.value = (res.value_is_null(5) ? NAN : res.value_as_f64(5));
      info.unit_name = res.value_as_str(6).c_str();
      info.warn = (res.value_is_null(7) ? NAN : res.value_as_f64(7));
      info.warn_low = (res.value_is_null(8) ? NAN : res.value_as_f64(8));
      info.warn_mode = res.value_as_bool(9);
      info.crit = (res.value_is_null(10) ? NAN : res.value_as_f64(10));
      info.crit_low = (res.value_is_null(11) ? NAN : res.value_as_f64(11));
      info.crit_mode = res.value_as_bool(12);
      info.min = (res.value_is_null(13) ? NAN : res.value_as_f64(13));
      info.max = (res.value_is_null(14) ? NAN : res.value_as_f64(14));
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
  QMutexLocker lock(&_statusm);
  _status = status;
  return ;
}

void stream::_set_ack_events() {
  _ack_events += _pending_events;
  _pending_events = 0;
}
