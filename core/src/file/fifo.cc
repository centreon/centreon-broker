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

#include <cstring>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/file/fifo.hh"

#define BUF_SIZE 4096 * 4

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

/**
 *  Fifo constructor.
 *
 *  @param[in] path  Path of the fifo.
 */
fifo::fifo(std::string const& path) :
  _path(path) {
  _open_fifo();
}

/**
 *  Destructor.
 */
fifo::~fifo() {
  ::close(_fd);
  ::unlink(_path.c_str());
}

/**
 *  @brief Read a line of the fifo.
 *
 *  Will block if no line available until the timeout is exceeded.
 *
 *  @param[in] usecs_timeout  The timeout, in microseconds.
 *
 *  @return                   A line, or an empty string if
 *                            the timeout was exceeded.
 */
std::string fifo::read_line(int usecs_timeout) {
  // Check if a line was already buffered.
  size_t index;
  if ((index = _polled_line.find_first_of('\n')) != std::string::npos) {
    std::string ret = _polled_line.substr(0, index + 1);
    _polled_line.erase(0, index + 1);
    return (ret);
  }

  // Poll for a line.
  fd_set polled_fd;
  struct timeval tv;
  FD_ZERO(&polled_fd);
  FD_SET(_fd, &polled_fd);
  tv.tv_sec = usecs_timeout / 1000000;
  tv.tv_usec = usecs_timeout % 1000000;
  if (::select(
          _fd + 1,
          &polled_fd,
          NULL,
          NULL,
          ((usecs_timeout == -1) ? NULL : &tv)) == -1) {
    char const* msg = ::strerror(errno);
    throw (exceptions::msg()
           << "fifo: can't poll file '" << _path << "': " << msg);
  }

  // Read everything.
  char buf[BUF_SIZE];
  int ret = ::read(_fd, buf, BUF_SIZE - 1);
  if (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
    return ("");
  if (ret == -1) {
    const char* msg = ::strerror(errno);
    throw (exceptions::msg()
           << "fifo: can't read file '" << _path << "': " << msg);
  }
  buf[ret] = '\0';
  _polled_line.append(buf);

  // Check if a line was buffered.
  if ((index = _polled_line.find_first_of('\n')) != std::string::npos) {
    std::string ret = _polled_line.substr(0, index + 1);
    _polled_line.erase(0, index + 1);
    return (ret);
  }
  return ("");
}

/**
 *  Open the fifo.
 */
void fifo::_open_fifo()  {
  // Does file exist and is a FIFO ?
  struct stat s;
  // Stat failed, probably because of inexistant file.
  if (::stat(_path.c_str(), &s) != 0) {
    char const* msg(strerror(errno));
    logging::config(logging::medium) << "stats: cannot stat() '"
      << _path << "': " << msg;

    // Create FIFO.
    if (::mkfifo(
          _path.c_str(),
          S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)
        != 0) {
      char const* msg(strerror(errno));
      throw (exceptions::msg()
             << "fifo: can't create fifo '" << _path << "' :" << msg);
    }
  }
  else if (!S_ISFIFO(s.st_mode))
    throw (exceptions::msg() <<  "fifo: file '" << _path
           << "' exists but is not a FIFO");

  // Open fifo.
  // We use O_RDWR because select flag a FIFO at EOF when there is
  // no more data - but later writers can make data available.
  // When using O_RDWR, this flagging never happen, as there is always
  // at least one writer.
  _fd = ::open(_path.c_str(), O_RDWR | O_NONBLOCK);
  if (_fd == -1) {
    const char* msg(::strerror(errno));
    throw (exceptions::msg()
           << "fifo: can't open file '" << _path
           << "' : " << msg);
  }
}
