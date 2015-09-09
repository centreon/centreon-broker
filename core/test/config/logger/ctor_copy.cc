/*
** Copyright 2011-2012 Centreon
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

#include "com/centreon/broker/config/logger.hh"

using namespace com::centreon::broker;

/**
 *  Check that the logger configuration class can be copied properly.
 *
 *  @return 0 on success.
 */
int main() {
  // First object.
  config::logger l1;
  l1.config(false);
  l1.debug(true);
  l1.error(true);
  l1.info(false);
  l1.level(logging::medium);
  l1.max_size(1456354874ull);
  l1.name("FooBar");
  l1.type(config::logger::file);

  // Second object.
  config::logger l2(l1);

  // Change first object.
  l1.config(true);
  l1.debug(false);
  l1.error(false);
  l1.info(true);
  l1.level(logging::low);
  l1.max_size(484587188ull);
  l1.name("BazQux");
  l1.type(config::logger::standard);

  // Check.
  return ((!l1.config())
          || (l1.debug())
          || (l1.error())
          || (!l1.info())
          || (l1.level() != logging::low)
          || (l1.max_size() != 484587188ull)
          || (l1.name() != "BazQux")
          || (l1.type() != config::logger::standard)
          || (l2.config())
          || (!l2.debug())
          || (!l2.error())
          || (l2.info())
          || (l2.level() != logging::medium)
          || (l2.max_size() != 1456354874ull)
          || (l2.name() != "FooBar")
          || (l2.type() != config::logger::file));
}
