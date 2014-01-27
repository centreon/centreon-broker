/*
** Copyright 2012-2014 Merethis
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
#include <cstring>
#include <fcntl.h>
#include <iomanip>
#include <QFileInfo>
#include <QMutexLocker>
#include <poll.h>
#include <sstream>
#include <time.h>
#include <unistd.h>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/stats/builder.hh"
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
  _fifo = fifo_file.toStdString();

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
 *  Open FIFO.
 *
 *  @return true on success.
 */
bool worker::_open() {
  bool retval;
  _fd = open(_fifo.c_str(), O_WRONLY | O_NONBLOCK);
  if (_fd < 0) {
    if (errno != ENXIO) {
      char const* msg(strerror(errno));
      throw (exceptions::msg() << "cannot open FIFO file: " << msg);
    }
    else
      retval = false;
  }
  else
    retval = true;
  return (retval);
}

/**
 *  Thread entry point.
 */
void worker::run() {
  try {
    while (!_should_exit) {
      // Check file opening.
      if (_buffer.empty()) {
        _close();
        usleep(100000);
        if (!_open())
          continue ;
      }

      // FD sets.
      pollfd fds;
      fds.fd = _fd;
      fds.events = POLLOUT;
      fds.revents = 0;

      // Multiplexing.
      int flagged(poll(&fds, 1, 1000));

      // Error.
      if (flagged < 0) {
        // Unrecoverable.
        if (errno != EINTR) {
          char const* msg(strerror(errno));
          throw (exceptions::msg() << "multiplexing failure: " << msg);
        }
      }
      else if (flagged > 0) {
        // FD error.
        if ((fds.revents & (POLLERR | POLLNVAL | POLLHUP)))
          throw (exceptions::msg() << "FIFO fd has pending error");
        // Readable.
        else if ((fds.revents & POLLOUT)) {
          if (_buffer.empty()) {
            // Generate statistics.
            builder stats_builder;
            stats_builder.build();
            _buffer = stats_builder.data();
          }

          // Write data.
          ssize_t wb(write(_fd, _buffer.c_str(), _buffer.size()));
          if (wb > 0)
            _buffer.erase(0, wb);
          else
            _buffer.clear();
        }
      }
    }
  }
  catch (std::exception const& e) {
    logging::error(logging::high)
      << "stats: FIFO thread will exit due to the following error: "
      << e.what();
  }
  catch (...) {
    logging::error(logging::high)
      << "stats: FIFO thread will exit due to an unknown error";
  }
  ::unlink(_fifo.c_str());
  return ;
}
