/*
** Copyright 2011 Merethis
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

#include <assert.h>
#include <math.h>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QVariant>
#include <sstream>
#include <stdlib.h>
#include "events/perfdata.hh"
#include "events/service_status.hh"
#include "exceptions/basic.hh"
#include "logging/logging.hh"
#include "multiplexing/publisher.hh"
#include "storage/parser.hh"
#include "storage/perfdata.hh"
#include "storage/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

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
  assert(false);
  abort();
  return (*this);
}

/**
 *  Clear QtSql objects.
 */
void stream::_clear_qsql() {
  _insert_data_bin.reset();
  _update_metrics.reset();
  _centreon_db.reset();
  _storage_db.reset();
  return ;
}

/**
 *  @brief Find index ID.
 *
 *  Look through the index cache for the specified index. If it cannot
 *  be found, insert an entry in the database.
 *
 *  @param[in] host_id    Host ID associated to the index.
 *  @param[in] service_id Service ID associated to the index.
 *
 *  @return Index ID matching host and service ID.
 */
unsigned int stream::_find_index_id(unsigned int host_id,
                                               unsigned int service_id) {
  unsigned int retval;

  // Look in the cache.
  std::map<std::pair<unsigned int, unsigned int>, unsigned int>::const_iterator it(
    _index_cache.find(std::make_pair(host_id, service_id)));
  if (it != _index_cache.end())
    retval = it->second;

  // Can't find in cache, insert in DB.
  else {
    // Build query.
    std::ostringstream oss;
    oss << "INSERT INTO index_data (host_id, service_id)" \
           " VALUES (" << host_id << ", " << service_id << ")";

    // Execute query.
    QSqlQuery q(_storage_db->exec(oss.str().c_str()));
    if (_storage_db->lastError().isValid())
      throw (exceptions::basic() << "storage: insertion of index (" << host_id << ", "
               << service_id << ") failed: "
               << _storage_db->lastError().text().toStdString().c_str());

    // Fetch insert ID with query if possible.
    if (_storage_db->driver()->hasFeature(QSqlDriver::LastInsertId)
        || !(retval = q.lastInsertId().toUInt())) {
      q.finish();
      std::ostringstream oss2;
      oss2 << "SELECT id" \
              " FROM index_data" \
              " WHERE host_id=" << host_id
           << " AND service_id=" << service_id;
      QSqlQuery q2(_storage_db->exec(oss2.str().c_str()));
      if (_storage_db->lastError().isValid())
        throw (exceptions::basic() << "storage: could not fetch index_id of newly inserted index ("
                 << host_id << ", " << service_id << "): "
                 << _storage_db->lastError().text().toStdString().c_str());
      retval = q2.value(0).toUInt();
      if (!retval)
        throw (exceptions::basic() << "storage: index_data table is corrupted: got 0 as index_id");
    }

    // Insert index in cache.
    _index_cache[std::make_pair(host_id, service_id)] = retval;
  }

  return (retval);
}

/**
 *  @brief Find metric ID.
 *
 *  Look through the metric cache for the specified metric. If it cannot
 *  be found, insert an entry in the database.
 *
 *  @param[in] index_id    Index ID of the metric.
 *  @param[in] metric_name Name of the metric.
 *
 *  @return Metric ID requested, 0 if it could not be found not
 *          inserted.
 */
