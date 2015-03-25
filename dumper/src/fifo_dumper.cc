/*
** Copyright 2013 Merethis
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

#include <QMutexLocker>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include "com/centreon/broker/dumper/dump.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/dumper/fifo_dumper.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"

#define BUF_SIZE 4096 * 4

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

extern unsigned int instance_id;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] path    Dumper path.
 *  @param[in] tagname Dumper tagname.
 */
fifo_dumper::fifo_dumper(
          std::string const& path,
          std::string const& tagname)
  : _path(path),
    _process_in(true),
    _process_out(true),
    _tagname(tagname),
    _file(-1) {
  _open_fifo();
}

/**
 *  Destructor.
 */
fifo_dumper::~fifo_dumper() {
  ::close(_file);
  ::unlink(_path.c_str());
}

/**
 *  Set processing flags.
 *
 *  @param[in] in  Set to true to process input events.
 *  @param[in] out Set to true to process output events.
 */
void fifo_dumper::process(bool in, bool out) {
  _process_in = in;
  _process_out = out;
  return ;
}

/**
 *  Read data from the dumper.
 *
 *  @param[out] d Bunch of data.
 */
void fifo_dumper::read(misc::shared_ptr<io::data>& d) {
  d.clear();

  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
           << "directory dumper stream is shutdown");

  // Get a line if a line was already polled.
  size_t index;
  if ((index = _polled_line.find_first_of('\n')) != std::string::npos) {
    misc::shared_ptr<dump> dmp(new dump);
    dmp->content = QString::fromStdString(_polled_line.substr(0, index + 1));
    dmp->filename = QString::fromStdString(_path);
    dmp->tag = QString::fromStdString(_tagname);
    dmp->instance_id = instance_id;
    _polled_line.erase(0, index + 1);
    d = dmp;
    return ;
  }

  logging::debug(logging::medium)
    << "dumper: fifo dumper polling "  << _path;

  // Poll for a line.
  fd_set polled_fd;
  struct timeval tv;
  FD_ZERO(&polled_fd);
  FD_SET(_file,  &polled_fd);
  tv.tv_sec = 3;
  tv.tv_usec = 0;
  if (::select(_file + 1, &polled_fd, NULL, NULL, &tv) == -1) {
    const char* msg = ::strerror(errno);
    throw (exceptions::msg()
           << "dumper: can't poll file '" << _path
           << "' for the fifo dumper: " << msg);
  }

  // Read everything.
  char buf[BUF_SIZE];
  int ret = ::read(_file, buf, BUF_SIZE - 1);
  if (ret == -1 && errno == EAGAIN)
    return ;
  if (ret == -1) {
    const char* msg = ::strerror(errno);
    throw (exceptions::msg()
           << "dumper: can't read file '" << _path
           << "' for the fifo dumper: " << msg);
  }
  buf[ret] = '\0';
  _polled_line.append(buf);

  logging::debug(logging::medium)
    << "dumper: fifo dumper read " << ret << " bytes";

  return ;
}

/**
 *  Write data to the dumper.
 *
 *  @param[in] d Data to write.
 *
 *  @return Always return 1, or throw exceptions.
 */
unsigned int fifo_dumper::write(misc::shared_ptr<io::data> const& d) {
  throw (exceptions::msg()
         << "dumper: attempt to write from a fifo dumper stream");

  return (1);
}

/**
 *  Open the fifo file.
 */
void fifo_dumper::_open_fifo() {
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
             << "dumper: can't create fifo '" << _path
             << "' for the fifo dumper: " << msg);
    }
  }
  else if (!S_ISFIFO(s.st_mode))
    throw (exceptions::msg() <<  "dumper: file '" << _path
           << "' exists but is not a FIFO");

  // Open fifo.
  // We use O_RDWR because select flag a FIFO at EOF when there was
  // no more data - but later writers can make data available.
  // When using O_RDWR, this flagging never happen.
  _file = ::open(_path.c_str(), O_RDWR | O_NONBLOCK);
  if (_file == -1) {
    const char* msg(::strerror(errno));
    throw (exceptions::msg()
           << "dumper: can't open file '" << _path
           << "' for the fifo dumper: " << msg);
  }
}
