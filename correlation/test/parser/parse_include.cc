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
    QMap<QPair<unsigned int, unsigned int>, node> parsed;
    correlation::parser p;
    p.parse(
        PROJECT_SOURCE_DIR "/correlation/test/parser/parse_include.xml",
        false,
        parsed);

    // Expected result.
    QMap<QPair<unsigned int, unsigned int>, node> expected;
    node& h1(expected[qMakePair(13u, 0u)]);
    h1.host_id = 13;
    h1.since = 789;
    h1.state = 1;
    node& s1(expected[qMakePair(13u, 21u)]);
    s1.host_id = 13;
    s1.service_id = 21;
    s1.since = 456;
    node& s2(expected[qMakePair(13u, 12u)]);
    s2.host_id = 13;
    s2.service_id = 12;
    s2.since = 666;
    s2.state = 2;
    node& h2(expected[qMakePair(42u, 0u)]);
    h2.host_id = 42;
    node& s3(expected[qMakePair(42u, 66u)]);
    s3.host_id = 42;
    s3.service_id = 66;
    s3.state = 3;
    s1.add_dependency(&s2);
    s1.add_dependency(&h1);
    s2.add_dependency(&h1);
    s3.add_dependency(&h2);

    // Compare parsing with expected result.
    compare_states(parsed, expected);

    // Success.
    error = false;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Return.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
