/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <memory>
#include <vector>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/stats/worker_pool.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::stats;

worker_pool::worker_pool() {}

void worker_pool::add_worker(std::string const& fifo) {
  // Does file exist and is a FIFO ?
  struct stat s;
  std::string fifo_path = fifo;
  // Stat failed, probably because of inexistant file.
  if (stat(fifo_path.c_str(), &s) != 0) {
    char const* msg(strerror(errno));
    logging::config(logging::medium)
        << "stats: cannot stat() '" << fifo_path << "': " << msg;

    // Create FIFO.
    if (mkfifo(fifo_path.c_str(),
               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH) != 0) {
      char const* msg(strerror(errno));
      throw(exceptions::msg()
            << "cannot create FIFO '" << fifo_path << "': " << msg);
    }
  } else if (!S_ISFIFO(s.st_mode))
    throw(exceptions::msg()
          << "file '" << fifo_path << "' exists but is not a FIFO");

  // Create thread.
  _workers_fifo.push_back(std::make_shared<stats::worker>());
  _workers_fifo.back()->run(fifo_path);
}

void worker_pool::cleanup() {
  for (std::vector<std::shared_ptr<stats::worker> >::iterator
           it = _workers_fifo.begin(),
           end = _workers_fifo.end();
       it != end; ++it) {
    (*it)->exit();
    (*it)->wait();
    it->reset();
  }
}
