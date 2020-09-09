/*
** Copyright 2011-2015,2017 Centreon
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

#include "com/centreon/broker/graphite/stream.hh"
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/metric.hh"

using namespace asio;
using namespace com::centreon::broker;
using namespace com::centreon::broker::graphite;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Constructor.
 *
 */
stream::stream(std::string const& metric_naming,
               std::string const& status_naming,
               std::string const& escape_string,
               std::string const& db_user,
               std::string const& db_password,
               std::string const& db_host,
               unsigned short db_port,
               uint32_t queries_per_transaction,
               std::shared_ptr<persistent_cache> const& cache)
    : _metric_naming{metric_naming},
      _status_naming{status_naming},
      _db_user{db_user},
      _db_password{db_password},
      _db_host{db_host},
      _db_port{db_port},
      _queries_per_transaction{
          (queries_per_transaction == 0) ? 1 : queries_per_transaction},
      _pending_queries{0},
      _actual_query{0},
      _commit_flag{false},
      _cache{cache},
      _metric_query{_metric_naming, escape_string, query::metric, _cache},
      _status_query{_status_naming, escape_string, query::status, _cache},
      _socket{_io_context} {
  // Create the basic HTTP authentification header.
  if (!_db_user.empty() && !_db_password.empty()) {
    std::string auth{_db_user};
    auth.append(":").append(_db_password);

    _auth_query.append("Authorization: Basic ")
        .append(misc::string::base64_encode(auth))
        .append("\n");
    _query.append(_auth_query);
  }

  ip::tcp::resolver resolver{_io_context};
  ip::tcp::resolver::query query{_db_host, std::to_string(_db_port)};

  try {
    ip::tcp::resolver::iterator it{resolver.resolve(query)};
    ip::tcp::resolver::iterator end;

    std::error_code err{std::make_error_code(std::errc::host_unreachable)};

    // it can resolve to multiple addresses like ipv4 and ipv6
    // we need to try all to find the first available socket
    while (err && it != end) {
      _socket.connect(*it, err);

      if (err)
        _socket.close();

      ++it;
    }

    if (err) {
      throw exceptions::msg()
          << "graphite: can't connect to graphite on host '" << _db_host
          << "', port '" << _db_port << "': " << err.message();
    }
  } catch (std::system_error const& se) {
    throw exceptions::msg()
        << "graphite: can't connect to graphite on host '" << _db_host
        << "', port '" << _db_port << "': " << se.what();
  }
}

/**
 *  Destructor.
 */
stream::~stream() {
}

/**
 *  Flush the stream.
 *
 *  @return Number of events acknowledged.
 */
int stream::flush() {
  logging::debug(logging::medium)
      << "graphite: commiting " << _actual_query << " queries";
  int ret(_pending_queries);
  if (_actual_query != 0)
    _commit();
  _actual_query = 0;
  _pending_queries = 0;
  _commit_flag = false;
  return (ret);
}

/**
 *  Read from the database.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw exceptions::shutdown() << "cannot read from Graphite database";
  return true;
}

/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(json11::Json::object& tree) const {
  std::lock_guard<std::mutex> lock(_statusm);
  if (!_status.empty())
    tree["status"] = _status;
}


/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(std::shared_ptr<io::data> const& data) {
  // Take this event into account.
  ++_pending_queries;
  if (!validate(data, "graphite"))
    return (0);

  // Give the event to the cache.
  _cache.write(data);

  // Process metric events.
  if (data->type() ==
      io::events::data_type<io::events::storage, storage::de_metric>::value) {
    if (_process_metric(*std::static_pointer_cast<storage::metric const>(data)))
      ++_actual_query;
  } else if (data->type() == io::events::data_type<io::events::storage,
                                                   storage::de_status>::value) {
    if (_process_status(*std::static_pointer_cast<storage::status const>(data)))
      ++_actual_query;
  }
  if (_actual_query >= _queries_per_transaction)
    _commit_flag = true;

  if (_commit_flag)
    return (flush());
  else
    return (0);
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Process a metric event.
 *
 *  @param[in] me  The event to process.
 */
bool stream::_process_metric(storage::metric const& me) {
  std::string to_append = _metric_query.generate_metric(me);
  _query.append(to_append);
  return (!to_append.empty());
}

/**
 *  Process a status event.
 *
 *  @param[in] st  The status event.
 */
bool stream::_process_status(storage::status const& st) {
  std::string to_append = _status_query.generate_status(st);
  _query.append(to_append);
  return (!to_append.empty());
}

/**
 *  Commit all the processed event to the database.
 */
void stream::_commit() {
  if (!_query.empty()) {
    std::error_code err;

    asio::write(_socket, buffer(_query), asio::transfer_all(), err);
    if (err)
      throw exceptions::msg()
          << "graphite: can't send data to graphite on host '" << _db_host
          << "', port '" << _db_port << "': " << err.message();

    _query.clear();
    _query.append(_auth_query);
  }
}
