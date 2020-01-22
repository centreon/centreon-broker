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

#include "com/centreon/broker/file/internal.hh"
#include "com/centreon/broker/file/factory.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

void file::unload() {
  // Unregister file layer.
  io::protocols::instance().unreg("file");
}

void file::load() {
  // File module.
  logging::info(logging::high)
      << "file: module for Centreon Broker " << CENTREON_BROKER_VERSION;

  // Register file layer.
  io::protocols::instance().reg("file", std::make_shared<file::factory>(), 1,
                                3);
}
