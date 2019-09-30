/*
** Copyright 2011-2017 Centreon
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

#include "com/centreon/broker/influxdb/influxdb12.hh"
#include <iterator>
#include <vector>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"

using namespace asio;
using namespace com::centreon::broker::influxdb;

static const char* query_footer = "\n";

/**
 *  Constructor.
 */
influxdb12::influxdb12(std::string const& user,
                       std::string const& passwd,
                       std::string const& addr,
                       unsigned short port,
                       std::string const& db,
                       std::string const& status_ts,
                       std::vector<column> const& status_cols,
                       std::string const& metric_ts,
                       std::vector<column> const& metric_cols,
                       macro_cache const& cache)
    : _host(addr), _port(port), _cache(cache) {
  // Try to connect to the server.
  logging::debug(logging::medium)
      << "influxdb: connecting using 1.2 Line Protocol";
  _connect_socket();
  _socket->close();
  _create_queries(user, passwd, db, status_ts, status_cols, metric_ts,
                  metric_cols);
}

/**
 *  Destructor.
 */
influxdb12::~influxdb12() {}

/**
 *  Clear the query.
 */
void influxdb12::clear() {
  _query.clear();
  return;
}

/**
 *  Write a metric to the query.
 *
 *  @param[in] m  The metric to write.
 */
void influxdb12::write(storage::metric const& m) {
  _query.append(_metric_query.generate_metric(m));
  return;
}

/**
 *  Write a status to the query.
 *
 *  @param[in] s  The status to write.
 */
void influxdb12::write(storage::status const& s) {
  _query.append(_status_query.generate_status(s));
  return;
}

/**
 *  Commit a query.
 */
void influxdb12::commit() {
  if (_query.empty())
    return;

  std::stringstream content_length;
  size_t length = _query.size() + ::strlen(query_footer);
  content_length << "Content-Length: " << length << "\n";

  std::string final_query;
  final_query.reserve(length + _post_header.size() +
                      content_length.str().size() + 1);
  final_query.append(_post_header)
      .append(content_length.str())
      .append("\n")
      .append(_query)
      .append(query_footer);

  _connect_socket();

  std::error_code err;

  asio::write(*_socket, buffer(final_query), asio::transfer_all(), err);
  if (err)
    throw exceptions::msg()
        << "influxdb: couldn't commit data to InfluxDB with address '"
        << _socket->remote_endpoint().address().to_string() << "' and port '"
        << _socket->remote_endpoint().port() << "': " << err.message();
  // Receive the server answer.

  std::string answer;
  std::size_t total_read{0}, read_size{2048};

  do {
    answer.resize(read_size);

    total_read += _socket->read_some(asio::buffer(&answer[total_read], read_size - total_read), err);
    if (total_read == read_size)
      total_read += 2048;

    answer.resize(total_read);

    if (err)
      throw exceptions::msg()
          << "influxdb: couldn't receive InfluxDB answer with address '"
          << _socket->remote_endpoint().address().to_string() << "' and port '"
          << _socket->remote_endpoint().port() << "': " << err.message();


  } while (!_check_answer_string(answer));
  _socket->close();
  _query.clear();
}

/**
 *  Connect the socket to the endpoint.
 */
void influxdb12::_connect_socket() {
  _socket.reset(new ip::tcp::socket{_io_context});
  ip::tcp::resolver resolver{_io_context};
  ip::tcp::resolver::query query{_host, std::to_string(_port)};

  try {
    ip::tcp::resolver::iterator it{resolver.resolve(query)};
    ip::tcp::resolver::iterator end;

    std::error_code err{std::make_error_code(std::errc::host_unreachable)};

    // it can resolve to multiple addresses like ipv4 and ipv6
    // we need to try all to find the first available socket
    while (err && it != end) {
      _socket->connect(*it, err);

      if (err)
        _socket->close();

      ++it;
    }

    if (err) {
      throw exceptions::msg()
          << "influxdb: couldn't connect to InfluxDB with address '" << _host
          << "' and port '" << _port << "': " << err.message();
    }
  } catch (std::system_error const& se) {
    throw exceptions::msg()
        << "influxdb: couldn't connect to InfluxDB with address '" << _host
        << "' and port '" << _port << "': " << se.what();
  }
}

/**
 *  Check the server's answer.
 *
 *  @param[in] ans  The server's answer.
 *
 *  @return         True of the answer was complete, false otherwise.
 */
bool influxdb12::_check_answer_string(std::string const& ans) {
  size_t first_line = ans.find_first_of('\n');
  if (first_line == std::string::npos)
    return (false);
  std::string first_line_str = ans.substr(0, first_line);

  logging::debug(logging::medium)
      << "influxdb: received an answer from "
      << _socket->remote_endpoint().address().to_string() << "' and port '"
      << _socket->remote_endpoint().port() << "': '" << ans << "'";

  // Split the first line using the power of std.
  std::istringstream iss(first_line_str);
  std::vector<std::string> split;
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(), std::back_inserter(split));

  if (split.size() < 3)
    throw(exceptions::msg()
          << "influxdb: unrecognizable HTTP header for '"
          << _socket->remote_endpoint().address().to_string() << "' and port '"
          << _socket->remote_endpoint().port() << "': got '" << first_line_str
          << "'");

  if ((split[0] == "HTTP/1.0") && (split[1] == "204") && (split[2] == "No") &&
      (split[3] == "Content"))
    return (true);
  else
    throw(exceptions::msg()
          << "influxdb: got an error from '"
          << _socket->remote_endpoint().address().to_string() << "' and port '"
          << _socket->remote_endpoint().port() << "': '" << ans << "'");
}

/**
 *  Create the queries for influxdb.
 *
 *  @param[in] status_ts    Name of the timeseries status.
 *  @param[in] status_cols  Column for the statuses.
 *  @param[in] metric_ts    Name of the timeseries metric.
 *  @param[in] metric_cols  Column for the metrics.
 */
void influxdb12::_create_queries(std::string const& user,
                                 std::string const& passwd,
                                 std::string const& db,
                                 std::string const& status_ts,
                                 std::vector<column> const& status_cols,
                                 std::string const& metric_ts,
                                 std::vector<column> const& metric_cols) {
  // Create POST HTTP header.
  std::string base_url;
  base_url.append("/write?u=")
      .append(user)
      .append("&p=")
      .append(passwd)
      .append("&db=")
      .append(db)
      .append("&precision=s");
  _post_header.append("POST ").append(base_url).append(" HTTP/1.0\n");

  // Create protocol objects.
  _status_query = line_protocol_query(status_ts, status_cols,
                                      line_protocol_query::status, _cache);
  _metric_query = line_protocol_query(metric_ts, metric_cols,
                                      line_protocol_query::metric, _cache);
  return;
}
