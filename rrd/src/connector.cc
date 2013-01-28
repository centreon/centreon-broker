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

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/rrd/connector.hh"
#include "com/centreon/broker/rrd/output.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
connector::connector()
  : io::endpoint(false),
    _cached_port(0),
    _write_metrics(true),
    _write_status(true) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
connector::connector(connector const& right)
  : io::endpoint(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& right) {
  if (this != &right) {
    io::endpoint::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Clone the connector.
 *
 *  @return This object.
 */
io::endpoint* connector::clone() const {
  return (new connector(*this));
}

/**
 *  Close the connector.
 */
void connector::close() {
  return ;
}

/**
 *  Connect.
 *
 *  @return Stream object.
 */
misc::shared_ptr<io::stream> connector::open() {
  misc::shared_ptr<io::stream> retval;
  if (!_cached_local.isEmpty())
    retval = misc::shared_ptr<io::stream>(new output(
                                                _metrics_path,
                                                _status_path,
                                                _cached_local,
                                                _write_metrics,
                                                _write_status));
  else if (_cached_port)
    retval = misc::shared_ptr<io::stream>(new output(
                                                _metrics_path,
                                                _status_path,
                                                _cached_port,
                                                _write_metrics,
                                                _write_status));
  else
    retval = misc::shared_ptr<io::stream>(new output(
                                                _metrics_path,
                                                _status_path,
                                                _write_metrics,
                                                _write_status));
  return (retval);
}

/**
 *  Set the local socket path.
 *
 *  @param[in] local_socket Local socket path.
 */
void connector::set_cached_local(QString const& local_socket) {
  _cached_local = local_socket;
  return ;
}

/**
 *  Set the network connection port.
 *
 *  @param[in] port rrdcached port.
 */
void connector::set_cached_net(unsigned short port) throw () {
  _cached_port = port;
  return ;
}

/**
 *  Set the RRD metrics path.
 *
 *  @param[in] metrics_path Where metrics RRD files will be written.
 */
void connector::set_metrics_path(QString const& metrics_path) {
  _metrics_path = _real_path_of(metrics_path);
  return ;
}

/**
 *  Set the RRD status path.
 *
 *  @param[in] status_path Where status RRD files will be written.
 */
void connector::set_status_path(QString const& status_path) {
  _status_path = _real_path_of(status_path);
  return ;
}

/**
 *  Set whether or not metrics should be written.
 *
 *  @param[in] write_metrics true if metrics must be written.
 */
void connector::set_write_metrics(bool write_metrics) throw () {
  _write_metrics = write_metrics;
  return ;
}

/**
 *  Set whether or not status should be written.
 *
 *  @param[in] write_status true if status must be written.
 */
void connector::set_write_status(bool write_status) throw () {
  _write_status = write_status;
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
void connector::_internal_copy(connector const& right) {
  _cached_local = right._cached_local;
  _cached_port = right._cached_port;
  _metrics_path = right._metrics_path;
  _status_path = right._status_path;
  _write_metrics = right._write_metrics;
  _write_status = right._write_status;
  return ;
}

/**
 *  Get the real path (absolute, expanded) of a path.
 *
 *  @param[in] path Path to resolve.
 *
 *  @return Real path.
 */
QString connector::_real_path_of(QString const& path) {
  // Variables.
  QString retval;
  char* real_path(realpath(qPrintable(path), NULL));

  // Resolution success.
  if (real_path) {
    logging::info(logging::medium) << "RRD: path '" << path
      << "' resolved as '" << real_path << "'";
    try {
      retval = real_path;
    }
    catch (...) {
      free(real_path);
      throw ;
    }
    free(real_path);
  }
  // Resolution failure.
  else {
    char const* msg(strerror(errno));
    logging::error(logging::high) << "RRD: could not resolve path '"
      << path << "', using it as such: " << msg;
    retval = path;
  }

  // Last slash.
  int last_index(retval.size() - 1);
  if (retval[last_index] != '/')
    retval.append("/");

  return (retval);
}
