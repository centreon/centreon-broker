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
connector::connector() : io::endpoint(false), _cached_port(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
connector::connector(connector const& c)
  : io::endpoint(c),
    _cached_local(c._cached_local),
    _cached_port(c._cached_port),
    _metrics_path(c._metrics_path),
    _status_path(c._status_path) {}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& c) {
  io::endpoint::operator=(c);
  _cached_local = c._cached_local;
  _cached_port = c._cached_port;
  _metrics_path = c._metrics_path;
  _status_path = c._status_path;
  return (*this);
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
QSharedPointer<io::stream> connector::open() {
  QSharedPointer<io::stream> retval;
  if (!_cached_local.isEmpty())
    retval = QSharedPointer<io::stream>(new output(_metrics_path,
                                                   _status_path,
                                                   _cached_local));
  else if (_cached_port)
    retval = QSharedPointer<io::stream>(new output(_metrics_path,
                                                   _status_path,
                                                   _cached_port));
  else
    retval = QSharedPointer<io::stream>(new output(_metrics_path,
                                                   _status_path));
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
void connector::set_cached_net(unsigned short port) {
  _cached_port = port;
  return ;
}

/**
 *  Set the RRD metrics path.
 *
 *  @param[in] metrics_path Where metrics RRD files will be written.
 */
void connector::set_metrics_path(QString const& metrics_path) {
  _metrics_path = metrics_path;
  return ;
}

/**
 *  Set the RRD status path.
 *
 *  @param[in] status_path Where status RRD files will be written.
 */
void connector::set_status_path(QString const& status_path) {
  _status_path = status_path;
  return ;
}
