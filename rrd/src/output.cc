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
#include "events/perfdata.hh"
#include "exceptions/basic.hh"
#include "logging/logging.hh"
#include "rrd/lib.hh"
#include "rrd/output.hh"

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
 *  @param[in] rrd_path Path in which RRD files should be written.
 */
output::output(QString const& rrd_path)
  : _backend(new lib), _rrd_path(rrd_path) {}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
output::output(output const& o)
  : io::stream(o), _backend(new lib), _rrd_path(o._rrd_path) {}

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
  _rrd_path = o._rrd_path;
  return (*this);
}

/**
 *  Read data.
 *
 *  @return Does not return, throw an exception.
 */
QSharedPointer<io::data> output::read() {
  throw (exceptions::basic() << "attempt to read data from an RRD output endpoint");
  return (QSharedPointer<io::data>());
}

/**
 *  Write an event.
 *
 *  @param[in] d Data to write.
 */
void output::write(QSharedPointer<io::data> d) {
  if (d->type() == events::event::PERFDATA) {
    logging::debug << logging::HIGH << "RRD: new perfdata event";
    QSharedPointer<events::perfdata> e(d.staticCast<events::perfdata>());
    std::ostringstream oss1;
    oss1 << _rrd_path.toStdString() << "/" << e->metric_id << ".rrd";
    try {
      _backend->open(oss1.str().c_str(), e->name);
    }
    catch (exceptions::basic const& b) {
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
    catch (exceptions::basic const& b) {
      logging::error << logging::MEDIUM << b.what() << " (ignored)";
    }
  }
  return ;
}
