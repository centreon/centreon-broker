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

#include <cerrno>
#include <cstdlib>
#include <QTcpSocket>
#if QT_VERSION >= 0x040400
#  include <QLocalSocket>
#endif // Qt >= 4.4.0
#include <sstream>
#include <unistd.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/rrd/cached.hh"
#include "com/centreon/broker/rrd/exceptions/open.hh"
#include "com/centreon/broker/rrd/exceptions/update.hh"
#include "com/centreon/broker/rrd/lib.hh"

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
  : _batch(false),
    _lib(tmpl_path, cache_size) {}

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
  char const buffer[] = "BATCH\n";
  _send_to_cached(buffer, sizeof(buffer) - 1);
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
    char buffer[] = ".\n";
    _send_to_cached(buffer, sizeof(buffer) - 1);
  }
}

#if QT_VERSION >= 0x040400
/**
 *  Connect to a local socket.
 *
 *  @param[in] name Socket name.
 */
void cached::connect_local(std::string const& name) {
  // Create socket object.
  QLocalSocket* ls(new QLocalSocket);
  _socket.reset(ls);

  // Connect to server.
  ls->connectToServer(QString::fromStdString(name));
  if (!ls->waitForConnected(-1)) {
    broker::exceptions::msg e;
    e << "RRD: could not connect to local socket '" << name
      << ": " << ls->errorString();
    _socket.reset();
    throw (e);
  }
}
#endif // Qt >= 4.4.0

/**
 *  Connect to a remote server.
 *
 *  @param[in] address Server address.
 *  @param[in] port    Port to connect to.
 */
void cached::connect_remote(
               std::string const& address,
               unsigned short port) {
  // Create socket object.
  QTcpSocket* ts(new QTcpSocket);
  _socket.reset(ts);

  // Connect to server.
  ts->connectToHost(QString::fromStdString(address), port);
  if (!ts->waitForConnected(-1)) {
    broker::exceptions::msg e;
    e << "RRD: could not connect to remote server '" << address
      << ":" << port << "': " << ts->errorString();
    _socket.reset();
    throw (e);
  }

  // Set the SO_KEEPALIVE option.
  ts->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
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
    throw (exceptions::open() << "RRD: file '" << filename
             << "' does not exist");

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
void cached::open(
               std::string const& filename,
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
    _send_to_cached(oss.str().c_str());
  }
  catch (broker::exceptions::msg const& e) {
    logging::error(logging::medium) << e.what();
  }

  if (::remove(filename.c_str())) {
    char const* msg(strerror(errno));
    logging::error(logging::high) << "RRD: could not remove file '"
      << filename << "': " << msg;
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
  oss << "UPDATE " << _filename << " " << t
      << ":" << value << "\n";

  // Send command.
  logging::debug(logging::high) << "RRD: updating file '"
    << _filename << "' (" << oss.str() << ")";
  try {
    _send_to_cached(oss.str().c_str());
  }
  catch (broker::exceptions::msg const& e) {
    if (!strstr(e.what(), "illegal attempt to update using time"))
      throw (exceptions::update() << e.what());
    else
      logging::error(logging::low)
        << "RRD: ignored update error in file '"
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
void cached::_send_to_cached(
               char const* command,
               unsigned int size) {
  // Check socket.
  if (!_socket.get())
    throw (broker::exceptions::msg() << "RRD: attempt to communicate " \
             "with rrdcached without connecting first");

  // Check command size.
  if (!size)
    size = strlen(command);

  // Write data.
  while (size > 0) {
    qint64 rb;
    rb = _socket->write(command, size);
    if (rb < 0)
      throw (broker::exceptions::msg() << "RRD: error while sending " \
               "command to rrdcached: " << _socket->errorString());
    size -= rb;
  }

  // Read response.
  if (!_batch) {
    _socket->waitForBytesWritten(-1);
    char line[1024];
    _socket->waitForReadyRead(-1);
    if (_socket->readLine(line, sizeof(line)) < 0)
      throw (broker::exceptions::msg() << "RRD: error while getting " \
               "response from rrdcached: " << _socket->errorString());
    int lines;
    lines = strtol(line, NULL, 10);
    if (lines < 0)
      throw (broker::exceptions::msg()
             << "RRD: rrdcached query failed on file '"
             << _filename << "' (" << command << "): "
             << line);
    while (lines > 0) {
      _socket->waitForReadyRead(-1);
      if (_socket->readLine(line, sizeof(line)) < 0)
        throw (broker::exceptions::msg() << "RRD: error while getting "
               << "response from rrdcached for file '" << _filename
               << "': " << _socket->errorString());
      --lines;
    }
  }
}
