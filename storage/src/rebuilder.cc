/*
** Copyright 2012-2015,2017 Centreon
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
#include <ctime>
#include <QVariant>
#include <sstream>
#include "com/centreon/broker/database.hh"
#include "com/centreon/broker/database_query.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/rebuild.hh"
#include "com/centreon/broker/storage/status.hh"
#include "com/centreon/broker/storage/rebuilder.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] db_cfg                  Database configuration.
 *  @param[in] rebuild_check_interval  How often the rebuild thread will
 *                                     check for rebuild.
 *  @param[in] rrd_length              Length of RRD files.
 *  @param[in] interval_length         Length in seconds of a time unit.
 */
rebuilder::rebuilder(
             database_config const& db_cfg,
             unsigned int rebuild_check_interval,
             unsigned int rrd_length,
             unsigned int interval_length)
  : _db_cfg(db_cfg),
    _interval_length(interval_length),
    _rebuild_check_interval(rebuild_check_interval),
    _rrd_len(rrd_length),
    _should_exit(false) {
  _db_cfg.set_queries_per_transaction(1);
}

/**
 *  Destructor.
 */
rebuilder::~rebuilder() throw () {}

/**
 *  Set the exit flag.
 */
void rebuilder::exit() throw () {
  _should_exit = true;
  return ;
}

/**
 *  Get the rebuild check interval.
 *
 *  @return Rebuild check interval in seconds.
 */
unsigned int rebuilder::get_rebuild_check_interval() const throw () {
  return (_rebuild_check_interval);
}

/**
 *  Get the RRD length in seconds.
 *
 *  @return RRD length in seconds.
 */
unsigned int rebuilder::get_rrd_length() const throw () {
  return (_rrd_len);
}

/**
 *  Thread entry point.
 */
