/*
** Copyright 2020 Centreon
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

#ifndef CCB_RRD_CACHED_HH
#define CCB_RRD_CACHED_HH

#include <fmt/format.h>

#include <asio.hpp>

#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/rrd/exceptions/open.hh"
#include "com/centreon/broker/rrd/exceptions/update.hh"
#include "com/centreon/broker/rrd/lib.hh"

using namespace com::centreon;
using namespace com::centreon::exceptions;

CCB_BEGIN()

namespace rrd {

template <typename T>
class cached : public backend {
  asio::io_context _io_context;
  bool _batch;
  lib _lib;
  T _socket;
  std::string _filename;

 public:
  cached(const std::string& tmpl_path, uint32_t cache_size)
      : _batch{false}, _lib{tmpl_path, cache_size}, _socket{_io_context} {}
  /**
   * @brief Open an RRD file which already exists.
   *
   * @param filename Path to the RRD file.
   */
  void open(const std::string& filename) {
    // Close previous file.
    this->close();

    // Check that the file exists.
    if (access(filename.c_str(), F_OK))
      throw broker::rrd::exceptions::open(
          "RRD: file '{}' does not exist", filename);

    // Remember information for further operations.
    _filename = filename;
  }

  /**
   *  Open a RRD file and create it if it does not exists.
   *
   *  @param[in] filename   Path to the RRD file.
   *  @param[in] length     Duration in seconds that the RRD file should
   *                        retain.
   *  @param[in] from       Timestamp of the first record.
   *  @param[in] step       Time interval between each record.
   *  @param[in] value_type Type of the metric.
   */
  void open(std::string const& filename,
            uint32_t length,
            time_t from,
            uint32_t step,
            short value_type = 0) {
    // Close previous file.
    this->close();

    // Remember informations for further operations.
    _filename = filename;

    /* We are unfortunately forced to use librrd to create RRD file as
    ** rrdcached does not support RRD file creation.
    */
    _lib.open(filename, length, from, step, value_type);
  }

  /**
   * @brief Close the current RRD file.
   */
  void close() {
    _filename.clear();
    _batch = false;
  }

  /**
   * @brief Clera the template cache.
   */
  void clean() { _lib.clean(); }

  /**
   *  Remove the RRD file.
   *
   *  @param[in] filename Path to the RRD file.
   */
  void remove(std::string const& filename) {
    // Build rrdcached command.
    std::string cmd(fmt::format("FORGET {}\n", filename));

    try {
      _send_to_cached(cmd);
    } catch (msg_fmt const& e) {
      logging::error(logging::medium) << e.what();
    }

    if (::remove(filename.c_str()))
      logging::error(logging::high) << "RRD: could not remove file '"
                                    << filename << "': " << strerror(errno);
  }

  /**
   *  Send data to rrdcached.
   *
   *  @param[in] command Command to send.
   */
  void _send_to_cached(std::string const& command) {
    std::error_code err;

    asio::write(_socket, asio::buffer(command), asio::transfer_all(), err);

    if (err)
      throw msg_fmt("RRD: error while sending "
                            "command to rrdcached: {}",
                            err.message());

    // Read response.
    if (!_batch) {
      asio::streambuf stream;
      std::string line;

      asio::read_until(_socket, stream, '\n', err);

      if (err)
        throw msg_fmt("RRD: error while getting "
                      "response from rrdcached: {}",
                      err.message());

      std::istream is(&stream);
      std::getline(is, line);

      int lines;
      try {
        lines = std::stoi(line);
      } catch (...) {
        lines = -1;
      }

      if (lines < 0)
        throw msg_fmt(
            "RRD: rrdcached query failed on file '{}' ({}"
            "): {}", _filename, command, line);
      while (lines > 0) {
        asio::read_until(_socket, stream, '\n', err);
        if (err)
          throw msg_fmt(
              "RRD: error while getting "
              "response from rrdcached for file '{}" 
              "': {}",  _filename, err.message());

        std::istream is(&stream);
        std::getline(is, line);
        --lines;
      }
    }
  }

  /**
   *  Initiates the bulk load of multiple commands.
   */
  void begin() {
    // Send BATCH command to rrdcached.
    _batch = true;
    _send_to_cached("BATCH\n");
  }

  /**
   *  Connect to a local socket.
   *
   *  @param[in] name Socket name.
   */
  void connect_local(std::string const& name) {
    // Create socket object.
    asio::local::stream_protocol::endpoint ep(name);

    try {
      _socket.connect(ep);
    } catch (std::system_error const& se) {
      throw msg_fmt("RRD: could not connect to local socket '{}: {}",
                    name, se.what());
    }
  }

  /**
   *  Connect to a remote server.
   *
   *  @param[in] address Server address.
   *  @param[in] port    Port to connect to.
   */
  void connect_remote(std::string const& address, unsigned short port) {
    asio::ip::tcp::resolver resolver{_io_context};
    asio::ip::tcp::resolver::query query{address, std::to_string(port)};

    try {
      asio::ip::tcp::resolver::iterator it{resolver.resolve(query)};
      asio::ip::tcp::resolver::iterator end;

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
        throw msg_fmt("RRD: could not connect to remote server '{}" 
                                  ":{} : {}", address, port, err.message());
      }

      asio::socket_base::keep_alive option{true};
      _socket.set_option(option);
    } catch (std::system_error const& se) {
        throw msg_fmt("RRD: could not resolve remove server '{}" 
                                  ":{} : {}", address, port, se.what());
    }
  }

  /**
   *  Commit current transaction.
   */
  void commit() {
    if (_batch) {
      // Send a . on the line to indicate that transaction is over.
      _batch = false;
      _send_to_cached(".\n");
    }
  }

  /**
   *  Update the RRD file with new value.
   *
   *  @param[in] t     Timestamp of value.
   *  @param[in] value Associated value.
   */
  void update(time_t t, std::string const& value) {
    // Build rrdcached command.
    std::string cmd(fmt::format("UPDATE {} {}:{}\n", _filename, t, value));

    // Send command.
    logging::debug(logging::high)
        << "RRD: updating file '" << _filename << "' (" << cmd << ")";
    try {
      _send_to_cached(cmd);
    } catch (msg_fmt const& e) {
      if (!strstr(e.what(), "illegal attempt to update using time"))
        throw exceptions::update(e.what());
      else
        logging::error(logging::low) << "RRD: ignored update error in file '"
                                     << _filename << "': " << e.what() + 5;
    }
  }
};
}  // namespace rrd

CCB_END()

#endif /* !CCB_RRD_CACHED_HH */
