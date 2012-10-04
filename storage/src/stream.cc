/*
** Copyright 2011-2012 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QThread>
#include <QVariant>
#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/storage/exceptions/perfdata.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/parser.hh"
#include "com/centreon/broker/storage/perfdata.hh"
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
  return (isnan(f) ? QVariant(QVariant::Double) : QVariant(f));
}

/**
 *  Check that two double are equal.
 *
 *  @param[in] d1 First double.
 *  @param[in] d2 Second double.
 *
 *  @return true if d1 and d2 are equal.
 */
static inline bool double_equal(double d1, double d2) {
  return (fabs(d1 - d2) < EPSILON);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] storage_type            Storage DB type.
 *  @param[in] storage_host            Storage DB host.
 *  @param[in] storage_port            Storage DB port.
 *  @param[in] storage_user            Storage DB user.
 *  @param[in] storage_password        Storage DB password.
 *  @param[in] storage_db              Storage DB name.
 *  @param[in] queries_per_transaction Queries per transaction.
 *  @param[in] rrd_len                 RRD length.
 *  @param[in] interval_length         Interval length.
 *  @param[in] rebuild_check_interval  How often the stream must check
 *                                     for graph rebuild.
 *  @param[in] store_in_db             Should we insert data in
 *                                     data_bin ?
 *  @param[in] check_replication       true to check replication status.
 */
stream::stream(
          QString const& storage_type,
          QString const& storage_host,
          unsigned short storage_port,
          QString const& storage_user,
          QString const& storage_password,
          QString const& storage_db,
          unsigned int queries_per_transaction,
          unsigned int rrd_len,
          time_t interval_length,
          unsigned int rebuild_check_interval,
          bool store_in_db,
          bool check_replication) {
  // Process events.
  _process_out = true;

  // Queries per transaction.
  _queries_per_transaction = ((queries_per_transaction >= 2)
                              ? queries_per_transaction
                              : 1);
  _transaction_queries = 0;

  // Store in DB.
  _store_in_db = store_in_db;

  // Storage connection ID.
  QString storage_id;
  storage_id.setNum((qulonglong)this, 16);

  // Add database connection.
  _storage_db.reset(new QSqlDatabase(QSqlDatabase::addDatabase(storage_type, storage_id)));
  if (storage_type == "QMYSQL")
    _storage_db->setConnectOptions("CLIENT_FOUND_ROWS");

  // Set database parameters.
  _storage_db->setHostName(storage_host);
  _storage_db->setPort(storage_port);
  _storage_db->setUserName(storage_user);
  _storage_db->setPassword(storage_password);
  _storage_db->setDatabaseName(storage_db);

  try {
    {
      QMutexLocker lock(&global_lock);
      // Open database.
      if (!_storage_db->open()) {
        _clear_qsql();
        throw (broker::exceptions::msg() << "storage: could not connect " \
               "to Centreon Storage database");
      }
    }

    // Check that replication is OK.
    if (check_replication) {
      logging::debug(logging::medium)
        << "storage: checking replication status";
      QSqlQuery q(*_storage_db);
      if (!q.exec("SHOW SLAVE STATUS"))
        logging::info(logging::medium)
          << "storage: could not check replication status";
      else {
        if (!q.next())
          logging::info(logging::medium)
            << "storage: database is not under replication";
        else {
          QSqlRecord record(q.record());
          unsigned int i(0);
          for (QString field = record.fieldName(i);
               !field.isEmpty();
               field = record.fieldName(++i))
            if (((field == "Slave_IO_Running")
                 && (q.value(i).toString() != "Yes"))
                || ((field == "Slave_SQL_Running")
                    && (q.value(i).toString() != "Yes"))
                || ((field == "Seconds_Behind_Master")
                    && (q.value(i).toInt() != 0)))
              throw (broker::exceptions::msg() << "storage: " \
                          "replication is not complete: " << field
                       << "=" << q.value(i).toString());
          logging::info(logging::medium)
            << "storage: database replication is complete, " \
               "connection granted";
        }
      }
    }
    else
      logging::debug(logging::medium)
        << "storage: NOT checking replication status";

    // Prepare queries.
    _prepare();

    // Initial transaction.
    if (_queries_per_transaction > 1)
      _storage_db->transaction();

    // Run rebuild thread.
    _rebuild_thread.set_interval(rebuild_check_interval);
    _rebuild_thread.set_interval_length(interval_length);
    _rebuild_thread.set_rrd_length(rrd_len);
    _rebuild_thread.set_db(*_storage_db);
    _rebuild_thread.run();

    // Register with multiplexer.
    multiplexing::engine::instance().hook(*this, false);
  }
  catch (...) {
    {
      QMutexLocker lock(&global_lock);
      // Delete statements.
      _clear_qsql();
    }

    // Remove this connection.
    QSqlDatabase::removeDatabase(storage_id);
    throw ;
  }

  // Set parameters.
  _rrd_len = rrd_len;
  _interval_length = interval_length;
}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s) : multiplexing::hooker(s) {
  // Processing.
  _process_out = s._process_out;

  // Queries per transaction.
  _queries_per_transaction = s._queries_per_transaction;
  _transaction_queries = 0;

  // Store in DB.
  _store_in_db = s._store_in_db;

  // Storage connection ID.
  QString storage_id;
  storage_id.setNum((qulonglong)this, 16);

  // Clone Storage database.
  _storage_db.reset(new QSqlDatabase(QSqlDatabase::cloneDatabase(
                                                     *s._storage_db,
                                                     storage_id)));

  // Copy rebuild thread.
  _rebuild_thread = s._rebuild_thread;

  try {
    {
      QMutexLocker lock(&global_lock);
      // Open database.
      if (!_storage_db->open()) {
        _clear_qsql();
        throw (broker::exceptions::msg() << "storage: could not connect " \
               "to Centreon Storage database");
      }
    }

    // Prepare queries.
    _prepare();

    // Initial transaction.
    if (_queries_per_transaction > 1)
      _storage_db->transaction();

    // Run rebuild thread.
    _rebuild_thread.run();

    // Register with multiplexer.
    multiplexing::engine::instance().hook(*this, false);
  }
  catch (...) {
    {
      QMutexLocker lock(&global_lock);
      // Delete statements.
      _clear_qsql();
    }

    // Remove this connection.
    QSqlDatabase::removeDatabase(storage_id);
    throw ;
  }

  // Set parameters.
  _rrd_len = s._rrd_len;
  _interval_length = s._interval_length;
}

