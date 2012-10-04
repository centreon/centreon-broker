/*
** Copyright 2012 Merethis
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

#include <ctime>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/shared_ptr.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/rebuild.hh"
#include "com/centreon/broker/storage/status.hh"
#include "com/centreon/broker/storage/rebuilder.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

// Local types.
struct index_info {
  unsigned int index_id;
  unsigned int host_id;
  unsigned int service_id;
};

struct metric_info {
  unsigned int metric_id;
  QString metric_name;
  short metric_type;
};

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
rebuilder::rebuilder()
  : _interval(600),
    _interval_length(60),
    _rrd_len(15552000),
    _should_exit(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
rebuilder::rebuilder(rebuilder const& right) : QThread() {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
rebuilder::~rebuilder() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
rebuilder& rebuilder::operator=(rebuilder const& right) {
  if (this != &right) {
    if (_db.isOpen())
      _db.close();
    if (!_db.connectionName().isEmpty())
      QSqlDatabase::removeDatabase(_db.connectionName());
    _internal_copy(right);
  }
  return (*this);
}

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
unsigned int rebuilder::get_interval() const throw () {
  return (_interval);
}

/**
 *  Get the interval length in seconds.
 *
 *  @return Interval length in seconds.
 */
time_t rebuilder::get_interval_length() const throw () {
  return (_interval_length);
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
  while (!_should_exit) {
    try {
      // Open DB.
      if (!_db.open())
        throw (broker::exceptions::msg() << "storage: rebuilder: could "
               "not connect to Centreon Storage database");

      try {
        // Fetch index to rebuild.
        std::list<index_info> index_to_rebuild;
        {
          QSqlQuery index_to_rebuild_query(_db);
          if (!index_to_rebuild_query.exec(
                 "SELECT id, host_id, service_id"
                 " FROM index_data"
                 " WHERE must_be_rebuild='1'"))
            throw (broker::exceptions::msg() << "storage: rebuilder: "
                      "could not fetch index to rebuild: "
                   << index_to_rebuild_query.lastError().text());
          while (!_should_exit && index_to_rebuild_query.next()) {
            index_info info;
            info.index_id = index_to_rebuild_query.value(0).toUInt();
            info.host_id = index_to_rebuild_query.value(1).toUInt();
            info.service_id = index_to_rebuild_query.value(2).toUInt();
            index_to_rebuild.push_back(info);
          }
        }

        // Browse list of index to rebuild.
        while (!_should_exit && !index_to_rebuild.empty()) {
          // Get check interval of host/service.
          unsigned int index_id;
          unsigned int check_interval(0);
          {
            index_info info(index_to_rebuild.front());
            index_id = info.index_id;
            index_to_rebuild.pop_front();

            std::ostringstream oss;
            if (!info.service_id)
              oss << "SELECT check_interval"
                  << " FROM hosts"
                  << " WHERE host_id=" << info.host_id;
            else
              oss << "SELECT check_interval"
                  << " FROM services"
                  << " WHERE host_id=" << info.host_id
                  << "  AND service_id=" << info.service_id;
            QSqlQuery query(_db);
            if (query.exec(oss.str().c_str()) && query.next())
              check_interval = query.value(0).toUInt();
            if (!check_interval)
              check_interval = 5;
          }
          logging::info(logging::medium) << "storage: rebuilder: index "
            << index_id << " (interval " << check_interval
            << ") will be rebuild";

          // Set index as being rebuilt.
          _set_index_rebuild(index_id, 2);

          try {
            // Fetch metrics to rebuild.
            std::list<metric_info> metrics_to_rebuild;
            {
              std::ostringstream oss;
              oss << "SELECT metric_id, metric_name, data_source_type"
                  << " FROM metrics"
                  << " WHERE index_id=" << index_id;
              QSqlQuery metrics_to_rebuild_query(_db);
              if (!metrics_to_rebuild_query.exec(oss.str().c_str()))
                throw (broker::exceptions::msg()
                       << "storage: rebuilder: could not fetch "
                       << "metrics of index " << index_id);
              while (!_should_exit && metrics_to_rebuild_query.next()) {
                metric_info info;
                info.metric_id
                  = metrics_to_rebuild_query.value(0).toUInt();
                info.metric_name
                  = metrics_to_rebuild_query.value(1).toString();
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
                info.metric_id,
                info.metric_name,
                info.metric_type,
                check_interval * _interval_length);
            }

            // Rebuild status.
            _rebuild_status(
              index_id,
              check_interval * _interval_length);
          }
          catch (...) {
            // Set index as rebuilt.
            _set_index_rebuild(index_id, 0);

            // Rethrow exception.
            throw ;
          }

          // Set index as rebuilt.
          if (!_should_exit)
            _set_index_rebuild(index_id, 0);
        }

        // Close DB.
        _db.close();
      }
      catch (...) {
        // Close DB and rethrow.
        _db.close();
        throw ;
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
    time_t target(time(NULL) + _interval);
    while (!_should_exit && (target > time(NULL)))
      sleep(1);
  }
  return ;
}

/**
 *  Set the database object.
 *
 *  @param[in] db DB object to copy.
 */
void rebuilder::set_db(QSqlDatabase const& db) {
  // Connection ID.
  QString id;
  id.setNum((qulonglong)this, 16);

  // Remove old DB.
  QSqlDatabase::removeDatabase(id);

  // Clone database.
  _db = QSqlDatabase::cloneDatabase(db, id);

  return ;
}

/**
 *  Set the rebuild check interval.
 *
 *  @param[in] interval Rebuild check interval in seconds.
 */
void rebuilder::set_interval(unsigned int interval) throw () {
  _interval = interval;
  return ;
}

/**
 *  Set the interval length.
 *
 *  @param[in] interval_length Interval length in seconds.
 */
void rebuilder::set_interval_length(time_t interval_length) throw () {
  _interval_length = interval_length;
  return ;
}

/**
 *  Set the RRD length.
 *
 *  @param[in] rrd_length RRD length in seconds.
 */
void rebuilder::set_rrd_length(unsigned int rrd_length) throw () {
  _rrd_len = rrd_length;
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void rebuilder::_internal_copy(rebuilder const& right) {
  // Copy DB.
  QString id;
  id.setNum((qulonglong)this, 16);
  _db = QSqlDatabase::cloneDatabase(right._db, id);

  // Copy other data.
  _interval = right._interval;
  _interval_length = right._interval_length;
  _rrd_len = right._rrd_len;

  return ;
}

/**
 *  Rebuild a metric.
 *
 *  @param[in] metric_id   Metric ID.
 *  @param[in] metric_name Metric name.
 *  @param[in] type        Metric type.
 *  @param[in] interval    Host/service check interval.
 */
void rebuilder::_rebuild_metric(
                  unsigned int metric_id,
                  QString const& metric_name,
                  short metric_type,
                  unsigned int interval) {
  // Log.
  logging::info(logging::low)
    << "storage: rebuilder: rebuilding metric " << metric_id
    << " (name " << metric_name << ", type " << metric_type
    << ", interval " << interval << ")";

  // Send rebuild start event.
  _send_rebuild_event(false, metric_id, false);

  try {
    // Get data.
    std::ostringstream oss;
    oss << "SELECT ctime, value"
        << " FROM data_bin"
        << " WHERE id_metric=" << metric_id
        << " ORDER BY ctime ASC";
    QSqlQuery data_bin_query(_db);
    if (data_bin_query.exec(oss.str().c_str()))
      while (data_bin_query.next()) {
        misc::shared_ptr<storage::metric> entry(new storage::metric);
        entry->ctime = data_bin_query.value(0).toUInt();
        entry->interval = interval;
        entry->metric_id = metric_id;
        entry->name = metric_name;
        entry->rrd_len = _rrd_len;
        entry->value = data_bin_query.value(1).toDouble();
        entry->value_type = metric_type;
        multiplexing::publisher().write(entry.staticCast<io::data>());
      }
    else
      logging::error(logging::medium) << "storage: rebuilder: "
        << "cannot fetch data of metric " << metric_id << ": "
        << data_bin_query.lastError().text();
  }
  catch (...) {
    // Send rebuild end event.
    _send_rebuild_event(true, metric_id, false);

    // Rethrow exception.
    throw ;
  }

  // Send rebuild end event.
  _send_rebuild_event(true, metric_id, false);

  return ;
}

/**
 *  Rebuild a status.
 *
 *  @param[in] index_id Index ID.
 *  @param[in] interval Host/service check interval.
 */
void rebuilder::_rebuild_status(
                  unsigned int index_id,
                  unsigned int interval) {
  // Log.
  logging::info(logging::low)
    << "storage: rebuilder: rebuilding status " << index_id
    << "(interval " << interval << ")";

  // Send rebuild start event.
  _send_rebuild_event(false, index_id, true);

  try {
    // Get data.
    std::ostringstream oss;
    oss << "SELECT d.ctime, d.status"
        << " FROM metrics AS m"
        << " JOIN data_bin AS d"
        << " ON m.metric_id=d.id_metric"
        << " WHERE m.index_id=" << index_id
        << " ORDER BY d.ctime ASC";
    QSqlQuery data_bin_query(_db);
    if (data_bin_query.exec(oss.str().c_str()))
      while (data_bin_query.next()) {
        misc::shared_ptr<storage::status> entry(new storage::status);
        entry->ctime = data_bin_query.value(0).toUInt();
        entry->index_id = index_id;
        entry->interval = interval;
        entry->rrd_len = _rrd_len;
        entry->state = data_bin_query.value(1).toInt();
        multiplexing::publisher().write(entry.staticCast<io::data>());
      }
    else
      logging::error(logging::medium) << "storage: rebuilder: "
        << "cannot fetch data of index " << index_id << ": "
        << data_bin_query.lastError().text();
  }
  catch (...) {
    // Send rebuild end event.
    _send_rebuild_event(true, index_id, true);

    // Rethrow exception.
    throw ;
  }

  // Send rebuild end event.
  _send_rebuild_event(true, index_id, true);

  return ;
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
  misc::shared_ptr<storage::rebuild> rb(new storage::rebuild);
  rb->end = end;
  rb->id = id;
  rb->is_index = is_index;
  multiplexing::publisher().write(rb.staticCast<io::data>());
  return ;
}

/**
 *  Set index rebuild flag.
 *
 *  @param[in] index_id Index to update.
 *  @param[in] state    Rebuild state (0, 1 or 2).
 */
void rebuilder::_set_index_rebuild(unsigned int index_id, short state) {
  std::ostringstream oss;
  oss << "UPDATE index_data"
      << " SET must_be_rebuild=" << state + 1
      << " WHERE id=" << index_id;
  QSqlQuery update_index_query(_db);
  if (!update_index_query.exec(oss.str().c_str()))
    logging::error(logging::low)
      << "storage: rebuilder: cannot update state of index "
      << index_id << ": " << update_index_query.lastError().text();
  return ;
}
