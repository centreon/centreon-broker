/*
** Copyright 2015 Merethis
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
  try :
    _path(path),
    _tagname(tagname),
    _fifo(_path) {}
catch (std::exception const& e) {
  throw (exceptions::msg() << "dumper: error in fifo dumper initialization: "
                           << e.what());
}

/**
 *  Destructor.
 */
fifo_dumper::~fifo_dumper() {}

/**
 *  Read data from the dumper.
 *
 *  @param[out] d Next available event.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool fifo_dumper::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  d.clear();
  std::string line;
  try {
    time_t now(time(NULL));
    int timeout;
    if (deadline == (time_t)-1)
      timeout = -1;
    else if (now >= deadline)
      timeout = 0;
    else
      timeout = (deadline - now) * 1000000;
    line = _fifo.read_line(timeout);
    if (!line.empty()) {
      misc::shared_ptr<dumper::dump> dmp(new dumper::dump);
      dmp->content = QString::fromStdString(line);
      dmp->filename = QString::fromStdString(_path);
      dmp->tag = QString::fromStdString(_tagname);
      d = dmp;
    }
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "dumper: error while trying to read fifo: " << e.what());
  }
  return (!line.empty());
}

/**
 *  Write data to the dumper.
 *
 *  @param[in] d Data to write.
 *
 *  @return Always return 1, or throw exceptions.
 */
unsigned int fifo_dumper::write(misc::shared_ptr<io::data> const& d) {
  (void)d;
  throw (io::exceptions::shutdown(false, true)
         << "cannot write to a FIFO dumper");
  return (1);
}
