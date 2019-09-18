/*
** Copyright 2011-2013 Centreon
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

#include "com/centreon/broker/rrd/cached.hh"
#include <unistd.h>
#include <asio.hpp>
#include <cerrno>
#include <cstdlib>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/rrd/exceptions/open.hh"
#include "com/centreon/broker/rrd/exceptions/update.hh"
#include "com/centreon/broker/rrd/lib.hh"

using namespace asio;
using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

/**************************************
 *                                     *
 *            Public Methods           *
 *                                     *
 **************************************/

/**
 *  Constructor.
 *
 *  @param[in] tmpl_path  The template path.
 *  @param[in] cache_size The maximum number of cache element.
 */
cached::cached(std::string const& tmpl_path, unsigned int cache_size)
    : _batch(false), _lib(tmpl_path, cache_size) {}

/**
 *  Destructor.
 */
cached::~cached() {}

/**
 *  Initiates the bulk load of multiple commands.
 */
void cached::begin() {
  // Send BATCH command to rrdcached.
  _batch = true;
  std::string buffer{"BATCH\n"};
  if (_type == cached::tcp)
    _send_to_cached<std::shared_ptr<ip::tcp::socket>&>(buffer, _tcp_socket);
  else
    _send_to_cached<std::shared_ptr<local::stream_protocol::socket>&>(
        buffer, _local_socket);

  return;
}

/**
 *  Clear the tempalte cache.
 */
void cached::clean() {
  _lib.clean();
}

/**
 *  Close the current RRD file.
 */
void cached::close() {
  _filename.clear();
  _batch = false;
}

/**
 *  Commit current transaction.
 */
void cached::commit() {
  if (_batch) {
    // Send a . on the line to indicate that transaction is over.
    _batch = false;
    std::string buffer{".\n"};
    if (_type == cached::tcp)
      _send_to_cached<std::shared_ptr<ip::tcp::socket>&>(buffer, _tcp_socket);
    else
      _send_to_cached<std::shared_ptr<local::stream_protocol::socket>&>(
          buffer, _local_socket);
  }
}

/**
 *  Connect to a local socket.
 *
 *  @param[in] name Socket name.
 */
void cached::connect_local(std::string const& name) {
  // Create socket object.

  local::stream_protocol::endpoint ep("/tmp/foobar");
  local::stream_protocol::socket* ls(
      new local::stream_protocol::socket{_io_context});
  _local_socket.reset(ls);

  try {
    _local_socket->connect(ep);
  } catch (std::system_error const& se) {
    broker::exceptions::msg e;
    e << "RRD: could not connect to local socket '" << name << ": "
      << se.what();
    _local_socket.reset();
    throw(e);
  }

  return;
}

/**
 *  Connect to a remote server.
 *
 *  @param[in] address Server address.
 *  @param[in] port    Port to connect to.
 */
void cached::connect_remote(std::string const& address, unsigned short port) {
  // Create socket object.
  asio::ip::tcp::socket* ts{new asio::ip::tcp::socket{_io_context}};
  _tcp_socket.reset(ts);

  asio::ip::tcp::resolver resolver{_io_context};
  asio::ip::tcp::resolver::query query{address, std::to_string(port)};

  try {
    asio::ip::tcp::resolver::iterator it{resolver.resolve(query)};
    asio::ip::tcp::resolver::iterator end;

    std::error_code err{std::make_error_code(std::errc::host_unreachable)};

    // it can resolve to multiple addresses like ipv4 and ipv6
    // we need to try all to find the first available socket
    while (err && it != end) {
      _tcp_socket->connect(*it, err);

      if (err)
        _tcp_socket->close();

      ++it;
    }

    if (err) {
      broker::exceptions::msg e;
      e << "RRD: could not connect to remote server '" << address << ":" << port
        << "': " << err.message();
      _tcp_socket.reset();
      throw e;
    }

    asio::socket_base::keep_alive option{true};
    _tcp_socket->set_option(option);
  } catch (std::system_error const& se) {
    broker::exceptions::msg e;
    e << "RRD: could not resolve remote server '" << address << ":" << port
      << "': " << se.what();
    _tcp_socket.reset();
    throw(e);
  }
  return;
}