unsigned int stream::_find_metric_id(unsigned int index_id,
                                     QString const& metric_name) {
  unsigned int retval;

  // Look in the cache.
  std::map<std::pair<unsigned int, QString>, unsigned int>::const_iterator it
    = _metric_cache.find(std::make_pair(index_id, metric_name));
  if (it != _metric_cache.end())
    retval = it->second;

  // Can't find in cache, insert in DB.
  else {
    // Build query.
    std::ostringstream oss;
    std::string escaped_metric_name(_storage_db->driver()->formatValue(QSqlField(metric_name.toStdString().c_str(),
        QVariant::String),
      true).toStdString());
    oss << "INSERT INTO metrics (index_id, metric_name, must_be_rebuild)" \
      " VALUES (" << index_id << ", " << escaped_metric_name << ", 1)";

    // Execute query.
    QSqlQuery q(_storage_db->exec(oss.str().c_str()));
    if (_storage_db->lastError().isValid())
      throw (exceptions::basic() << "storage: insertion of metric '" << metric_name.toStdString().c_str()
               << "' of index " << index_id << " failed: "
               << _storage_db->lastError().text().toStdString().c_str());

    // Fetch insert ID with query if possible.
    if (_storage_db->driver()->hasFeature(QSqlDriver::LastInsertId)
        || !(retval = q.lastInsertId().toUInt())) {
      q.finish();
      std::ostringstream oss2;
      oss2 << "SELECT metric_id" \
              " FROM metrics" \
              " WHERE index_id=" << index_id
           << " AND metric_name=" << escaped_metric_name;
      QSqlQuery q2(_storage_db->exec(oss2.str().c_str()));
      if (_storage_db->lastError().isValid())
        throw (exceptions::basic() << "storage: could not fetch metric_id of newly inserted metric '"
                 << metric_name.toStdString().c_str() << "' of index " << index_id
                 << ": " << _storage_db->lastError().text().toStdString().c_str());
      retval = q2.value(0).toUInt();
      if (!retval)
        throw (exceptions::basic() << "storage: metrics table is corrupted: got 0 as metric_id");
    }

    // Insert metric in cache.
    _metric_cache[std::make_pair(index_id, metric_name)] = retval;
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
    QSqlQuery q(_storage_db->exec("SELECT id, host_id, service_id" \
                                  " FROM index_data"));
    if (_storage_db->lastError().isValid())
      throw (exceptions::basic() << "storage: could not fetch index list from data DB: "
               << _storage_db->lastError().text().toStdString().c_str());

    // Loop through result set.
    while (q.next())
      _index_cache[std::make_pair(q.value(1).toUInt(),
        q.value(2).toUInt())]
        = q.value(0).toUInt();
  }

  // Fill metric cache.
  {
    // Execute query.
    QSqlQuery q(_storage_db->exec("SELECT metric_id, index_id, metric_name" \
                                  " FROM metrics"));
    if (_storage_db->lastError().isValid())
      throw (exceptions::basic() << "storage: could not fetch metric list from data DB: "
               << _storage_db->lastError().text().toStdString().c_str());

    // Loop through result set.
    while (q.next())
      _metric_cache[std::make_pair(q.value(1).toUInt(),
        q.value(2).toString())]
          = q.value(0).toUInt();
  }

  // Fetch configuration from configuration DB.
  {
    QSqlQuery q(_centreon_db->exec("SELECT interval_length" \
                                   " FROM cfg_nagios" \
                                   " WHERE interval_length IS NOT NULL AND interval_length > 0" \
                                   " ORDER BY interval_length"));
    if (_centreon_db->lastError().isValid() || !q.next()) {
      logging::config << logging::HIGH << "storage: could not get interval length, assuming 60 seconds";
      _interval_length = 60;
    }
    else {
      _interval_length = q.value(0).toUInt();
      if (!_interval_length)
        _interval_length = 60;
      logging::config << logging::MEDIUM << "storage: interval length is "
        << static_cast<unsigned int>(_interval_length) << " seconds";
    }
  }

  // Fetch configuration from data DB.
  {
    QSqlQuery q(_storage_db->exec("SELECT len_storage_rrd, RRDdatabase_path, storage_type" \
                                  " FROM config"));
    if (_storage_db->lastError().isValid())
      throw (exceptions::basic() << "storage: could not get configuration from DB: "
               << _storage_db->lastError().text().toStdString().c_str());
    _rrd_len = q.value(0).toUInt() * 24 * 60 * 60 / _interval_length;
    logging::config << logging::MEDIUM << "storage: RRD length is "
      << _rrd_len << " seconds";
    _metrics_path = q.value(1).toString();
    logging::config << logging::MEDIUM << "storage: metrics path is "
      << _metrics_path.toStdString().c_str();
    _store_in_db = (q.value(2).toUInt() == 2);
    if (_store_in_db)
      logging::config << logging::MEDIUM << "storage: will store in 'data_bin'";
    else
      logging::config << logging::MEDIUM << "storage: will not store in 'data_bin'";
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
    throw (exceptions::basic() << "storage: could not prepare metrics update query: "
             << _update_metrics->lastError().text().toStdString().c_str());

  // Prepare data_bind insert query.
  _insert_data_bin.reset(new QSqlQuery(*_storage_db));
  if (!_insert_data_bin->prepare("INSERT INTO data_bin (" \
                                 " id_metric, ctime, value, status)" \
                                 " VALUES (:id_metric," \
                                 " :ctime," \
                                 " :value," \
                                 " :status)"))
    throw (exceptions::basic() << "storage: could not prepare data_bin insert query: "
             << _insert_data_bin->lastError().text().toStdString().c_str());

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] centreon_type     Centreon DB type.
 *  @param[in] centreon_host     Centreon DB host.
 *  @param[in] centreon_port     Centreon DB port.
 *  @param[in] centreon_user     Centreon DB user.
 *  @param[in] centreon_password Centreon DB password.
 *  @param[in] centreon_db       Centreon DB name.
 *  @param[in] storage_type      Storage DB type.
 *  @param[in] storage_host      Storage DB host.
 *  @param[in] storage_port      Storage DB port.
 *  @param[in] storage_user      Storage DB user.
 *  @param[in] storage_password  Storage DB password.
 *  @param[in] storage_db        Storage DB name.
 */
stream::stream(QString const& centreon_type,
               QString const& centreon_host,
               unsigned short centreon_port,
               QString const& centreon_user,
               QString const& centreon_password,
               QString const& centreon_db,
               QString const& storage_type,
               QString const& storage_host,
               unsigned short storage_port,
               QString const& storage_user,
               QString const& storage_password,
               QString const& storage_db) {
  // Centreon connection ID.
  QString centreon_id;
  centreon_id.setNum((qulonglong)this, 16);
  centreon_id.append("Centreon");

  // Add database connection.
  _centreon_db.reset(new QSqlDatabase(QSqlDatabase::addDatabase(centreon_type, centreon_id)));
  if (centreon_type == "QMYSQL")
    _centreon_db->setConnectOptions("CLIENT_FOUND_ROWS");

  // Open database.
  _centreon_db->setHostName(centreon_host);
  _centreon_db->setPort(centreon_port);
  _centreon_db->setUserName(centreon_user);
  _centreon_db->setPassword(centreon_password);
  _centreon_db->setDatabaseName(centreon_db);
  if (!_centreon_db->open()) {
    _clear_qsql();
    QSqlDatabase::removeDatabase(centreon_id);
    throw (exceptions::basic() << "storage: could not connect to Centreon database");
  }

  // Storage connection ID.
  QString storage_id;
  storage_id.setNum((qulonglong)this, 16);
  storage_id.append("Storage");

  // Add database connection.
  _storage_db.reset(new QSqlDatabase(QSqlDatabase::addDatabase(storage_type, storage_id)));
  if (centreon_type == "QMYSQL")
    _storage_db->setConnectOptions("CLIENT_FOUND_ROWS");

  // Open database.
  _storage_db->setHostName(storage_host);
  _storage_db->setPort(storage_port);
  _storage_db->setUserName(storage_user);
  _storage_db->setPassword(storage_password);
  _storage_db->setDatabaseName(storage_db);
  if (!_storage_db->open()) {
    _clear_qsql();
    QSqlDatabase::removeDatabase(centreon_id);
    QSqlDatabase::removeDatabase(storage_id);
    throw (exceptions::basic() << "storage: could not connect to Centreon Storage database");
  }

  // Prepare queries.
  try {
    _prepare();
  }
  catch (...) {
    _clear_qsql();
    QSqlDatabase::removeDatabase(centreon_id);
    QSqlDatabase::removeDatabase(storage_id);
    throw ;
  }
}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s) : io::stream(s) {
  // Centreon connection ID.
  QString centreon_id;
  centreon_id.setNum((qulonglong)this, 16);
  centreon_id.append("Centreon");

  // Clone Centreon database.
  _centreon_db.reset(new QSqlDatabase(QSqlDatabase::cloneDatabase(*s._centreon_db, centreon_id)));

  // Open Centreon database.
  if (!_centreon_db->open()) {
    _clear_qsql();
    QSqlDatabase::removeDatabase(centreon_id);
    throw (exceptions::basic() << "storage: could not connect to Centreon database");
  }

  // Storage connection ID.
  QString storage_id;
  storage_id.setNum((qulonglong)this, 16);
  storage_id.append("Storage");

  // Clone Storage database.
  _storage_db.reset(new QSqlDatabase(QSqlDatabase::cloneDatabase(*s._storage_db, storage_id)));

  // Open Storage database.
  if (!_storage_db->open()) {
    _clear_qsql();
    QSqlDatabase::removeDatabase(centreon_id);
    QSqlDatabase::removeDatabase(storage_id);
    throw (exceptions::basic() << "storage: could not connect to Centreon Storage database");
  }

  // Prepare queries.
  try {
    _prepare();
  }
  catch (...) {
    _clear_qsql();
    QSqlDatabase::removeDatabase(centreon_id);
    QSqlDatabase::removeDatabase(storage_id);
    throw ;
  }
}

