/*
** Copyright 2011-2012 Merethis
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

#include <QDir>
#include <QFile>
#include <QString>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/correlation/correlator.hh"
#include "com/centreon/broker/correlation/parser.hh"
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "test/parser/common.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Check that retention work with an empty initial retention file.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
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
  config_path.append("/centreon_broker_unit_test1");
  QFile::remove(config_path);
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
  QString retention_path(QDir::tempPath());
  retention_path.append("/centreon_broker_unit_test2");
  QFile::remove(retention_path);

  // Correlator.
  correlator c;
  c.load(config_path, retention_path);

  // Submit state change.
  {
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 13;
    ss->service_id = 21;
    ss->state_type = 1;
    ss->current_state = 2;
    c.write(ss.staticCast<io::data>());
  }
  {
    misc::shared_ptr<neb::host_status> hs(new neb::host_status);
    hs->host_id = 42;
    hs->state_type = 1;
    hs->current_state = 2;
    c.write(hs.staticCast<io::data>());
  }
  {
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 42;
    ss->service_id = 12;
    ss->state_type = 1;
    ss->current_state = 2;
    c.write(ss.staticCast<io::data>());
  }

  // Dump retention file.
  c.stopping();

  // Read retention state.
  QMap<QPair<unsigned int, unsigned int>, node> retained;
  parser p;
  p.parse(config_path, false, retained);
  p.parse(retention_path, true, retained);

  // Delete temporary files.
  QFile::remove(config_path);
  QFile::remove(retention_path);

  // Compare current with retained state.
  return (!compare_states(c.get_state(), retained));
}
