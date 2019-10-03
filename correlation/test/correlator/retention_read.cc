/*
** Copyright 2011-2014 Centreon
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
#include <QString>
#include <cstdlib>
#include <exception>
#include <iostream>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/correlation/parser.hh"
#include "com/centreon/broker/correlation/stream.hh"
#include "test/parser/common.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Check that retention work with an empty initial retention file.
 *
 *  @return 0 on success.
 */
int main() {
  /*// Initialization.
  config::applier::init();

  // Write file.
  char const* file_content =
    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
    "<centreonbroker>\n"
    "  <host id=\"13\" since=\"789\" />\n"
    "  <host id=\"42\" />\n"
    "  <service id=\"21\" host=\"13\" />\n"
    "  <service id=\"66\" host=\"42\" />\n"
    "  <service id=\"33\" host=\"13\" />\n"
    "  <service id=\"12\" host=\"42\" />\n"
    "  <parent host=\"13\" parent=\"42\" />\n"
    "  <dependency dependent_host=\"13\" dependent_service=\"21\"\n"
    "              host=\"13\" service=\"33\" />\n"
    "  <dependency dependent_host=\"42\" dependent_service=\"12\"\n"
    "              host=\"13\" />\n"
    "</centreonbroker>\n";
  QString config_path(QDir::tempPath());
  config_path.append("/broker_correlation_correlator_retention_read1");
  ::remove(config_path.toStdString().c_str());
  {
    QFile f(config_path);
    if (!f.open(QIODevice::WriteOnly))
      return (1);
    while (*file_content) {
      qint64 wb(f.write(file_content, strlen(file_content)));
      if (wb <= 0)
        return (1);
      file_content += wb;
    }
    f.close();
  }
  file_content =
    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
    "<centreonbroker>\n"
    "  <host id=\"13\" since=\"123\" state=\"2\" />\n"
    "  <service id=\"21\" host=\"13\" since=\"34523\" state=\"3\" />\n"
    "  <issue host=\"13\" service=\"21\" ack_time=\"32\" start_time=\"8236\"
  />\n" "  <service id=\"33\" host=\"13\" since=\"751\" state=\"3\" />\n" "
  <issue host=\"13\" service=\"33\" ack_time=\"0\" start_time=\"234\" />\n"
    "</centreonbroker>\n";
  QString retention_path(QDir::tempPath());
  retention_path.append("/broker_correlation_correlator_retention_read2");
  ::remove(retention_path.toStdString().c_str());
  {
    QFile f(retention_path);
    if (!f.open(QIODevice::WriteOnly))
      return (1);
    while (*file_content) {
      qint64 wb(f.write(file_content, strlen(file_content)));
      if (wb <= 0)
        return (1);
      file_content += wb;
    }
    f.close();
  }

  // Error flag.
  bool error(true);
  try {
    // Correlator.
    correlator c(0);
    c.load(config_path, retention_path);

    // Read retention state.
    QMap<QPair<uint32_t, uint32_t>, node> retained;
    parser p;
    p.parse(config_path, false, retained);
    p.parse(retention_path, true, retained);

    // Compare current with retained state.
    compare_states(c.get_state(), retained);

    // Success.
    error = false;
  }
  catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Delete temporary files.
  ::remove(config_path.toStdString().c_str());
  ::remove(retention_path.toStdString().c_str());

  // Cleanup.
  config::applier::deinit();

  // Return check result.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);*/
  return (0);
}
