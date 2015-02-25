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

#include <sstream>
#include <QHostAddress>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/influxdb/influxdb9.hh"

using namespace com::centreon::broker::influxdb;

static const char* query_header =
    "\"points\": [";
static const char* query_footer = "]}";

/**
 *  Constructor.
 */
influxdb9::influxdb9(
            std::string const& user,
            std::string const& passwd,
            std::string const& addr,
            unsigned short port,
            std::string const& db)
  : _host(addr),
    _port(port) {
  _connect_socket();
  _socket->close();

  std::string base_url;
  base_url
    .append("/db/").append(db)
    .append("/series?u=").append(user)
    .append("&p=").append(passwd)
    .append("&time_precision=s");
  _post_header.append("POST ").append(base_url).append(" HTTP/1.0\n");
  _db_header.append("{\"database\":\"").append(db).append("\",").append(query_header);
}

/**
 *  Copy constructor.
 *
 *  @param[in] f Object to copy.
 */
influxdb9::influxdb9(influxdb9 const& f) {
  influxdb::operator=(f);
}

/**
 *  Destructor.
 */
influxdb9::~influxdb9() {}

/**
 *  Assignment operator.
 *
 *  @param[in] f Object to copy.
 *
 *  @return This object.
 */
influxdb9& influxdb9::operator=(influxdb9 const& f) {
  if (this != &f) {
    _query = f._query;
  }
  return (*this);
}

/**
 *  Clear the query.
 */
void influxdb9::clear() {
  _query.clear();
}

/**
 *  Write a metric to the query.
 *
 *  @param[in] m  The metric to write.
 */
void influxdb9::write(storage::metric const& m) {
  std::stringstream s;

  s << "{ \"name\": \"" << m.name.toStdString() << "\","
    << "\"tags\": {" << "\"metric_id = \"" << m.metric_id << "},"
    << "\"timestamp\":" << m.ctime
    << "\"fields\": {" << "\"value\":" << m.value << "} },";
  _query.append(s.str());
}

/**
 *  Commit a query.
 */
void influxdb9::commit() {
  if (_query.empty())
    return ;

  std::stringstream content_length;
  size_t length = _query.size() + _db_header.size() + ::strlen(query_footer);
  content_length << "Content-Length: " << length << "\n";

  std::string final_query;
  final_query.reserve(length + _post_header.size() + content_length.str().size() + 1);
  final_query
    .append(_post_header).append(content_length.str()).append("\n")
    .append(_db_header).append(_query).append(query_footer);

  _connect_socket();

  if (_socket->write(final_query.c_str(), final_query.size())
        != final_query.size())
    throw exceptions::msg()
      << "influxdb: couldn't commit data to influxdb with address '"
      << _socket->peerAddress().toString()
      << "' and port '" << _socket->peerPort() << "': "
      << _socket->errorString();

  _socket->waitForBytesWritten();

  _query.clear();
}

/**
 *  Connect the socket to the endpoint.
 */
void influxdb9::_connect_socket() {
  _socket.reset(new QTcpSocket);
  _socket->connectToHost(QString::fromStdString(_host), _port);
  if (!_socket->waitForConnected())
    throw exceptions::msg()
      << "influxdb: couldn't connect to influxdb with address '"
      << _host << "' and port '" << _port << "': " << _socket->errorString();
}
