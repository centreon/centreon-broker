/*
** Copyright 2013-2015 Centreon
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

#include <cstdlib>
#include <exception>
#include <iostream>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/correlation/parser.hh"
#include "test/parser/common.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Check that parsing of file with 'include' directive works.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Error flag.
  bool error(true);
  try {
    // Parse file.
    QMap<QPair<uint32_t, uint32_t>, node> parsed;
    correlation::parser p;
    p.parse(PROJECT_SOURCE_DIR "/correlation/test/parser/parse_include.xml",
            parsed);

    // Expected result.
    QMap<QPair<uint32_t, uint32_t>, node> expected;
    node& h1(expected[qMakePair(13u, 0u)]);
    h1.host_id = 13;
    h1.current_state = 1;
    node& s1(expected[qMakePair(13u, 21u)]);
    s1.host_id = 13;
    s1.service_id = 21;
    node& s2(expected[qMakePair(13u, 12u)]);
    s2.host_id = 13;
    s2.service_id = 12;
    s2.current_state = 2;
    node& h2(expected[qMakePair(42u, 0u)]);
    h2.host_id = 42;
    node& s3(expected[qMakePair(42u, 66u)]);
    s3.host_id = 42;
    s3.service_id = 66;
    s3.current_state = 3;
    s1.add_dependency(&s2);
    s1.add_dependency(&h1);
    s2.add_dependency(&h1);
    s3.add_dependency(&h2);

    // Compare parsing with expected result.
    compare_states(parsed, expected);

    // Success.
    error = false;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Return.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