void rebuilder::run() {
  while (!_should_exit && _rebuild_check_interval) {
    try {
      // Open DB.
      std::unique_ptr<database> db;
      try {
        db.reset(new database(_db_cfg));
      }
      catch (std::exception const& e) {
        throw (broker::exceptions::msg() << "storage: rebuilder: could "
               "not connect to Centreon Storage database: "
               << e.what());
      }

      // Database schema version.
      bool db_v2(db->schema_version() == database::v2);

      // Fetch index to rebuild.
      index_info info;
      _next_index_to_rebuild(info, *db);
      while (!_should_exit && info.index_id) {
        // Get check interval of host/service.
        unsigned int index_id;
        unsigned int host_id;
        unsigned int service_id;
        unsigned int check_interval(0);
        unsigned int rrd_len;
        {
          index_id = info.index_id;
          host_id = info.host_id;
          service_id = info.service_id;
          rrd_len = info.rrd_retention;

          std::ostringstream oss;
          if (!info.service_id)
            oss << "SELECT check_interval"
                << " FROM " << (db_v2 ? "hosts" : "rt_hosts")
                << " WHERE host_id=" << info.host_id;
          else
            oss << "SELECT check_interval"
                << " FROM " << (db_v2 ? "services" : "rt_services")
                << " WHERE host_id=" << info.host_id
                << "  AND service_id=" << info.service_id;
          database_query query(*db);
          query.run_query(oss.str());
          if (query.next())
            check_interval = (query.value(0).isNull()
                              ? 0.0
                              : query.value(0).toDouble()) * _interval_length;
          if (!check_interval)
            check_interval = 5 * 60;
        }
        logging::info(logging::medium) << "storage: rebuilder: index "
          << index_id << " (interval " << check_interval
          << ") will be rebuild";

        // Set index as being rebuilt.
        _set_index_rebuild(*db, index_id, 2);

        try {
          // Fetch metrics to rebuild.
          std::list<metric_info> metrics_to_rebuild;
          {
            std::ostringstream oss;
            oss << "SELECT metric_id, metric_name, data_source_type"
                << " FROM " << (db_v2 ? "metrics" : "rt_metrics")
                << " WHERE index_id=" << index_id;
            database_query metrics_to_rebuild_query(*db);
            try { metrics_to_rebuild_query.run_query(oss.str()); }
            catch (std::exception const& e) {
              throw (exceptions::msg()
                     << "storage: rebuilder: could not fetch "
                     << "metrics of index " << index_id);
            }
            while (!_should_exit && metrics_to_rebuild_query.next()) {
              metric_info info;
              info.metric_id
                = metrics_to_rebuild_query.value(0).toUInt();
              info.metric_name
                = metrics_to_rebuild_query.value(1).toString().toStdString();
              info.metric_type
                = metrics_to_rebuild_query.value(2).toInt();
              metrics_to_rebuild.push_back(info);
            }
          }

          // Browse metrics to rebuild.
          while (!_should_exit && !metrics_to_rebuild.empty()) {
            metric_info info(metrics_to_rebuild.front());
            metrics_to_rebuild.pop_front();
            _rebuild_metric(
              *db,
              info.metric_id,
              host_id,
              service_id,
              info.metric_name,
              info.metric_type,
              check_interval,
              rrd_len);
          }

          // Rebuild status.
          _rebuild_status(
            *db,
            index_id,
            check_interval);
        }
        catch (...) {
          // Set index as to-be-rebuilt.
          _set_index_rebuild(*db, index_id, 1);

          // Rethrow exception.
          throw ;
        }

        // Set index as rebuilt or to-be-rebuild
        // if we were interrupted.
        _set_index_rebuild(*db, index_id, (_should_exit ? 1 : 0));

        // Get next index to rebuild.
        _next_index_to_rebuild(info, *db);
      }
    }
    catch (std::exception const& e) {
      logging::error(logging::high) << e.what();
    }
    catch (...) {
      logging::error(logging::high)
        << "storage: rebuilder: unknown error";
    }

    // Sleep a while.
    time_t target(time(NULL) + _rebuild_check_interval);
    while (!_should_exit && (target > time(NULL)))
      sleep(1);
  }
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Get next index to rebuild.
 *
 *  @param[out] info  Information about the next index to rebuild.
 *                    Zero'd if no index is waiting for rebuild.
 *  @param[in]  db    Database object.
 */
void rebuilder::_next_index_to_rebuild(index_info& info, database& db) {
  bool db_v2(db.schema_version() == database::v2);
  std::ostringstream query;
  query << "SELECT " << (db_v2 ? "id" : "index_id")
        << "       , host_id, service_id, rrd_retention"
           "  FROM " << (db_v2 ? "index_data" : "rt_index_data")
        << "  WHERE must_be_rebuild=" << (db_v2 ? "'1'" : "1")
        << "  LIMIT 1";
  database_query index_to_rebuild_query(db);
  index_to_rebuild_query.run_query(
    query.str(),
    "storage: rebuilder: could not fetch index to rebuild");
  if (index_to_rebuild_query.next()) {
    info.index_id = index_to_rebuild_query.value(0).toUInt();
    info.host_id = index_to_rebuild_query.value(1).toUInt();
    info.service_id = index_to_rebuild_query.value(2).toUInt();
    info.rrd_retention
      = (index_to_rebuild_query.value(3).isNull()
         ? 0
         : index_to_rebuild_query.value(3).toUInt());
    if (!info.rrd_retention)
      info.rrd_retention = _rrd_len;
  }
  else
    memset(&info, 0, sizeof(info));
}

/**
 *  Rebuild a metric.
 *
 *  @param[in] db           Database object.
 *  @param[in] metric_id    Metric ID.
 *  @param[in] host_id      Id of the host this metric belong to.
 *  @param[in] service_id   Id of the service this metric belong to.
 *  @param[in] metric_name  Metric name.
 *  @param[in] type         Metric type.
 *  @param[in] interval     Host/service check interval.
 *  @param[in] length       Metric RRD length in seconds.
 */
void rebuilder::_rebuild_metric(
                  database& db,
                  unsigned int metric_id,
                  unsigned int host_id,
                  unsigned int service_id,
                  std::string const& metric_name,
                  short metric_type,
                  unsigned int interval,
                  unsigned int length) {
  // Log.
  logging::info(logging::low)
    << "storage: rebuilder: rebuilding metric " << metric_id
    << " (name " << metric_name << ", type " << metric_type
    << ", interval " << interval << ")";

  // Send rebuild start event.
  _send_rebuild_event(false, metric_id, false);

  // Database schema version.
  bool db_v2(db.schema_version() == database::v2);

  try {
    // Get data.
    std::ostringstream oss;
    oss << "SELECT ctime, value"
        << " FROM " << (db_v2 ? "data_bin" : "log_data_bin")
        << " WHERE " << (db_v2 ? "id_metric" : "metric_id")
        << "=" << metric_id
        << " ORDER BY ctime ASC";
    database_query data_bin_query(db);
    bool caught(false);
    try { data_bin_query.run_query(oss.str()); }
    catch (std::exception const& e) {
      caught = true;
      logging::error(logging::medium) << "storage: rebuilder: "
        << "cannot fetch data of metric " << metric_id << ": "
        << e.what();
    }
    if (!caught)
      while (!_should_exit && data_bin_query.next()) {
        std::shared_ptr<storage::metric> entry(new storage::metric);
        entry->ctime = data_bin_query.value(0).toUInt();
        entry->interval = interval;
        entry->is_for_rebuild = true;
        entry->metric_id = metric_id;
        entry->name = metric_name;
        entry->rrd_len = length;
        entry->value_type = metric_type;
        entry->value = data_bin_query.value(1).toDouble();
        entry->host_id = host_id;
        entry->service_id = service_id;
        if (entry->value > FLT_MAX * 0.999)
          entry->value = INFINITY;
        else if (entry->value < -FLT_MAX * 0.999)
          entry->value = -INFINITY;
        multiplexing::publisher().write(entry);
      }
  }
  catch (...) {
    // Send rebuild end event.
    _send_rebuild_event(true, metric_id, false);

    // Rethrow exception.
    throw ;
  }

  // Send rebuild end event.
  _send_rebuild_event(true, metric_id, false);
}

/**
 *  Rebuild a status.
 *
 *  @param[in] db        Database object.
 *  @param[in] index_id  Index ID.
 *  @param[in] interval  Host/service check interval.
 */
void rebuilder::_rebuild_status(
                  database& db,
                  unsigned int index_id,
                  unsigned int interval) {
  // Log.
  logging::info(logging::low)
    << "storage: rebuilder: rebuilding status " << index_id
    << " (interval " << interval << ")";

  // Send rebuild start event.
  _send_rebuild_event(false, index_id, true);

  // Database schema version.
  bool db_v2(db.schema_version() == database::v2);

  try {
    // Get data.
    std::ostringstream oss;
    oss << "SELECT d.ctime, d.status"
        << " FROM " << (db_v2 ? "metrics" : "rt_metrics") << " AS m"
        << " JOIN " << (db_v2 ? "data_bin" : "log_data_bin") << " AS d"
        << "   ON m.metric_id=d." << (db_v2 ? "id_metric" : "metric_id")
        << " WHERE m.index_id=" << index_id
        << " ORDER BY d.ctime ASC";
    database_query data_bin_query(db);
    bool caught(false);
    try { data_bin_query.run_query(oss.str()); }
    catch (std::exception const& e) {
      caught = true;
      logging::error(logging::medium) << "storage: rebuilder: "
        << "cannot fetch data of index " << index_id << ": "
        << e.what();
    }
    if (!caught)
      while (!_should_exit && data_bin_query.next()) {
        std::shared_ptr<storage::status> entry(new storage::status);
        entry->ctime = data_bin_query.value(0).toUInt();
        entry->index_id = index_id;
        entry->interval = interval;
        entry->is_for_rebuild = true;
        entry->rrd_len = _rrd_len;
        entry->state = data_bin_query.value(1).toInt();
        multiplexing::publisher().write(entry);
      }
  }
  catch (...) {
    // Send rebuild end event.
    _send_rebuild_event(true, index_id, true);

    // Rethrow exception.
    throw ;
  }

  // Send rebuild end event.
  _send_rebuild_event(true, index_id, true);
}

/**
 *  Send a rebuild event.
 *
 *  @param[in] end      false if rebuild is starting, true if it is ending.
 *  @param[in] id       Index or metric ID.
 *  @param[in] is_index true for an index ID, false for a metric ID.
 */
void rebuilder::_send_rebuild_event(
                  bool end,
                  unsigned int id,
                  bool is_index) {
  std::shared_ptr<storage::rebuild> rb(new storage::rebuild);
  rb->end = end;
  rb->id = id;
  rb->is_index = is_index;
  multiplexing::publisher().write(rb);
  return ;
}

/**
 *  Set index rebuild flag.
 *
 *  @param[in] db        Database object.
 *  @param[in] index_id  Index to update.
 *  @param[in] state     Rebuild state (0, 1 or 2).
 */
void rebuilder::_set_index_rebuild(
                  database& db,
                  unsigned int index_id,
                  short state) {
  bool db_v2(db.schema_version() == database::v2);
  std::ostringstream oss;
  oss << "UPDATE " << (db_v2 ? "index_data" : "rt_index_data")
      << " SET must_be_rebuild="
      << (db_v2 ? "'" : "") << state << (db_v2 ? "'" : "")
      << " WHERE " << (db_v2 ? "id" : "index_id") << "=" << index_id;
  database_query update_index_query(db);
  try { update_index_query.run_query(oss.str()); }
  catch (std::exception const& e) {
    logging::error(logging::low)
      << "storage: rebuilder: cannot update state of index "
      << index_id << ": " << e.what();
  }
}
