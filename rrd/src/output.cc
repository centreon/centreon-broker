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

#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/rrd/lib.hh"
#include "com/centreon/broker/rrd/metric.hh"
#include "com/centreon/broker/rrd/output.hh"
#include "com/centreon/broker/rrd/status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] metrics_path Path in which metrics RRD files should be
 *                          written.
 *  @param[in] status_path  Path in which status RRD files should be
 *                          written.
 */
output::output(QString const& metrics_path, QString const& status_path)
  : _backend(new lib),
    _metrics_path(metrics_path),
    _status_path(status_path) {}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
output::output(output const& o)
  : io::stream(o),
    _backend(new lib),
    _metrics_path(o._metrics_path),
    _status_path(o._status_path) {}

/**
 *  Destructor.
 */
output::~output() {}

/**
 *  Assignment operator.
 *
 *  @param[in] o Object to copy.
 *
 *  @return This object.
 */
output& output::operator=(output const& o) {
  io::stream::operator=(o);
  _metrics_path = o._metrics_path;
  _status_path = o._status_path;
  return (*this);
}

/**
 *  Read data.
 *
 *  @return Does not return, throw an exception.
 */
QSharedPointer<io::data> output::read() {
  throw (exceptions::msg()
           << "RRD: attempt to read data from an output endpoint");
  return (QSharedPointer<io::data>());
}

/**
 *  Write an event.
 *
 *  @param[in] d Data to write.
 */
void output::write(QSharedPointer<io::data> d) {
  if (d->type() == "com::centreon::broker::rrd::metric") {
    // Debug message.
    logging::debug << logging::MEDIUM << "RRD: new metric data";
    QSharedPointer<rrd::metric> e(d.staticCast<rrd::metric>());

    // Write metrics RRD.
    std::ostringstream oss1;
    oss1 << _metrics_path.toStdString() << "/" << e->metric_id << ".rrd";
    try {
      _backend->open(oss1.str().c_str(), e->name);
    }
    catch (exceptions::msg const& b) { // XXX : should be specialized
      _backend->open(oss1.str().c_str(),
        e->name,
        e->rrd_len / e->interval,
        0,
        e->interval);
    }
    std::ostringstream oss2;
    oss2 << e->value;
    try {
      _backend->update(e->ctime, oss2.str().c_str());
    }
    catch (exceptions::msg const& b) { // XXX : should be specialized
      logging::error << logging::MEDIUM << b.what() << " (ignored)";
    }
  }
  else if (d->type() == "com::centreon::broker::rrd::status") {
    // Debug message.
    logging::debug << logging::MEDIUM << "RRD: new status data";
    QSharedPointer<rrd::status> e(d.staticCast<rrd::status>());

    // Write status RRD.
    std::ostringstream oss1;
    oss1 << _status_path.toStdString() << "/" << e->index_id << ".rrd";
    try {
      _backend->open(oss1.str().c_str(), "status");
    }
    catch (exceptions::msg const& b) { // XXX : should be specialized
      _backend->open(oss1.str().c_str(),
        "status",
        e->rrd_len / e->interval,
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
    catch (exceptions::msg const& b) { // XXX : should be specialized
      logging::error << logging::MEDIUM << b.what() << " (ignored)";
    }
  }
  return ;
}