/**
 *  Destructor.
 */
stream::~stream() {
  QString centreon_id;
  centreon_id.setNum((qulonglong)this, 16);
  QString storage_id;
  storage_id.setNum((qulonglong)this, 16);
  _clear_qsql();
  QSqlDatabase::removeDatabase(centreon_id);
  QSqlDatabase::removeDatabase(storage_id);
}

/**
 *  Read from the datbase.
 *
 *  @return Does not return, throw an exception.
 */
QSharedPointer<io::data> stream::read() {
  throw (exceptions::basic() << "storage: attempt to read from a storage stream (software bug)");
  return (QSharedPointer<io::data>());
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 */
void stream::write(QSharedPointer<io::data> data) {
  // Process service status events.
  if (data->type() == events::event::SERVICESTATUS) {
    logging::debug << logging::HIGH << "storage: processing service status event";
    QSharedPointer<events::service_status> ss(data.staticCast<events::service_status>());

    // Parse perfdata.
    std::list<perfdata> pds;
    parser p;
    p.parse_perfdata(ss->perf_data, pds);

    // Loop through all metrics.
    for (std::list<perfdata>::iterator it = pds.begin(), end = pds.end();
         it != end;
         ++it) {
      perfdata& pd(*it);

      // Find index_id.
      unsigned int index_id(_find_index_id(ss->host_id, ss->service_id));

      // Find metric_id.
      unsigned int metric_id(_find_metric_id(index_id, pd.name()));

      // Update metrics table.
      _update_metrics->bindValue(":unit_name", pd.unit());
      _update_metrics->bindValue(":warn", check_double(pd.warning()));
      _update_metrics->bindValue(":crit", check_double(pd.critical()));
      _update_metrics->bindValue(":min", check_double(pd.min()));
      _update_metrics->bindValue(":max", check_double(pd.max()));
      _update_metrics->bindValue(":index_id", index_id);
      _update_metrics->bindValue(":metric_name", pd.name());
      _update_metrics->exec();

      if (_store_in_db) {
        // Insert perfdata in data_bin.
        _insert_data_bin->bindValue(":id_metric", metric_id);
        _insert_data_bin->bindValue(":ctime", static_cast<unsigned int>(ss->execution_time));
        _insert_data_bin->bindValue(":value", pd.value());
        _insert_data_bin->bindValue(":status", ss->current_state);
        _insert_data_bin->exec();
      }

      // Send perfdata event to processing.
      logging::debug << logging::HIGH << "storage: generating perfdata event";
      QSharedPointer<events::perfdata> perf(new events::perfdata);
      perf->ctime = ss->execution_time;
      perf->interval = ss->check_interval * _interval_length;
      perf->metric_id = metric_id;
      perf->name = pd.name();
      perf->rrd_len = _rrd_len;
      perf->status = ss->current_state;
      perf->value = pd.value();
      multiplexing::publisher().write(perf.staticCast<io::data>());
    }
  }
  return ;
}
