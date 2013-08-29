/*
** Copyright 2011-2013 Merethis
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
  return (isnan(f) ? QVariant(QVariant::Double) : QVariant(f));
}

/**
 *  Check that two double are equal.
 *
 *  @param[in] a First value.
 *  @param[in] b Second value.
 *
 *  @return true if a and b are equal.
 */
static inline bool double_equal(double a, double b) {
  return ((isnan(a) && isnan(b))
          || (isinf(a)
              && isinf(b)
              && (std::signbit(a) == std::signbit(b)))
          || (std::isfinite(a)
              && std::isfinite(b)
              && !(fabs((a) - (b)) > (0.01 * fabs(a)))));
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
 *  @param[in] insert_in_index_data    Create entries in index_data or
 *                                     not.
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
          bool check_replication,
          bool insert_in_index_data) {
  // Process events.
  _process_out = true;

  // Queries per transaction.
  _queries_per_transaction = ((queries_per_transaction >= 2)
                              ? queries_per_transaction
                              : 1);
  _transaction_queries = 0;

  // Store in DB.
  _store_in_db = store_in_db;
  _insert_in_index_data = insert_in_index_data;

  // Storage connection ID.
  QString storage_id;
  storage_id.setNum((qulonglong)this, 16);

  // Add database connection.
  _storage_db.reset(
    new QSqlDatabase(QSqlDatabase::addDatabase(
                                     storage_type,
                                     storage_id)));
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

    // Set parameters.
    _rrd_len = (rrd_len ? rrd_len : 15552000);
    _interval_length = interval_length;

    // Prepare queries.
    _prepare();

    // Initial transaction.
    if (_queries_per_transaction > 1)
      _storage_db->transaction();

    // Run rebuild thread.
    _rebuild_thread.set_interval(rebuild_check_interval);
    _rebuild_thread.set_interval_length(interval_length);
    _rebuild_thread.set_rrd_length(_rrd_len);
    _rebuild_thread.set_db(*_storage_db);
    _rebuild_thread.start();

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
  _insert_in_index_data = s._insert_in_index_data;

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

    // Set parameters.
    _rrd_len = s._rrd_len;
    _interval_length = s._interval_length;

    // Prepare queries.
    _prepare();

    // Initial transaction.
    if (_queries_per_transaction > 1)
      _storage_db->transaction();

    // Run rebuild thread.
    _rebuild_thread.start();

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
 *  Get endpoint statistics.
 *
 *  @param[out] buffer Output buffer.
 */
void stream::statistics(std::string& buffer) const {
  QMutexLocker lock(&_statusm);
  buffer.append(_status);
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
unsigned int stream::write(misc::shared_ptr<io::data> const& data) {
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

      unsigned int rrd_len;
      unsigned int index_id(_find_index_id(
                              ss->host_id,
                              ss->service_id,
                              ss->host_name,
                              ss->service_description,
                              &rrd_len));
      if (index_id != 0) {
        // Generate status event.
        logging::debug(logging::low)
          << "storage: generating status event for (" << ss->host_id
          << ", " << ss->service_id << ") of index " << index_id;
        misc::shared_ptr<storage::status> status(new storage::status);
        status->ctime = ss->last_check;
        status->index_id = index_id;
        status->interval = static_cast<time_t>(
                             ss->check_interval * _interval_length);
        status->is_for_rebuild = false;
        status->rrd_len = rrd_len;
        status->state = ss->last_hard_state;
        multiplexing::publisher().write(status.staticCast<io::data>());

        if (!ss->perf_data.isEmpty()) {
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
            return (1);
          }

          // Loop through all metrics.
          for (QList<perfdata>::iterator
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
                                     &metric_type,
                                     &metric_locked));

            if (_store_in_db) {
              // Append perfdata to queue.
              metric_value val;
              val.c_time = ss->last_check;
              val.metric_id = metric_id;
              val.status = ss->current_state + 1;
              val.value = pd.value();
              _perfdata_queue.push_back(val);
            }

            if (!metric_locked) {
              // Send perfdata event to processing.
              logging::debug(logging::high)
                << "storage: generating perfdata event";
              misc::shared_ptr<storage::metric>
                perf(new storage::metric);
              perf->ctime = ss->last_check;
              perf->interval = static_cast<time_t>(
                                 ss->check_interval * _interval_length);
              perf->is_for_rebuild = false;
              perf->metric_id = metric_id;
              perf->name = pd.name();
              perf->rrd_len = rrd_len;
              perf->value = pd.value();
              perf->value_type = metric_type;
              multiplexing::publisher().write(perf.staticCast<io::data>());
            }
          }
        }
      }
    }
  }

  // Commit transactions.
  if (_queries_per_transaction > 1) {
    logging::debug(logging::low) << "storage: current transaction has "
      << _transaction_queries << " pending queries";
    if (_storage_db->isOpen()
        && ((_transaction_queries >= _queries_per_transaction)
            || data.isNull())) {
      logging::info(logging::medium)
        << "storage: committing transaction";
      _update_status("status=committing current transaction\n");
      _storage_db->commit();
      _storage_db->transaction();
      _transaction_queries = 0;
      _update_status("");
    }
  }
  unsigned int perfdata_events(_perfdata_queue.size());
  logging::debug(logging::low) << "storage: " << perfdata_events
    << " data_bin events are pending";
  if ((perfdata_events >= _queries_per_transaction)
      || data.isNull())
    _insert_perfdatas();

  return (1);
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

  // Delete index.
  while (1) {
    // Fetch next index to delete.
    unsigned long long index_id;
    {
      QSqlQuery q(*_storage_db);
      if (!q.exec("SELECT id FROM index_data WHERE to_delete=1 LIMIT 1")
          || q.lastError().isValid())
        throw (broker::exceptions::msg()
               << "storage: could not query index_data to get index to delete: "
               << q.lastError().text());
      if (!q.next())
        break ;
      index_id = q.value(0).toULongLong();
    }

    // Get associated metrics.
    std::list<unsigned long long> metrics_to_delete;
    {
      std::ostringstream oss;
      oss << "SELECT metric_id FROM metrics WHERE index_id=" << index_id;
      QSqlQuery q(*_storage_db);
      if (!q.exec(oss.str().c_str()) || q.lastError().isValid())
        throw (broker::exceptions::msg()
               << "storage: could not get metrics of index "
               << index_id);
      while (q.next())
        metrics_to_delete.push_back(q.value(0).toULongLong());
    }

    // Delete metrics.
    _delete_metrics(metrics_to_delete);
    deleted_metrics += metrics_to_delete.size();

    // Delete index from DB.
    {
      std::ostringstream oss;
      oss << "DELETE FROM index_data WHERE id=" << index_id;
      QSqlQuery q(*_storage_db);
      if (!q.exec(oss.str().c_str()) || q.lastError().isValid())
        logging::error(logging::low) << "storage: cannot delete index "
          << index_id << ": " << q.lastError().text();
    }
    ++deleted_index;

    // Remove associated graph.
    misc::shared_ptr<remove_graph> rg(new remove_graph);
    rg->id = index_id;
    rg->is_index = true;
    multiplexing::publisher().write(rg.staticCast<io::data>());
  }

  // Search standalone metrics to delete.
  std::list<unsigned long long> metrics_to_delete;
  {
    QSqlQuery q(*_storage_db);
    if (!q.exec("SELECT metric_id FROM metrics WHERE to_delete=1")
        || q.lastError().isValid())
      throw (broker::exceptions::msg()
             << "storage: could not get the list of metrics to delete");
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
 *  Clear QtSql objects.
 */
void stream::_clear_qsql() {
  _update_metrics.reset();
  if (_storage_db.get() && _storage_db->isOpen()) {
    _insert_perfdatas();
    if (_queries_per_transaction > 1)
      _storage_db->commit();
    _storage_db->close();
  }
  _storage_db.reset();
  return ;
}

/**
 *  Delete specified metrics.
 *
 *  @param[in] metrics_to_delete Metrics to delete.
 */
void stream::_delete_metrics(
               std::list<unsigned long long> const& metrics_to_delete) {
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
      oss << "DELETE FROM metrics WHERE metric_id=" << metric_id;
      QSqlQuery q(*_storage_db);
      if (!q.exec(oss.str().c_str()) || q.lastError().isValid())
        logging::error(logging::low)
          << "storage: cannot remove metric " << metric_id << ": "
          << q.lastError().text();
    }

    // Remove associated graph.
    misc::shared_ptr<remove_graph> rg(new remove_graph);
    rg->id = metric_id;
    rg->is_index = false;
    multiplexing::publisher().write(rg.staticCast<io::data>());
  }

  return ;
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
 *
 *  @return Index ID matching host and service ID.
 */
