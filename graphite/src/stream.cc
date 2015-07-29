/*
** Copyright 2011-2014 Merethis
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
          unsigned int queries_per_transaction)
  : _process_out(true),
    _metric_naming(metric_naming),
    _status_naming(status_naming),
    _db_user(db_user),
    _db_password(db_password),
    _db_host(db_host),
    _db_port(db_port),
    _queries_per_transaction(queries_per_transaction == 0 ?
                               1 : queries_per_transaction),
    _pending_queries(0),
    _actual_query(0),
    _metric_query(_metric_naming, query::metric),
    _status_query(_status_naming, query::status) {
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
}

/**
 *  Destructor.
 */
stream::~stream() {
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
 *  Read from the database.
 *
 *  @param[out] d Cleared.
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  throw (com::centreon::broker::exceptions::msg()
         << "graphite: attempt to read from a graphite stream (not supported yet)");
  return ;
}

/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(io::properties& tree) const {
  QMutexLocker lock(&_statusm);
  if (!_status.empty()) {
    io::property& p(tree["status"]);
    p.set_perfdata(_status);
    p.set_graphable(false);
  }
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
unsigned int stream::write(misc::shared_ptr<io::data> const& data) {
  // Check that processing is enabled.
  if (!_process_out)
    throw (io::exceptions::shutdown(true, true)
             << "graphite stream is shutdown");

  bool commit = false;
  ++_pending_queries;

  // Process metric events.
  if (!data.isNull()) {
    if (data->type()
          == io::events::data_type<io::events::storage,
                                   storage::de_metric>::value) {
      _process_metric(data.ref_as<storage::metric const>());
      ++_actual_query;
    }
    else if (data->type()
               == io::events::data_type<io::events::storage,
                                        storage::de_status>::value) {
      _process_status(data.ref_as<storage::status const>());
      ++_actual_query;
    }
    if (_actual_query >= _queries_per_transaction)
      commit = true;
  }
  else
    commit = true;

  if (commit) {
    logging::debug(logging::medium)
      << "graphite: commiting " << _actual_query << " queries";
    unsigned int ret = _pending_queries;
    if (_actual_query != 0)
      _commit();
    _actual_query = 0;
    _pending_queries = 0;
    return (ret);
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
 *  Process a metric event.
 *
 *  @param[in] me  The event to process.
 */
void stream::_process_metric(storage::metric const& me) {
  _query.append(_metric_query.generate_metric(me));
}

/**
 *  Process a status event.
 *
 *  @param[in] st  The status event.
 */
void stream::_process_status(storage::status const& st) {
  _query.append(_status_query.generate_status(st));
}

/**
 *  Commit all the processed event to the database.
 */
void stream::_commit() {
  std::auto_ptr<QTcpSocket> connect(new QTcpSocket);

  connect->connectToHost(QString::fromStdString(_db_host), _db_port);
  if (!connect->waitForConnected())
    throw exceptions::msg()
          << "graphite: can't connect to graphite on host '"
          << _db_host << "', port '" << _db_port << "': "
          << connect->errorString();

  if (connect->write(_query.c_str(), _query.size()) == -1)
    throw exceptions::msg()
      << "graphite: can't send data to graphite on host '"
      << _db_host << "', port '" << _db_port << "': "
      << connect->errorString();

  if (connect->waitForBytesWritten() == false)
    throw exceptions::msg()
      << "graphite: can't send data to graphite on host '"
      << _db_host << "', port '" << _db_port << "': "
      << connect->errorString();

  connect->close();
  connect->waitForDisconnected();
  _query.clear();
  _query.append(_auth_query);
}