/**
 *  Open a RRD file which already exists.
 *
 *  @param[in] filename Path to the RRD file.
 */
void cached::open(std::string const& filename) {
  // Close previous file.
  this->close();

  // Check that the file exists.
  if (access(filename.c_str(), F_OK))
    throw(exceptions::open()
          << "RRD: file '" << filename << "' does not exist");

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
void cached::open(std::string const& filename,
                  unsigned int length,
                  time_t from,
                  unsigned int step,
                  short value_type) {
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
 *  Remove the RRD file.
 *
 *  @param[in] filename Path to the RRD file.
 */
void cached::remove(std::string const& filename) {
  // Build rrdcached command.
  std::ostringstream oss;
  oss << "FORGET " << filename << "\n";

  try {
    if (_type == cached::tcp)
      _send_to_cached<std::shared_ptr<ip::tcp::socket>&>(oss.str(),
                                                         _tcp_socket);
    else
      _send_to_cached<std::shared_ptr<local::stream_protocol::socket>&>(
          oss.str(), _local_socket);
  } catch (broker::exceptions::msg const& e) {
    logging::error(logging::medium) << e.what();
  }

  if (::remove(filename.c_str())) {
    char const* msg(strerror(errno));
    logging::error(logging::high)
        << "RRD: could not remove file '" << filename << "': " << msg;
  }
}

/**
 *  Update the RRD file with new value.
 *
 *  @param[in] t     Timestamp of value.
 *  @param[in] value Associated value.
 */
void cached::update(time_t t, std::string const& value) {
  // Build rrdcached command.
  std::ostringstream oss;
  oss << "UPDATE " << _filename << " " << t << ":" << value << "\n";

  // Send command.
  logging::debug(logging::high)
      << "RRD: updating file '" << _filename << "' (" << oss.str() << ")";
  try {
    if (_type == cached::tcp)
      _send_to_cached<std::shared_ptr<ip::tcp::socket>&>(oss.str(),
                                                         _tcp_socket);
    else
      _send_to_cached<std::shared_ptr<local::stream_protocol::socket>&>(
          oss.str(), _local_socket);
  } catch (broker::exceptions::msg const& e) {
    if (!strstr(e.what(), "illegal attempt to update using time"))
      throw(exceptions::update() << e.what());
    else
      logging::error(logging::low) << "RRD: ignored update error in file '"
                                   << _filename << "': " << e.what() + 5;
  }
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Send data to rrdcached.
 *
 *  @param[in] command Command to send.
 *  @param[in] size    Size of command. If 0, set to strlen(command).
 */
template <typename T>
void cached::_send_to_cached(std::string const& command, T const& socket) {
  std::error_code err;

  // Check socket.
  if (!socket.get())
    throw(broker::exceptions::msg()
          << "RRD: attempt to communicate "
             "with rrdcached without connecting first");

  asio::write(*socket, asio::buffer(command), asio::transfer_all(), err);

  if (err)
    throw broker::exceptions::msg() << "RRD: error while sending "
                                       "command to rrdcached: "
                                    << err.message();

  // Read response.
  if (!_batch) {
    asio::streambuf stream;
    std::string line;

    asio::read_until(*socket, stream, '\n', err);

    if (err)
      throw(broker::exceptions::msg() << "RRD: error while getting "
                                         "response from rrdcached: "
                                      << err.message());

    std::istream is(&stream);
    std::getline(is, line);

    int lines = std::stoi(line);

    if (lines < 0)
      throw(broker::exceptions::msg()
            << "RRD: rrdcached query failed on file '" << _filename << "' ("
            << command << "): " << line);
    while (lines > 0) {
      asio::read_until(*socket, stream, '\n', err);
      if (err)
        throw(broker::exceptions::msg() << "RRD: error while getting "
                                        << "response from rrdcached for file '"
                                        << _filename << "': " << err.message());

      std::istream is(&stream);
      std::getline(is, line);
      --lines;
    }
  }
}