unsigned int stream::_find_index_id(
                       unsigned int host_id,
                       unsigned int service_id,
                       QString const& host_name,
                       QString const& service_desc,
                       unsigned int* rrd_len) {
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
      QString query("UPDATE index_data"
                    " SET host_name=:host_name,"
                    "     service_description=:service_description,"
                    "     special=:special"
                    " WHERE host_id=:host_id"
                    " AND service_id=:service_id");
      QSqlQuery q(*_storage_db);
      q.prepare(query);
      q.bindValue(":host_name", host_name);
      q.bindValue(":service_description", service_desc);
      q.bindValue(":special", (special ? 2 : 1));
      q.bindValue(":host_id", host_id);
      q.bindValue(":service_id", service_id);
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
    if (rrd_len)
      *rrd_len = it->second.rrd_retention;
  }
  // Can't find in cache, discard data or insert in DB.
  else {
    logging::info(logging::medium) << "storage: index not found for ("
      << host_id << ", " << service_id << ")";
    // Discard.
    if (!_insert_in_index_data)
      retval = 0;
    // Insert in index_data.
    else {
      logging::info(logging::medium)
        << "storage: creating new index for (" << host_id << ", "
        << service_id << ")";
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
          throw (broker::exceptions::msg() << "storage: could not " \
                    "fetch index_id of newly inserted index ("
                 << host_id << ", " << service_id << "): "
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
      info.rrd_retention = _rrd_len;
      _index_cache[std::make_pair(host_id, service_id)] = info;

      // Provide RRD retention.
      if (rrd_len)
        *rrd_len = info.rrd_retention;
    }
  }

  return (retval);
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
 *  @param[in,out] type        If not null, set to the metric type.
 *
 *  @return Metric ID requested, 0 if it could not be found not
 *          inserted.
 */
unsigned int stream::_find_metric_id(
                       unsigned int index_id,
                       QString metric_name,
                       QString const& unit_name,
                       double warn,
                       double warn_low,
                       bool warn_mode,
                       double crit,
                       double crit_low,
                       bool crit_mode,
                       double min,
                       double max,
                       unsigned int* type,
                       bool* locked) {
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
        || !double_equal(crit_low, it->second.crit_low)
        || (crit_mode != it->second.crit_mode)
        || !double_equal(max, it->second.max)
        || !double_equal(min, it->second.min)
        || !double_equal(warn, it->second.warn)
        || !double_equal(warn_low, it->second.warn_low)
        || (warn_mode != it->second.warn_mode)) {
      logging::info(logging::medium) << "storage: updating metric "
        << it->second.metric_id << " of (" << index_id << ", "
        << metric_name << ") (unit: " << unit_name << ", warning: "
        << warn_low << ":" << warn << ", critical: " << crit_low << ":"
        << crit << ", min: " << min << ", max: " << max << ")";
      // Update metrics table.
      _update_metrics->bindValue(":unit_name", unit_name);
      _update_metrics->bindValue(":warn", check_double(warn));
      _update_metrics->bindValue(":warn_low", check_double(warn_low));
      _update_metrics->bindValue(":warn_threshold_mode", warn_mode);
      _update_metrics->bindValue(":crit", check_double(crit));
      _update_metrics->bindValue(":crit_low", check_double(crit_low));
      _update_metrics->bindValue(":crit_threshold_mode", crit_mode);
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
      it->second.crit_low = crit_low;
      it->second.crit_mode = crit_mode;
      it->second.max = max;
      it->second.min = min;
      it->second.unit_name = unit_name;
      it->second.warn = warn;
      it->second.warn_low = warn_low;
      it->second.warn_mode = warn_mode;
    }

    // Anyway, we found the metric ID.
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
    // Build query.
    if (*type == perfdata::automatic)
      *type = perfdata::gauge;
    *locked = false;
    QString query(
              "INSERT INTO metrics "
              "  (index_id, metric_name, unit_name, warn, warn_low, "
              "   warn_threshold_mode, crit, crit_low, "
              "   crit_threshold_mode, min, max, data_source_type)"
              " VALUES (:index_id, :metric_name, :unit_name, :warn, "
              "         :warn_low, :warn_threshold_mode, :crit, "
              "         :crit_low, :crit_threshold_mode, :min, :max, "
              "         :data_source_type)");
    QSqlQuery q(*_storage_db);
    if (!q.prepare(query))
      throw (broker::exceptions::msg()
             << "storage: could not prepare metric insertion query: "
             << q.lastError().text());
    q.bindValue(":index_id", index_id);
    q.bindValue(":metric_name", metric_name);
    q.bindValue(":unit_name", unit_name);
    q.bindValue(":warn", check_double(warn));
    q.bindValue(":warn_low", check_double(warn_low));
    q.bindValue(":warn_threshold_mode", warn_mode);
    q.bindValue(":crit", check_double(crit));
    q.bindValue(":crit_low", check_double(crit_low));
    q.bindValue(":crit_threshold_mode", crit_mode);
    q.bindValue(":min", check_double(min));
    q.bindValue(":max", check_double(max));
    q.bindValue(":data_source_type", *type + 1);

    // Execute query.
    if (!q.exec() || q.lastError().isValid())
      throw (broker::exceptions::msg() << "storage: insertion of " \
                  "metric '" << metric_name << "' of index " << index_id
               << " failed: " << q.lastError().text());

    // Fetch insert ID with query if possible.
    if (!_storage_db->driver()->hasFeature(QSqlDriver::LastInsertId)
        || !(retval = q.lastInsertId().toUInt())) {
#if QT_VERSION >= 0x040302
      q.finish();
#endif // Qt >= 4.3.2
      QString query("SELECT metric_id"
                    " FROM metrics"
                    " WHERE index_id=:index_id"
                    " AND metric_name=:metric_name");
      QSqlQuery q2(*_storage_db);
      if (!q2.prepare(query))
        throw (broker::exceptions::msg()
               << "storage: could not prepare metric ID fetching query: "
               << q2.lastError().text());
      q2.bindValue(":index_id", index_id);
      q2.bindValue(":metric_name", metric_name);
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
    info.crit_low = crit_low;
    info.crit_mode = crit_mode;
    info.locked = false;
    info.max = max;
    info.metric_id = retval;
    info.min = min;
    info.type = *type;
    info.unit_name = unit_name;
    info.warn = warn;
    info.warn_low = warn_low;
    info.warn_mode = warn_mode;
    _metric_cache[std::make_pair(index_id, metric_name)] = info;
  }

  return (retval);
}

