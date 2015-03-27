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

#include <algorithm>
#include <iterator>
#include <sstream>
#include <vector>
#include <QHostAddress>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/influxdb/influxdb9.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/influxdb/json_printer.hh"

using namespace com::centreon::broker::influxdb;

static const char* query_footer = "]}";

/**
 *  Constructor.
 */
influxdb9::influxdb9(
            std::string const& user,
            std::string const& passwd,
            std::string const& addr,
            unsigned short port,
            std::string const& db,
            std::string const& status_ts,
            std::vector<column> const& status_cols,
            std::string const& metric_ts,
            std::vector<column> const& metric_cols)
  : _host(addr),
    _port(port) {
  logging::debug(logging::medium)
    << "influxdb: connecting using 0.9 version protocol";

  // Try to connect to the server.
  _connect_socket();
  _socket->close();

  _create_queries(user, passwd, db, status_ts, status_cols, metric_ts, metric_cols);
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
  _query.append(_metric_query.generate_metric(m));
}

/**
 *  Write a status to the query.
 *
 *  @param[in] s  The status to write.
 */
void influxdb9::write(storage::status const& s) {
  _query.append(_status_query.generate_status(s));
}

/**
 *  Commit a query.
 */
void influxdb9::commit() {
  if (_query.empty())
    return ;

  // Remove trailing coma.
  _query[_query.size() - 1] = ' ';

  std::stringstream content_length;
  size_t length = _query.size() + _db_header.size() + ::strlen(query_footer);
  content_length << "Content-Length: " << length << "\n";

  std::string final_query;
  final_query.reserve(length + _post_header.size() + content_length.str().size() + 1);
  final_query
    .append(_post_header).append(content_length.str()).append("\n")
    .append(_db_header).append(_query).append(query_footer);

  _connect_socket();

  // Send the data to the server.
  if (_socket->write(final_query.c_str(), final_query.size())
        != final_query.size())
    throw (exceptions::msg()
      << "influxdb: couldn't commit data to influxdb with address '"
      << _socket->peerAddress().toString()
      << "' and port '" << _socket->peerPort() << "': "
      << _socket->errorString());

  while (_socket->bytesToWrite() != 0) {
    if (_socket->waitForBytesWritten() == false)
      throw (exceptions::msg()
        << "influxdb: couldn't send data to influxdb with address '"
        << _socket->peerAddress().toString()
        << "' and port '" << _socket->peerPort() << "': "
        << _socket->errorString());
  }

  // Receive the server answer.
  QString answer;
  while (true) {
    if (_socket->waitForReadyRead() == false)
      throw (exceptions::msg()
        << "influxdb: couldn't receive influxdb answer with address '"
        << _socket->peerAddress().toString()
        << "' and port '" << _socket->peerPort() << "': "
        << _socket->errorString());

    answer.append(_socket->readAll());

    if (_check_answer_string(answer.toStdString()) == true)
      break;
  }
  _socket->close();
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

/**
 *  Check the server's answer.
 *
 *  @param[in] ans  The server's answer.
 *
 *  @return         True of the answer was complete, false otherwise.
 */
bool influxdb9::_check_answer_string(std::string const& ans) {
  size_t first_line = ans.find_first_of('\n');
  if (first_line == std::string::npos)
    return (false);
  std::string first_line_str = ans.substr(0, first_line);

  logging::debug(logging::medium)
    << "influxdb: received an answer from "
    << _socket->peerAddress().toString()
    << "' and port '" << _socket->peerPort() << "': '" << ans << "'";

  // Split the first line using the power of std.
  std::istringstream iss(first_line_str);
  std::vector<std::string> split;
  std::copy(
         std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         std::back_inserter(split));

  if (split.size() != 3)
    throw (exceptions::msg()
      << "influxdb: unrecognizable HTTP header for '"
      << _socket->peerAddress().toString()
      << "' and port '" << _socket->peerPort() << "': got '"
      << first_line_str << "'");

  if (split[0] == "HTTP/1.0" && split[1] == "200" && split[2] == "OK")
    return (true);
  else
    throw (exceptions::msg()
      << "influxdb: got an error from '"
      << _socket->peerAddress().toString()
      << "' and port '" << _socket->peerPort() << "': '"
      << ans << "'");
}

/**
 *  Create the queries for influxdb.
 *
 *  @param[in] status_ts    Name of the timeseries status.
 *  @param[in] status_cols  Column for the statuses.
 *  @param[in] metric_ts    Name of the timeseries metric.
 *  @param[in] metric_cols  Column for the metrics.
 */
void influxdb9::_create_queries(
                  std::string const& user,
                  std::string const& passwd,
                  std::string const& db,
                  std::string const& status_ts,
                  std::vector<column> const& status_cols,
                  std::string const& metric_ts,
                  std::vector<column> const& metric_cols) {
  // Create POST HTTP header.
  std::string base_url;
  base_url
    .append("/write?u=").append(user)
    .append("&p=").append(passwd);
  _post_header.append("POST ").append(base_url).append(" HTTP/1.0\n");

  // Create influxdb header.
  json_printer p;
  p.open_object().add_string("database", db).open_array("points");
  _db_header.append(p.get_data());
  p.clear();

  // Create status query.
  p.open_object()
     .add_string("name", status_ts)
     .open_object("tags");
  for (std::vector<column>::const_iterator
         it(status_cols.begin()),
         end(status_cols.end());
       it != end; ++it)
    if (it->is_flag()) {
      if (it->get_type() == column::number)
        p.add_value(it->get_name(), it->get_value());
      else if (it->get_type() == column::string)
        p.add_string(it->get_name(), it->get_value());
    }
   p.close_object()
     .add_value("timestamp", "$TIME$")
     .open_object("fields");
   for (std::vector<column>::const_iterator
          it(status_cols.begin()),
          end(status_cols.end());
        it != end; ++it)
     if (!it->is_flag()) {
       if (it->get_type() == column::number)
         p.add_value(it->get_name(), it->get_value());
       else if (it->get_type() == column::string)
         p.add_string(it->get_name(), it->get_value());
     }
   p.close_object().close_object();
   _status_query = query(p.get_data(), query::status);
   p.clear();

   // Create metric query.
   p.open_object()
      .add_string("name", metric_ts)
      .open_object("tags");
   for (std::vector<column>::const_iterator
          it(metric_cols.begin()),
          end(metric_cols.end());
        it != end; ++it)
     if (it->is_flag()) {
       if (it->get_type() == column::number)
         p.add_value(it->get_name(), it->get_value());
       else if (it->get_type() == column::string)
         p.add_string(it->get_name(), it->get_value());
     }
    p.close_object()
      .add_value("timestamp", "$TIME$")
      .open_object("fields");
    for (std::vector<column>::const_iterator
           it(metric_cols.begin()),
           end(metric_cols.end());
         it != end; ++it)
      if (!it->is_flag()) {
        if (it->get_type() == column::number)
          p.add_value(it->get_name(), it->get_value());
        else if (it->get_type() == column::string)
          p.add_string(it->get_name(), it->get_value());
      }
    p.close_object().close_object();
    _metric_query = query(p.get_data(), query::metric);
}
