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
#include <QFile>
#if QT_VERSION >= 0x040400
#  include <QLocalSocket>
#endif // Qt >= 4.4.0
#include <QTcpSocket>
#include <sstream>
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
  return ;
}

/**
 *  Clear the tempalte cache.
 */
void cached::clean() {
  _lib.clean();
  return ;
}

/**
 *  Close the current RRD file.
 */
void cached::close() {
  _filename.clear();
  _batch = false;
  return ;
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
  return ;
}

#if QT_VERSION >= 0x040400
/**
 *  Connect to a local socket.
 *
 *  @param[in] name Socket name.
 */
void cached::connect_local(QString const& name) {
  // Create socket object.
  QLocalSocket* ls(new QLocalSocket);
  _socket.reset(ls);

  // Connect to server.
  ls->connectToServer(name);
  if (!ls->waitForConnected(-1)) {
    broker::exceptions::msg e;
    e << "RRD: could not connect to local socket '" << name
      << ": " << ls->errorString();
    _socket.reset();
    throw (e);
  }

  return ;
}
#endif // Qt >= 4.4.0

/**
 *  Connect to a remote server.
 *
 *  @param[in] address Server address.
 *  @param[in] port    Port to connect to.
 */
void cached::connect_remote(
               QString const& address,
               unsigned short port) {
  // Create socket object.
  QTcpSocket* ts(new QTcpSocket);
  _socket.reset(ts);

  // Connect to server.
  ts->connectToHost(address, port);
  if (!ts->waitForConnected(-1)) {
    broker::exceptions::msg e;
    e << "RRD: could not connect to remote server '" << address
      << ":" << port << "': " << ts->errorString();
    _socket.reset();
    throw (e);
  }

  return ;
}

/**
 *  Open a RRD file which already exists.
 *
 *  @param[in] filename Path to the RRD file.
 */
void cached::open(QString const& filename) {
  // Close previous file.
  this->close();

  // Check that the file exists.
  if (!QFile::exists(filename))
    throw (exceptions::open() << "RRD: file '" << filename
             << "' does not exist");

  // Remember information for further operations.
  _filename = filename;

  return ;
}

/**
 *  Open a RRD file and create it if it does not exists.
 *
 *  @param[in] filename   Path to the RRD file.
 *  @param[in] length     Number of recording in the RRD file.
 *  @param[in] from       Timestamp of the first record.
 *  @param[in] step       Time interval between each record.
 *  @param[in] value_type Type of the metric.
 */
void cached::open(
               QString const& filename,
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

  return ;
}

/**
 *  Remove the RRD file.
 *
 *  @param[in] filename Path to the RRD file.
 */
void cached::remove(QString const& filename) {
  // Build rrdcached command.
  std::ostringstream oss;
  oss << "FORGET " << filename.toStdString() << "\n";

  try {
    _send_to_cached(oss.str().c_str());
  }
  catch (broker::exceptions::msg const& e) {
    logging::error(logging::medium) << e.what();
  }

  if (::remove(filename.toStdString().c_str())) {
    char const* msg(strerror(errno));
    logging::error(logging::high) << "RRD: could not remove file '"
      << filename << "': " << msg;
  }
  return ;
}

/**
 *  Update the RRD file with new value.
 *
 *  @param[in] t     Timestamp of value.
 *  @param[in] value Associated value.
 */
void cached::update(time_t t, QString const& value) {
  // Build rrdcached command.
  std::ostringstream oss;
  oss << "UPDATE " << _filename.toStdString() << " " << t
      << ":" << value.toStdString() << "\n";

  // Send command.
  try {
    _send_to_cached(oss.str().c_str());
  }
  catch (broker::exceptions::msg const& e) {
    if (!strstr(e.what(), "illegal attempt to update using time"))
      throw (exceptions::update() << e.what());
    else
      logging::error(logging::low)
        << "RRD: ignored update error: " << e.what() + 5;
  }

  return ;
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
             << "RRD: rrdcached query failed (" << command << "): "
             << line);
    while (lines > 0) {
      _socket->waitForReadyRead(-1);
      if (_socket->readLine(line, sizeof(line)) < 0)
        throw (broker::exceptions::msg() << "RRD: error while getting" \
                    " response from rrdcached: "
                 << _socket->errorString());
      --lines;
    }
  }

  return ;
}