/**
 *  Insert performance data entries in the data_bin table.
 */
void stream::_insert_perfdatas() {
  if (!_perfdata_queue.empty()) {
    // Status.
    _update_status("status=inserting performance data\n");

    // Insert first entry.
    std::ostringstream query;
    {
      metric_value& mv(_perfdata_queue.front());
      query.precision(10);
      query << std::fixed
            << "INSERT INTO data_bin (id_metric, ctime, status, value)"
               " VALUES (" << mv.metric_id << ", " << mv.c_time << ", "
            << mv.status << ", " << mv.value << ")";
      _perfdata_queue.pop_front();
    }

    // Insert perfdata in data_bin.
    while (!_perfdata_queue.empty()) {
      metric_value& mv(_perfdata_queue.front());
      query << ", (" << mv.metric_id << ", " << mv.c_time << ", "
            << mv.status << ", " << mv.value << ")";
      _perfdata_queue.pop_front();
    }

    // Execute query.
    QSqlQuery q(*_storage_db);
    logging::debug(logging::low)
      << "storage: executing query: " << query.str().c_str();
    if (!q.exec(query.str().c_str())
        || q.lastError().isValid())
      throw (broker::exceptions::msg()
             << "storage: could not insert data in data_bin: "
             << q.lastError().text());
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

  // Prepare metrics update query.
  _update_metrics.reset(new QSqlQuery(*_storage_db));
  if (!_update_metrics->prepare("UPDATE metrics" \
                                " SET unit_name=:unit_name," \
                                " warn=:warn," \
                                " warn_low=:warn_low," \
                                " warn_threshold_mode=:warn_threshold_mode," \
                                " crit=:crit," \
                                " crit_low=:crit_low," \
                                " crit_threshold_mode=:crit_threshold_mode," \
                                " min=:min," \
                                " max=:max" \
                                " WHERE index_id=:index_id" \
                                " AND metric_name=:metric_name"))
    throw (broker::exceptions::msg() << "storage: could not prepare " \
                "metrics update query: "
             << _update_metrics->lastError().text());

  return ;
}

/**
 *  Rebuild cache.
 */
void stream::_rebuild_cache() {
  // Status.
  _update_status("status=rebuilding index and metrics cache\n");

  // Delete old cache.
  _index_cache.clear();
  _metric_cache.clear();

  // Fill index cache.
  {
    // Execute query.
    QSqlQuery q("SELECT id, host_id, service_id, host_name, rrd_retention, service_description, special" \
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
      info.rrd_retention = (q.value(4).isNull() ? 0 : q.value(4).toUInt());
      if (!info.rrd_retention)
        info.rrd_retention = _rrd_len;
      info.service_description = q.value(5).toString();
      info.special = (q.value(6).toUInt() == 2);
      logging::debug(logging::high) << "storage: loaded index "
        << info.index_id << " of (" << host_id << ", "
        << service_id << ")";
      _index_cache[std::make_pair(host_id, service_id)] = info;
    }
  }

  // Fill metric cache.
  {
    // Execute query.
    QSqlQuery q("SELECT metric_id, index_id, metric_name, data_source_type, unit_name, warn, warn_low, warn_threshold_mode, crit, crit_low, crit_threshold_mode, min, max, locked" \
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
      info.warn_low = q.value(6).toDouble();
      info.warn_mode = q.value(7).toUInt();
      info.crit = q.value(8).toDouble();
      info.crit_low = q.value(9).toDouble();
      info.crit_mode = q.value(10).toUInt();
      info.min = q.value(11).toDouble();
      info.max = q.value(12).toDouble();
      info.locked = (q.value(13).toUInt() == 1);
      logging::debug(logging::high) << "storage: loaded metric "
        << info.metric_id << " of (" << index_id << ", " << name
        << "), type " << info.type;
      _metric_cache[std::make_pair(index_id, name)] = info;
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
