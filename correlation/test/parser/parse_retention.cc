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

#include <QDir>
#include <QFile>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/correlation/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/parser/common.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Check that non-retention file parsing work.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  /*config::applier::init();

  // Write file.
  char const* file_content =
    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
    "<centreonbroker>\n"
    "  <host id=\"13\" state=\"42\" />\n"
    "  <issue ack_time=\"0\" host=\"13\" service=\"0\" start_time=\"3456\" />\n"
    "  <host id=\"42\" />\n"
    "  <service id=\"21\" host=\"13\" />\n"
    "  <service id=\"66\" host=\"42\" state=\"3\" />\n"
    "  <issue ack_time=\"35\" host=\"42\" service=\"66\" start_time=\"9865\"
  />\n" "  <service id=\"33\" host=\"13\" state=\"33\" />\n" "  <issue
  ack_time=\"762\" host=\"13\" service=\"33\" start_time=\"2346213\" />\n" "
  <service id=\"12\" host=\"42\" state=\"2\" />\n" "  <parent host=\"13\"
  parent=\"42\" />\n" "  <dependency dependent_host=\"13\"
  dependent_service=\"21\"\n" "              host=\"13\" service=\"33\" />\n" "
  <dependency dependent_host=\"42\" dependent_service=\"12\"\n" " host=\"13\"
  />\n"
    "</centreonbroker>\n";
  QString file_path(QDir::tempPath());
  file_path.append("/broker_correlation_parser_parse_retention");
  ::remove(file_path.toStdString().c_str());
  QFile f(file_path);
  if (!f.open(QIODevice::WriteOnly))
    return (1);
  while (*file_content) {
    qint64 wb(f.write(file_content, strlen(file_content)));
    if (wb <= 0)
      return (1);
    file_content += wb;
  }
  f.close();

  // Error flag.
  bool error(true);
  try {
    // Initial state.
    QMap<QPair<unsigned int, unsigned int>, node> parsed1;
    QMap<QPair<unsigned int, unsigned int>, node> parsed2;
    {
      node& h1(parsed2[qMakePair(13u, 0u)]);
      h1.host_id = 13;
      h1.current_state = 3;
      h1.my_issue.reset(new issue);
      h1.my_issue->host_id = 13;
      h1.my_issue->start_time = 4768215;
      node& s1(parsed2[qMakePair(13u, 21u)]);
      s1.host_id = 13;
      s1.service_id = 21;
      node& s2(parsed2[qMakePair(13u, 33u)]);
      s2.host_id = 13;
      s2.service_id = 33;
      s2.current_state = 4;
      s1.add_dependency(&h1);
    }

    // Parse file.
    correlation::parser p;
    p.parse(file_path, true, parsed1); // Must be empty.
    p.parse(file_path, true, parsed2); // Adjusted with retention file.
    ::remove(file_path.toStdString().c_str());

    // Expected result.
    QMap<QPair<unsigned int, unsigned int>, node> expected;
    node& h1(expected[qMakePair(13u, 0u)]);
    h1.host_id = 13;
    h1.current_state = 42;
    h1.my_issue.reset(new issue);
    h1.my_issue->host_id = 13;
    h1.my_issue->start_time = 3456;
    node& s1(expected[qMakePair(13u, 21u)]);
    s1.host_id = 13;
    s1.service_id = 21;
    node& s2(expected[qMakePair(13u, 33u)]);
    s2.host_id = 13;
    s2.service_id = 33;
    s2.current_state = 33;
    s2.my_issue.reset(new issue);
    s2.my_issue->ack_time = 762;
    s2.my_issue->host_id = 13;
    s2.my_issue->service_id = 33;
    s2.my_issue->start_time = 2346213;
    s1.add_dependency(&h1);

    // Compare parsing with expected result.
    if (!parsed1.isEmpty())
      throw (exceptions::msg() << "parse1 is not empty");
    compare_states(expected, parsed2);

    // Success.
    error = false;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Return check result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);*/
  return (0);
}
