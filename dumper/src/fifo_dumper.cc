/*
** Copyright 2015,2017 Centreon
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
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
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
bool fifo_dumper::read(std::shared_ptr<io::data>& d, time_t deadline) {
  d.reset();
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
      std::shared_ptr<dumper::dump> dmp(new dumper::dump);
      dmp->content = line;
      dmp->filename = _path;
      dmp->tag = _tagname;
      d = dmp;
    }
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "dumper: error while trying to read fifo: " << e.what());
  }
  return !line.empty();
}

/**
 *  Write data to the dumper.
 *
 *  @param[in] d Data to write.
 *
 *  @return Always return 1, or throw exceptions.
 */
int fifo_dumper::write(std::shared_ptr<io::data> const& d) {
  if (d)
    throw (exceptions::shutdown()
           << "cannot write to FIFO dumper '" << _path << "'");
  return 1;
}
