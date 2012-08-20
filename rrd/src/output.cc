/*
** Copyright 2011 Merethis
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

#include <assert.h>
#include <sstream>
#include <stdlib.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/rrd/cached.hh"
#include "com/centreon/broker/rrd/exceptions/open.hh"
#include "com/centreon/broker/rrd/exceptions/update.hh"
#include "com/centreon/broker/rrd/lib.hh"
#include "com/centreon/broker/rrd/output.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  Any call to this constructor will result in a call to abort().
 *
 *  @param[in] o Object to copy.
 */
output::output(output const& o) : io::stream(o) {
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Any call to this method will result in a call to abort().
 *
 *  @param[in] o Object to copy.
 *
 *  @return This object.
 */
output& output::operator=(output const& o) {
  (void)o;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Standard constructor.
 *
 *  @param[in] metrics_path Path in which metrics RRD files should be
 *                          written.
 *  @param[in] status_path  Path in which status RRD files should be
 *                          written.
 */
output::output(QString const& metrics_path, QString const& status_path)
  : _backend(new lib),
    _metrics_path(metrics_path),
    _process_out(true),
    _status_path(status_path) {}

/**
 *  Local socket constructor.
 *
 *  @param[in] metrics_path See standard constructor.
 *  @param[in] status_path  See standard constructor.
 *  @param[in] local        Local socket connection parameters.
 */
output::output(QString const& metrics_path,
               QString const& status_path,
               QString const& local)
  : _metrics_path(metrics_path),
    _process_out(true),
    _status_path(status_path) {
  QScopedPointer<cached> rrdcached(new cached);
  rrdcached->connect_local(local);
  _backend.reset(rrdcached.take());
}

/**
 *  Network socket constructor.
 *
 *  @param[in] metrics_path See standard constructor.
 *  @param[in] status_path  See standard constructor.
 *  @param[in] port         rrdcached listening port.
 */
output::output(QString const& metrics_path,
               QString const& status_path,
               unsigned short port)
  : _metrics_path(metrics_path),
    _process_out(true),
    _status_path(status_path) {
  QScopedPointer<cached> rrdcached(new cached);
  rrdcached->connect_remote("localhost", port);
  _backend.reset(rrdcached.take());
}

/**
 *  Destructor.
 */
output::~output() {}

/**
 *  Set if output should be processed or not.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Set to true to process output events.
 */
void output::process(bool in, bool out) {
  (void)in;
  _process_out = out;
  return ;
}

/**
 *  Read data.
 *
 *  @return Does not return, throw an exception.
 */
QSharedPointer<io::data> output::read() {
  throw (broker::exceptions::msg()
           << "RRD: attempt to read data from an output endpoint");
  return (QSharedPointer<io::data>());
}

/**
 *  Write an event.
 *
 *  @param[in] d Data to write.
 */
void output::write(QSharedPointer<io::data> d) {
  if (!_process_out)
    throw (io::exceptions::shutdown(true, true)
             << "RRD output is shutdown");
  if (d->type() == "com::centreon::broker::storage::metric") {
    // Debug message.
    logging::debug << logging::MEDIUM << "RRD: new metric data";
    QSharedPointer<storage::metric> e(d.staticCast<storage::metric>());

    // Write metrics RRD.
    std::ostringstream oss1;
    oss1 << _metrics_path.toStdString() << "/" << e->metric_id << ".rrd";
    try {
      _backend->open(oss1.str().c_str(), e->name);
    }
    catch (exceptions::open const& b) {
      _backend->open(oss1.str().c_str(),
        e->name,
        e->rrd_len / (e->interval ? e->interval : 60) + 1,
        0,
        e->interval);
    }
    std::ostringstream oss2;
    oss2 << e->value;
    try {
      _backend->update(e->ctime, oss2.str().c_str());
    }
    catch (exceptions::update const& b) {
      logging::error(logging::low) << b.what() << " (ignored)";
    }
  }
  else if (d->type() == "com::centreon::broker::storage::status") {
    // Debug message.
    logging::debug << logging::MEDIUM << "RRD: new status data";
    QSharedPointer<storage::status> e(d.staticCast<storage::status>());

    // Write status RRD.
    std::ostringstream oss1;
    oss1 << _status_path.toStdString() << "/" << e->index_id << ".rrd";
    try {
      _backend->open(oss1.str().c_str(), "status");
    }
    catch (exceptions::open const& b) {
      _backend->open(oss1.str().c_str(),
        "status",
        e->rrd_len / (e->interval ? e->interval : 60),
        0,
        e->interval);
    }
    std::ostringstream oss2;
    if (e->state == 0)
      oss2 << 100;
    else if (e->state == 1)
      oss2 << 75;
    else if (e->state == 2)
      oss2 << 0;
    try {
      _backend->update(e->ctime, oss2.str().c_str());
    }
    catch (exceptions::update const& b) {
      logging::error << logging::MEDIUM << b.what() << " (ignored)";
    }
  }
  return ;
}
