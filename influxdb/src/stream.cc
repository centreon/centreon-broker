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

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <QSqlDriver>
#include <QThread>
#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/influxdb/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::influxdb;

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
 */
stream::stream(
          std::string const& user,
          std::string const& passwd,
          std::string const& addr,
          std::string const& db,
          unsigned int queries_per_transaction)
  : _process_out(false),
    _user(user),
    _password(passwd),
    _address(addr),
    _db(db),
    _queries_per_transaction(queries_per_transaction),
    _actual_query(0) {
  // Register with multiplexer.
  multiplexing::engine::instance().hook(*this, false);
}

/**
 *  Destructor.
 */
stream::~stream() {
  // Unregister from multiplexer.
  multiplexing::engine::instance().unhook(*this);
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
         << "storage: attempt to read from an influxdb stream (not supported yet)");
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
             << "influxdb stream is shutdown");

  // Process metric events.
  if (!data.isNull()) {

    ++_actual_query;
  }

  if (_actual_query >= _queries_per_transaction) {
    unsigned int ret = _actual_query;
    _actual_query = 0;
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
