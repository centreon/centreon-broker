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

#include <algorithm>
#include <iterator>
#include <sstream>
#include <vector>
#include <QHostAddress>
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/influxdb/influxdb10.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/influxdb/json_printer.hh"

using namespace com::centreon::broker::influxdb;

static const char* query_footer = "\n";

/**
 *  Constructor.
 */
influxdb10::influxdb10(
            std::string const& user,
            std::string const& passwd,
            std::string const& addr,
            unsigned short port,
            std::string const& db,
            std::string const& status_ts,
            std::vector<column> const& status_cols,
            std::string const& metric_ts,
            std::vector<column> const& metric_cols,
            macro_cache const& cache)
  : _host(addr),
    _port(port),
    _cache(cache){
  logging::debug(logging::medium)
    << "influxdb: connecting using 1.0 line protocol";

  // Try to connect to the server.
  _connect_socket();
  _socket->close();

  _create_queries(user, passwd, db, status_ts, status_cols, metric_ts, metric_cols);
}

/**
 *  Destructor.
 */
influxdb10::~influxdb10() {}

/**
 *  Clear the query.
 */
void influxdb10::clear() {
  _query.clear();
}

/**
 *  Write a metric to the query.
 *
 *  @param[in] m  The metric to write.
 */
void influxdb10::write(storage::metric const& m) {
  _query.append(_metric_query.generate_metric(m));
}

/**
 *  Write a status to the query.
 *
 *  @param[in] s  The status to write.
 */
void influxdb10::write(storage::status const& s) {
  _query.append(_status_query.generate_status(s));
}

/**
 *  Commit a query.
 */
void influxdb10::commit() {
  if (_query.empty())
    return ;

  std::stringstream content_length;
  size_t length = _query.size() + ::strlen(query_footer);
  content_length << "Content-Length: " << length << "\n";

  std::string final_query;
  final_query.reserve(length + _post_header.size() + content_length.str().size() + 1);
  final_query
    .append(_post_header).append(content_length.str()).append("\n")
    .append(_query).append(query_footer);

  _connect_socket();

  // Send the data to the server.
  if (_socket->write(final_query.c_str(), final_query.size())
      != static_cast<int>(final_query.size()))
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
void influxdb10::_connect_socket() {
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
bool influxdb10::_check_answer_string(std::string const& ans) {
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

  if (split.size() < 3)
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
 *  Escape string for influxdb10
 *
 *  @param str  The string.
 *  @return     The string, escaped.
 */
static std::string escape(std::string const& str) {
  std::string ret(str);
  ::com::centreon::broker::misc::string::replace(ret, " ", "\\ ");
  ::com::centreon::broker::misc::string::replace(ret, ",", "\\,");
  return (ret);
}

/**
 *  Create the queries for influxdb.
 *
 *  @param[in] status_ts    Name of the timeseries status.
 *  @param[in] status_cols  Column for the statuses.
 *  @param[in] metric_ts    Name of the timeseries metric.
 *  @param[in] metric_cols  Column for the metrics.
 */
void influxdb10::_create_queries(
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
    .append("&p=").append(passwd)
    .append("&db=").append(db);
  _post_header.append("POST ").append(base_url).append(" HTTP/1.0\n");

  // Create status query.
  std::string query_str;
  query_str
    .append(escape(status_ts));
  for (std::vector<column>::const_iterator
         it(status_cols.begin()),
         end(status_cols.end());
       it != end; ++it)
    if (it->is_flag()) {
      query_str.append(",");
      if (it->get_type() == column::number)
        query_str
          .append(escape(it->get_name()))
          .append("=")
          .append(escape(it->get_value()));
      else if (it->get_type() == column::string)
        query_str
          .append(escape(it->get_name()))
          .append("\"")
          .append("=")
          .append(escape(it->get_value()))
          .append("\"");
    }
  query_str.append(" ");
  bool first = true;
  for (std::vector<column>::const_iterator
         it(status_cols.begin()),
         end(status_cols.end());
       it != end; ++it)
    if (!it->is_flag()) {
      if (first)
        first = false;
      else
        query_str.append(",");
      if (it->get_type() == column::number)
        query_str
          .append(escape(it->get_name()))
          .append("=")
          .append(escape(it->get_value()));
      else if (it->get_type() == column::string)
        query_str
          .append(escape(it->get_name()))
          .append("\"")
          .append("=")
          .append(escape(it->get_value()))
          .append("\"");
    }
   query_str.append(" ").append("$TIME$");
   _status_query = query(query_str, query::status, _cache, true);

   // Create metric query.
   query_str.clear();
   query_str
     .append(escape(metric_ts));
   for (std::vector<column>::const_iterator
          it(metric_cols.begin()),
          end(metric_cols.end());
        it != end; ++it)
     if (it->is_flag()) {
       query_str.append(",");
       if (it->get_type() == column::number)
         query_str
           .append(escape(it->get_name()))
           .append("=")
           .append(escape(it->get_value()));
       else if (it->get_type() == column::string)
         query_str
           .append(escape(it->get_name()))
           .append("\"")
           .append("=")
           .append(escape(it->get_value()))
           .append("\"");
     }
   query_str.append(" ");
   first = true;
   for (std::vector<column>::const_iterator
          it(metric_cols.begin()),
          end(metric_cols.end());
        it != end; ++it)
     if (!it->is_flag()) {
       if (first)
         first = false;
       else
         query_str.append(",");
       if (it->get_type() == column::number)
         query_str
           .append(escape(it->get_name()))
           .append("=")
           .append(escape(it->get_value()));
       else if (it->get_type() == column::string)
         query_str
           .append(escape(it->get_name()))
           .append("\"")
           .append("=")
           .append(escape(it->get_value()))
           .append("\"");
     }
    query_str.append(" ").append("$TIME$");
    _metric_query = query(query_str, query::metric, _cache, true);
}