/**
 *  Destructor.
 */
stream::~stream() {
  // Stop rebuild thread.
  _rebuild_thread.exit();
  _rebuild_thread.wait(-1);

  // Unregister from multiplexer.
  multiplexing::engine::instance().unhook(*this);

  // Connection ID.
  QString storage_id;
  storage_id.setNum((qulonglong)this, 16);

  {
    QMutexLocker lock(&global_lock);
    // Reset statements.
    _clear_qsql();
  }

  // Remove this connection.
  QSqlDatabase::removeDatabase(storage_id);
}

/**
 *  Enable or disable output event processing.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Set to true to enable output event processing.
 */
void stream::process(bool in, bool out) {
  _process_out = in || !out; // Only for immediate shutdown.
  return ;
}

/**
 *  Read from the datbase.
 *
 *  @param[out] d Cleared.
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  throw (com::centreon::broker::exceptions::msg()
         << "storage: attempt to read from a storage stream (not supported yet)");
  return ;
}

/**
 *  Multiplexing starts.
 */
void stream::starting() {
  return ;
}

/**
 *  Multiplexing stopped.
 */
void stream::stopping() {
  _process_out = false;
  return ;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 */
void stream::write(misc::shared_ptr<io::data> const& data) {
  // Check that processing is enabled.
  if (!_process_out)
    throw (io::exceptions::shutdown(true, true)
             << "storage stream is shutdown");

  // Process service status events.
  if (!data.isNull()) {
    if (data->type() == "com::centreon::broker::neb::service_status") {
      logging::debug(logging::high)
        << "storage: processing service status event";
      misc::shared_ptr<neb::service_status>
        ss(data.staticCast<neb::service_status>());

      // Increase event count.
      ++_transaction_queries;

      if (!ss->perf_data.isEmpty()) {
        // Find index_id.
        unsigned int index_id(_find_index_id(
                                ss->host_id,
                                ss->service_id,
                                ss->host_name,
                                ss->service_description));

        // Generate status event.
        logging::debug(logging::low)
          << "storage: generating status event";
        misc::shared_ptr<storage::status> status(new storage::status);
        status->ctime = ss->last_check;
        status->index_id = index_id;
        status->interval = static_cast<time_t>(
                             ss->check_interval * _interval_length);
        status->rrd_len = _rrd_len;
        status->state = ss->last_hard_state;
        multiplexing::publisher().write(status.staticCast<io::data>());

        // Parse perfdata.
        QList<perfdata> pds;
        parser p;
        try {
          p.parse_perfdata(ss->perf_data, pds);
        }
        catch (storage::exceptions::perfdata const& e) { // Discard parsing errors.
          logging::error(logging::medium)
            << "storage: error while parsing perfdata of service ("
            << ss->host_id << ", " << ss->service_id << "): "
            << e.what();
          return ;
        }

        // Loop through all metrics.
        for (QList<perfdata>::iterator it(pds.begin()), end(pds.end());
             it != end;
             ++it) {
          perfdata& pd(*it);

          // Find metric_id.
          unsigned int metric_type(perfdata::automatic);
          unsigned int metric_id(_find_metric_id(
                                   index_id,
                                   pd.name(),
                                   pd.unit(),
                                   pd.warning(),
                                   pd.critical(),
                                   pd.min(),
                                   pd.max(),
                                   &metric_type));

          if (_store_in_db) {
            // Insert perfdata in data_bin.
            logging::debug(logging::low)
              << "storage: inserting perfdata in data_bin (metric: "
              << metric_id << ", ctime: " << ss->last_check
              << ", value: " << pd.value() << ", status: "
              << ss->current_state << ")";
            _insert_data_bin->bindValue(":id_metric", metric_id);
            _insert_data_bin->bindValue(
                                ":ctime",
                                static_cast<unsigned int>(ss->last_check));
            _insert_data_bin->bindValue(":value", pd.value());
            _insert_data_bin->bindValue(
                                ":status",
                                ss->current_state + 1);
            if (!_insert_data_bin->exec()
                || _insert_data_bin->lastError().isValid())
              throw (broker::exceptions::msg() << "storage: could not " \
                        "insert data in data_bin (metric " << metric_id
                     << ", ctime "
                     << static_cast<unsigned long long>(ss->last_check)
                     << "): " << _insert_data_bin->lastError().text());
          }

          // Send perfdata event to processing.
          logging::debug(logging::high)
            << "storage: generating perfdata event";
          misc::shared_ptr<storage::metric> perf(new storage::metric);
          perf->ctime = ss->last_check;
          perf->interval = static_cast<time_t>(ss->check_interval
                                               * _interval_length);
          perf->metric_id = metric_id;
          perf->name = pd.name();
          perf->rrd_len = _rrd_len;
          perf->value = pd.value();
          perf->value_type = ((metric_type == perfdata::automatic)
                              ? static_cast<unsigned int>(
                                  pd.value_type())
                              : metric_type);
          multiplexing::publisher().write(perf.staticCast<io::data>());
        }
      }
    }
  }

  // Commit transaction.
  if (_queries_per_transaction > 1) {
    logging::debug(logging::low) << "storage: current transaction has "
      << _transaction_queries << " pending queries";
    if (_storage_db->isOpen()
        && ((_transaction_queries >= _queries_per_transaction)
            || data.isNull())) {
      logging::info(logging::medium)
        << "storage: committing transaction";
      _storage_db->commit();
      _storage_db->transaction();
      _transaction_queries = 0;
    }
  }

  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Assignment operator.
 *
 *  Should not be used. Any call to this method will result in a call to
 *  abort().
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& s) {
  (void)s;
  assert(!"storage stream is not copyable");
  abort();
  return (*this);
}

/**
 *  Clear QtSql objects.
 */
void stream::_clear_qsql() {
  _insert_data_bin.reset();
  _update_metrics.reset();
  if (_storage_db.get() && _storage_db->isOpen()) {
    if (_queries_per_transaction > 1)
      _storage_db->commit();
    _storage_db->close();
  }
  _storage_db.reset();
  return ;
}

/**
 *  @brief Find index ID.
 *
 *  Look through the index cache for the specified index. If it cannot
 *  be found, insert an entry in the database.
 *
 *  @param[in] host_id      Host ID associated to the index.
 *  @param[in] service_id   Service ID associated to the index.
 *  @param[in] host_name    Host name associated to the index.
 *  @param[in] service_desc Service description associated to the index.
 *
 *  @return Index ID matching host and service ID.
 */
unsigned int stream::_find_index_id(
                       unsigned int host_id,
                       unsigned int service_id,
                       QString const& host_name,
                       QString const& service_desc) {
  unsigned int retval;

  // Look in the cache.
  std::map<
         std::pair<unsigned int, unsigned int>,
         index_info>::iterator
    it(_index_cache.find(std::make_pair(host_id, service_id)));

  // Special.
  bool special(!strncmp(
                  host_name.toStdString().c_str(),
                  BAM_NAME,
                  sizeof(BAM_NAME) - 1));

  // Found in cache.
  if (it != _index_cache.end()) {
    logging::debug(logging::low) << "storage: found index "
      << it->second.index_id << "of (" << host_id << ", "
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
      std::ostringstream oss;
      oss << "UPDATE index_data" \
             " SET host_name=:host_name," \
             "     service_description=:service_description," \
             "     special=:special" \
             " WHERE host_id=" << host_id
          << " AND service_id=" << service_id;
      QSqlQuery q(*_storage_db);
      q.prepare(oss.str().c_str());
      q.bindValue(":host_name", host_name);
      q.bindValue(":service_description", service_desc);
      q.bindValue(":special", (special ? 2 : 1));
      if (!q.exec() || q.lastError().isValid())
        throw (broker::exceptions::msg() << "storage: could not update "
                  "service information in index_data (host_id "
               << host_id << ", service_id " << service_id
               << ", host_name " << host_name
               << ", service_description " << service_desc
               << "): " << q.lastError().text());

      // Update cache entry.
      it->second.host_name = host_name;
      it->second.service_description = service_desc;
      it->second.special = special;
    }
    // Anyway, we found index ID.
    retval = it->second.index_id;
  }
  // Can't find in cache, insert in DB.
  else {
    logging::debug(logging::high) << "storage: creating new index for ("
      << host_id << ", " << service_id << ")";
    // Build query.
    std::ostringstream oss;
    oss << "INSERT INTO index_data (" \
           "  host_id, host_name," \
           "  service_id, service_description, " \
           "  must_be_rebuild, special)" \
           " VALUES (" << host_id << ", :host_name, " << service_id
        << ", :service_description, 1, :special)";
    QSqlQuery q(*_storage_db);
    q.prepare(oss.str().c_str());
    q.bindValue(":host_name", host_name);
    q.bindValue(":service_description", service_desc);
    q.bindValue(":special", (special ? 2 : 1));

    // Execute query.
    if (!q.exec() || q.lastError().isValid())
      throw (broker::exceptions::msg() << "storage: insertion of " \
                  "index (" << host_id << ", " << service_id
               << ") failed: " << q.lastError().text());

    // Fetch insert ID with query if possible.
    if (_storage_db->driver()->hasFeature(QSqlDriver::LastInsertId)
        || !(retval = q.lastInsertId().toUInt())) {
#if QT_VERSION >= 0x040302
      q.finish();
#endif // Qt >= 4.3.2
      std::ostringstream oss2;
      oss2 << "SELECT id" \
              " FROM index_data" \
              " WHERE host_id=" << host_id
           << " AND service_id=" << service_id;
      QSqlQuery q2(oss2.str().c_str(), *_storage_db);
      if (!q2.exec() || q2.lastError().isValid() || !q2.next())
        throw (broker::exceptions::msg() << "storage: could not fetch" \
                    " index_id of newly inserted index (" << host_id
                 << ", " << service_id << "): "
                 << q2.lastError().text());
      retval = q2.value(0).toUInt();
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
    info.service_description = service_desc;
    info.special = special;
    _index_cache[std::make_pair(host_id, service_id)] = info;
  }

  return (retval);
}

/**
 *  @brief Find metric ID.
 *
 *  Look through the metric cache for the specified metric. If it cannot
 *  be found, insert an entry in the database.
 *
 *  @param[in]  index_id    Index ID of the metric.
 *  @param[in]  metric_name Name of the metric.
 *  @param[in]  unit_name   Metric unit.
 *  @param[in]  warn        Warning threshold.
 *  @param[in]  crit        Critical threshold.
 *  @param[in]  min         Minimal metric value.
 *  @param[in]  max         Maximal metric value.
 *  @param[out] type        If not null, set to the metric type.
 *
 *  @return Metric ID requested, 0 if it could not be found not
 *          inserted.
 */
unsigned int stream::_find_metric_id(
                       unsigned int index_id,
                       QString metric_name,
                       QString const& unit_name,
                       double warn,
                       double crit,
                       double min,
                       double max,
                       unsigned int* type) {
  unsigned int retval;

  // Trim metric_name.
  metric_name = metric_name.trimmed();

  // Look in the cache.
  std::map<std::pair<unsigned int, QString>, metric_info>::iterator
    it(_metric_cache.find(std::make_pair(index_id, metric_name)));
  if (it != _metric_cache.end()) {
    logging::debug(logging::low) << "storage: found metric "
      << it->second.metric_id << " of (" << index_id << ", "
      << metric_name << ") in cache";
    // Should we update metrics ?
    if ((unit_name != it->second.unit_name)
        || !double_equal(crit, it->second.crit)
        || !double_equal(max, it->second.max)
        || !double_equal(min, it->second.min)
        || !double_equal(warn, it->second.warn)) {
      logging::info(logging::medium) << "storage: updating metric "
        << it->second.metric_id << " of (" << index_id << ", "
        << metric_name << ") (unit: " << unit_name << ", warning: "
        << warn << ", critical: " << crit << ", min: " << min
        << ", max: " << max << ")";
      // Update metrics table.
      _update_metrics->bindValue(":unit_name", unit_name);
      _update_metrics->bindValue(":warn", check_double(warn));
      _update_metrics->bindValue(":crit", check_double(crit));
      _update_metrics->bindValue(":min", check_double(min));
      _update_metrics->bindValue(":max", check_double(max));
      _update_metrics->bindValue(":index_id", index_id);
      _update_metrics->bindValue(":metric_name", metric_name);
      if (!_update_metrics->exec()
          || _update_metrics->lastError().isValid())
        throw (broker::exceptions::msg() << "storage: could not " \
                    "update metric (index_id " << index_id
                 << ", metric " << metric_name << "): "
                 << _update_metrics->lastError().text());

      // Update cache entry.
      it->second.crit = crit;
      it->second.max = max;
      it->second.min = min;
      it->second.unit_name = unit_name;
      it->second.warn = warn;
    }

    // Anyway, we found the metric ID.
    retval = it->second.metric_id;
    if (type)
      *type = it->second.type;
  }

  // Can't find in cache, insert in DB.
  else {
    logging::debug(logging::low)
      << "storage: creating new metric for (" << index_id
      << ", " << metric_name << ")";
    // Build query.
    std::ostringstream oss;
    std::string escaped_metric_name;
    {
      QSqlField field("metric_name", QVariant::String);
      field.setValue(metric_name.toStdString().c_str());
      escaped_metric_name
        = _storage_db->driver()->formatValue(field, true).toStdString();
    }
    std::string escaped_unit_name;
    {
      QSqlField field("unit_name", QVariant::String);
      field.setValue(unit_name.toStdString().c_str());
      escaped_unit_name
        = _storage_db->driver()->formatValue(field, true).toStdString();
    }
    oss << "INSERT INTO metrics (index_id, metric_name, unit_name, warn, crit, min, max)" \
      " VALUES (" << index_id << ", " << escaped_metric_name << ", "
        << escaped_unit_name << ", " << std::fixed << warn << ", "
        << crit << ", " << min << ", " << max << ")";

    // Execute query.
    QSqlQuery q(*_storage_db);
    if (!q.exec(oss.str().c_str()) || q.lastError().isValid())
      throw (broker::exceptions::msg() << "storage: insertion of " \
                  "metric '" << metric_name << "' of index " << index_id
               << " failed: " << q.lastError().text());

    // Fetch insert ID with query if possible.
    if (!_storage_db->driver()->hasFeature(QSqlDriver::LastInsertId)
        || !(retval = q.lastInsertId().toUInt())) {
#if QT_VERSION >= 0x040302
      q.finish();
#endif // Qt >= 4.3.2
      std::ostringstream oss2;
      oss2 << "SELECT metric_id" \
              " FROM metrics" \
              " WHERE index_id=" << index_id
           << " AND metric_name=" << escaped_metric_name;
      QSqlQuery q2(oss2.str().c_str(), *_storage_db);
      if (!q2.exec() || q2.lastError().isValid() || !q2.next())
        throw (broker::exceptions::msg() << "storage: could not fetch" \
                    " metric_id of newly inserted metric '"
                 << metric_name << "' of index " << index_id << ": "
                 << q2.lastError().text());
      retval = q2.value(0).toUInt();
      if (!retval)
        throw (broker::exceptions::msg() << "storage: metrics table " \
                 "is corrupted: got 0 as metric_id");
    }

    // Insert metric in cache.
    logging::info(logging::medium) << "storage: new metric "
      << retval << " for (" << index_id << ", " << metric_name << ")";
    metric_info info;
    info.crit = crit;
    info.max = max;
    info.metric_id = retval;
    info.min = min;
    info.type = perfdata::automatic;
    info.unit_name = unit_name;
    _metric_cache[std::make_pair(index_id, metric_name)] = info;

    // Fetch metric type.
    if (type)
      *type = info.type;
  }

  return (retval);
}

/**
 *  Prepare queries.
 */
void stream::_prepare() {
  // Fill index cache.
  {
    // Execute query.
    QSqlQuery q("SELECT id, host_id, service_id, host_name, service_description, special" \
                " FROM index_data",
                *_storage_db);
    if (!q.exec() || q.lastError().isValid())
      throw (broker::exceptions::msg() << "storage: could not fetch " \
                  "index list from data DB: "
               << q.lastError().text());

    // Loop through result set.
    while (q.next()) {
      index_info info;
      info.index_id = q.value(0).toUInt();
      unsigned int host_id(q.value(1).toUInt());
      unsigned int service_id(q.value(2).toUInt());
      info.host_name = q.value(3).toString();
      info.service_description = q.value(4).toString();
      info.special = (q.value(5).toUInt() == 2);
      logging::debug(logging::high) << "storage: loaded index "
        << info.index_id << " of (" << host_id << ", "
        << service_id << ")";
      _index_cache[std::make_pair(host_id, service_id)] = info;
    }
  }

  // Fill metric cache.
  {
    // Execute query.
    QSqlQuery q("SELECT metric_id, index_id, metric_name, data_source_type, unit_name, warn, crit, min, max" \
                " FROM metrics",
                *_storage_db);
    if (!q.exec() || q.lastError().isValid())
      throw (broker::exceptions::msg() << "storage: could not fetch " \
                  "metric list from data DB: "
               << q.lastError().text());

    // Loop through result set.
    while (q.next()) {
      metric_info info;
      info.metric_id = q.value(0).toUInt();
      unsigned int index_id(q.value(1).toUInt());
      QString name(q.value(2).toString());
      info.type = (q.value(3).isNull()
                   ? static_cast<unsigned int>(perfdata::automatic)
                   : q.value(3).toUInt());
      info.unit_name = q.value(4).toString();
      info.warn = q.value(5).toDouble();
      info.crit = q.value(6).toDouble();
      info.min = q.value(7).toDouble();
      info.max = q.value(8).toDouble();
      logging::debug(logging::high) << "storage: loaded metric "
        << info.metric_id << " of (" << index_id << ", " << name
        << "), type " << info.type;
      _metric_cache[std::make_pair(index_id, name)] = info;
    }
  }

  // Prepare metrics update query.
  _update_metrics.reset(new QSqlQuery(*_storage_db));
  if (!_update_metrics->prepare("UPDATE metrics" \
                                " SET unit_name=:unit_name," \
                                " warn=:warn," \
                                " crit=:crit," \
                                " min=:min," \
                                " max=:max" \
                                " WHERE index_id=:index_id" \
                                " AND metric_name=:metric_name"))
    throw (broker::exceptions::msg() << "storage: could not prepare " \
                "metrics update query: "
             << _update_metrics->lastError().text());

  // Prepare data_bind insert query.
  _insert_data_bin.reset(new QSqlQuery(*_storage_db));
  if (!_insert_data_bin->prepare("INSERT INTO data_bin (" \
                                 " id_metric, ctime, value, status)" \
                                 " VALUES (:id_metric," \
                                 " :ctime," \
                                 " :value," \
                                 " :status)"))
    throw (broker::exceptions::msg() << "storage: could not prepare " \
                "data_bin insert query: "
             << _insert_data_bin->lastError().text());

  return ;
}
