/*
** Copyright 2012 Merethis
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

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/stats/worker.hh"

using namespace com::centreon::broker::stats;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
worker::worker() : _fd(-1) {}

/**
 *  Destructor.
 */
worker::~worker() throw () {}

/**
 *  Set the exit flag.
 */
void worker::exit() {
  _should_exit = true;
  return ;
}

/**
 *  Run the statistics thread.
 *
 *  @param[in] fifo_file Path to the FIFO file.
 */
void worker::run(QString const& fifo_file) {
  // Close FD.
  _close();

  // Set FIFO file.
  _fifo = fifo_file;

  // Open FIFO.
  _open();

  // Set exit flag.
  _should_exit = false;

  // Launch thread.
  start();

  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
worker::worker(worker const& right) : QThread() {
  _internal_copy(right);
}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
worker& worker::operator=(worker const& right) {
  _internal_copy(right);
  return (*this);
}

/**
 *  Close FIFO fd.
 */
void worker::_close() {
  if (_fd >= 0) {
    close(_fd);
    _fd = -1;
  }
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void worker::_internal_copy(worker const& right) {
  (void)right;
  assert(!"statistics worker is not copyable");
  abort();
  return ;
}

/**
 *  Open FIFO.
 */
void worker::_open() {
  _fd = open(qPrintable(_fifo), O_WRONLY);
  if (_fd < 0) {
    char const* msg(strerror(errno));
    throw (exceptions::msg() << "cannot open FIFO file: " << msg);
  }
  return ;
}

/**
 *  Thread entry point.
 */
void worker::run() {
  try {
    while (!_should_exit) {
      // FD sets.
      fd_set e;
      fd_set w;
      FD_ZERO(&e);
      FD_ZERO(&w);
      FD_SET(_fd, &e);
      FD_SET(_fd, &w);

      // Timeout.
      struct timeval timeout;
      memset(&timeout, 0, sizeof(timeout));
      timeout.tv_sec = 1;

      // Multiplexing.
      int flagged(select(_fd + 1, NULL, &w, &e, &timeout));

      // Error.
      if (flagged < 0) {
        // Unrecoverable.
        if (errno != EINTR) {
          char const* msg(strerror(errno));
          throw (exceptions::msg() << "multiplexing failure: " << msg);
        }
      }
      // FD error.
      else if (FD_ISSET(_fd, &e))
        throw (exceptions::msg() << "FIFO fd has pending error");
      // Readable.
      else if (FD_ISSET(_fd, &w)) {
        if (_buffer.empty())
          // Generate statistics.
          // XXX
          _buffer = "foo bar 42\n";

        // Write data.
        ssize_t wb(write(_fd, _buffer.c_str(), _buffer.size()));
        if (wb > 0)
          _buffer.erase(0, wb);

        // Buffer empty, reopen FIFO.
        if (_buffer.empty()) {
          _close();
          usleep(500);
          _open();
        }
      }
    }
  }
  catch (std::exception const& e) {
    logging::error(logging::high)
      << "stats: thread will exit due to the following error: "
      << e.what();
  }
  catch (...) {
    logging::error(logging::high)
      << "stats: thread will exit due to an unknown error";
  }
  return ;
}
