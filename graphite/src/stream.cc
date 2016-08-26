/*
** Copyright 2011-2015 Centreon
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

#include <QByteArray>
#include <QMutexLocker>
#include <QTcpSocket>
#include <sstream>
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/graphite/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::graphite;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 */
stream::stream(
          std::string const& metric_naming,
          std::string const& status_naming,
          std::string const& db_user,
          std::string const& db_password,
          std::string const& db_host,
          unsigned short db_port,
          unsigned int queries_per_transaction,
          misc::shared_ptr<persistent_cache> const& cache)
  : _metric_naming(metric_naming),
    _status_naming(status_naming),
    _db_user(db_user),
    _db_password(db_password),
    _db_host(db_host),
    _db_port(db_port),
    _queries_per_transaction(queries_per_transaction == 0 ?
                               1 : queries_per_transaction),
    _pending_queries(0),
    _actual_query(0),
    _commit_flag(false),
    _cache(cache),
    _metric_query(_metric_naming, query::metric, _cache),
    _status_query(_status_naming, query::status, _cache) {
  // Create the basic HTTP authentification header.
  if (!_db_user.empty() && !_db_password.empty()) {
    QByteArray auth;
    auth
      .append(QString::fromStdString(_db_user))
      .append(":")
      .append(QString::fromStdString(_db_password));
    _auth_query
      .append("Authorization: Basic ")
      .append(QString(auth.toBase64()).toStdString())
      .append("\n");
    _query.append(_auth_query);
  }
  _socket = std::auto_ptr<QTcpSocket>(new QTcpSocket);
  _socket->connectToHost(QString::fromStdString(_db_host), _db_port);
  if (!_socket->waitForConnected())
    throw exceptions::msg()
          << "graphite: can't connect to graphite on host '"
          << _db_host << "', port '" << _db_port << "': "
          << _socket->errorString();
}

/**
 *  Destructor.
 */
stream::~stream() {
  if (_socket.get()) {
    _socket->close();
    _socket->waitForDisconnected();
  }
}

/**
 *  Flush the stream.
 *
 *  @return Number of events acknowledged.
 */
int stream::flush() {
  logging::debug(logging::medium)
    << "graphite: commiting " << _actual_query << " queries";
  int ret(_pending_queries);
  if (_actual_query != 0)
    _commit();
  _actual_query = 0;
  _pending_queries = 0;
  _commit_flag = false;
  return (ret);
}

/**
 *  Read from the database.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.clear();
  throw (com::centreon::broker::io::exceptions::shutdown(true, false)
         << "cannot read from Graphite database");
  return (true);
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
  return ;
}

/*
 * Do nothing.
 */
void stream::update() {
  return ;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(misc::shared_ptr<io::data> const& data) {
  if (!validate(data, "graphite"))
    return (1);

  ++_pending_queries;

  // Give the event to the cache.
  _cache.write(data);

  // Process metric events.
  if (data->type()
        == io::events::data_type<io::events::storage,
                                 storage::de_metric>::value) {
    if (_process_metric(data.ref_as<storage::metric const>()))
      ++_actual_query;
  }
  else if (data->type()
             == io::events::data_type<io::events::storage,
                                      storage::de_status>::value) {
    if (_process_status(data.ref_as<storage::status const>()))
      ++_actual_query;
  }
  if (_actual_query >= _queries_per_transaction)
    _commit_flag = true;

  if (_commit_flag)
    return (flush());
  else
    return (0);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Process a metric event.
 *
 *  @param[in] me  The event to process.
 */
bool stream::_process_metric(storage::metric const& me) {
  std::string to_append = _metric_query.generate_metric(me);
  _query.append(to_append);
  return (!to_append.empty());
}

/**
 *  Process a status event.
 *
 *  @param[in] st  The status event.
 */
bool stream::_process_status(storage::status const& st) {
  std::string to_append = _status_query.generate_status(st);
  _query.append(to_append);
  return (!to_append.empty());
}

/**
 *  Commit all the processed event to the database.
 */
void stream::_commit() {
  if (!_query.empty()) {
    if (_socket->write(_query.c_str(), _query.size()) == -1)
      throw exceptions::msg()
        << "graphite: can't send data to graphite on host '"
        << _db_host << "', port '" << _db_port << "': "
        << _socket->errorString();

    if (_socket->waitForBytesWritten() == false)
      throw exceptions::msg()
        << "graphite: can't send data to graphite on host '"
        << _db_host << "', port '" << _db_port << "': "
        << _socket->errorString();
  }

  _query.clear();
  _query.append(_auth_query);
}
